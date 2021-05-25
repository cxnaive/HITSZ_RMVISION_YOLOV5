#include <armor_finder/armor_finder.h>
#include <camera/cam_wrapper.h>
#include <camera/video_wrapper.h>
#include <energy.h>
#include <glog/logging.h>
#include <rmconfig.h>
#include <rmserial.h>
#include <rmtime.h>

#include <csignal>
#include <opencv2/opencv.hpp>

using namespace std;
//检测关闭程序键盘中断
static volatile int keepRunning = 1;

void sig_handler(int sig) {
    if (sig == SIGINT) {
        keepRunning = 0;
    }
}

//程序运行时信息
RmConfig config;
//程序运行时时间类
RmTime rmTime;
//串口实例
RmSerial rmSerial;
//摄像头实例
Camera* cam = nullptr;
// Video实例
VideoWrapper* video = nullptr;
// ArmorFinder实例
ArmorFinder* armor_finder;
// Energy 实例
Energy* energy;
//运行时原图实例
cv::Mat src;
cv::Mat src_energy;
//保存上一次循环运行模式
char lastRunMode;
static void OnInit(const char* cmd) {
    FLAGS_alsologtostderr = true;
    FLAGS_colorlogtostderr = true;
    google::InitGoogleLogging(cmd);

    rmTime.init();
    config.init_from_file();
    rmSerial.init();
    src = cv::Mat(640, 640, CV_8UC3);
    if (config.use_video) {
        video = new VideoWrapper(config.video_path);
        video->init();
    } else {
        cam = new Camera(config.camera_sn, config.camConfig);
        cam->init();
        if (!cam->init_is_successful()) {
            LOG(ERROR) << "Camera Init Failed!";
            keepRunning = false;
            return;
        }
        cam->setParam(config.ARMOR_CAMERA_EXPOSURE, config.ARMOR_CAMERA_GAIN);
        cam->start();
    }

    armor_finder =
        new ArmorFinder(config.ENEMY_COLOR, rmSerial, config.ANTI_TOP);
    energy = new Energy(rmSerial, config.ENEMY_COLOR);
    lastRunMode = ARMOR_STATE;
}

static void OnClose() { config.write_to_file(); }

void update_config() {
    receive_mtx.lock();
    config.RUNMODE = receive_config_data.state;
    config.ENEMY_COLOR = receive_config_data.enemy_color;
    config.ANTI_TOP = receive_config_data.anti_top;
    config.MCU_DELTA_X = receive_config_data.delta_x;
    config.MCU_DELTA_Y = receive_config_data.delta_y;
    config.BULLET_SPEED = receive_config_data.bullet_speed;
    config.MCU_PITCH_SPEED = receive_config_data.curr_pitch;
    config.MCU_YAW_SPEED = receive_config_data.curr_yaw;
    receive_mtx.unlock();
}
void check_mode_and_run(cv::Mat& src) {
    // rmSerial.manual_receive();
    update_config();
    if (lastRunMode == ARMOR_STATE && (config.RUNMODE == SMALL_ENERGY_STATE ||
                                       config.RUNMODE == BIG_ENERGY_STATE)) {
        if (!config.use_video)
            cam->setEnergy(config.ENERGY_CAMERA_EXPOSURE,
                          config.ENERGY_CAMERA_GAIN);
        LOG(WARNING) << "Change to Energy mode:" << config.RUNMODE;
        lastRunMode = config.RUNMODE;
        return;
    }
    if ((lastRunMode == SMALL_ENERGY_STATE ||
         lastRunMode == BIG_ENERGY_STATE) &&
        config.RUNMODE == ARMOR_STATE) {
        if (!config.use_video)
            cam->setArmor(config.ARMOR_CAMERA_EXPOSURE,
                          config.ARMOR_CAMERA_GAIN);
        LOG(WARNING) << "Change to Armor mode:" << config.RUNMODE;
        lastRunMode = config.RUNMODE;
        return;
    }
    if (lastRunMode != config.RUNMODE &&
        (config.RUNMODE == SMALL_ENERGY_STATE ||
         config.RUNMODE == BIG_ENERGY_STATE)) {
        energy->setEnergyInit();
    }
    
    if (config.RUNMODE == ARMOR_STATE) {
        armor_finder->run(src);
    }
    if (config.RUNMODE == SMALL_ENERGY_STATE) {
        src.copyTo(src_energy);
        energy->is_big = false;
        energy->is_small = true;
        energy->run(src_energy);
    }
    if (config.RUNMODE == BIG_ENERGY_STATE) {
        src.copyTo(src_energy);
        energy->is_big = true;
        energy->is_small = false;
        energy->run(src_energy);
    }
}

int main(int argc, char** argv) {
    signal(SIGINT, sig_handler);
    OnInit(argv[0]);
    while (keepRunning) {
        if (config.use_video) {
            if (!video->read(src)) break;
            cv::resize(src, src, cv::Size(640, 360));
        } else {
            cam->read(src);
            // config.camConfig.undistort(src);
        }
        if (config.show_origin) {
            cv::imshow("origin", src);
            // cv::waitKey(1);
        }
        check_mode_and_run(src);
        //cv::waitKey(9);
        if (config.has_show) cv::waitKey(1);
    }
    LOG(INFO) << "exiting...";
    OnClose();
    return 0;
}