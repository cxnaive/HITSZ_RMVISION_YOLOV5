#include <armor_finder/armor_finder.h>
#include <camera/cam_wrapper.h>
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
// ArmorFinder实例
ArmorFinder* armor_finder;
// Energy 实例
Energy* energy;
//运行时原图实例
cv::Mat src;
//保存上一次循环运行模式
char lastRunMode;
static void OnInit(const char* cmd) {
    FLAGS_alsologtostderr = true;
    FLAGS_colorlogtostderr = true;
    google::InitGoogleLogging(cmd);
    rmTime.init();

    config.init_from_file();

    //rmSerial.init();

    cam = new Camera(1, config.camConfig);
    cam->init();
    cam->setParam(config.ARMOR_CAMERA_EXPOSURE, config.ARMOR_CAMERA_GAIN);
    cam->start();

    armor_finder =
        new ArmorFinder(config.ENEMY_COLOR, rmSerial, config.ANTI_TOP);
    energy = new Energy(rmSerial, config.ENEMY_COLOR);
    lastRunMode = config.RUNMODE;
}

static void OnClose() { config.write_to_file(); }

void check_mode_and_run(cv::Mat &src) {
    if (lastRunMode == ARMOR_STATE && (config.RUNMODE == SMALL_ENERGY_STATE ||
                                       config.RUNMODE == BIG_ENERGY_STATE)) {
        cam->setParam(config.ENERGY_CAMERA_EXPOSURE, config.ENERGY_CAMERA_GAIN);
    }
    if ((lastRunMode == SMALL_ENERGY_STATE ||
         lastRunMode == BIG_ENERGY_STATE) &&
        config.RUNMODE == ARMOR_STATE) {
        cam->setParam(config.ARMOR_CAMERA_EXPOSURE, config.ARMOR_CAMERA_GAIN);
    }
    lastRunMode = config.RUNMODE;
    if(config.RUNMODE == ARMOR_STATE){
        armor_finder->run(src);
    }
    if(config.RUNMODE == SMALL_ENERGY_STATE){
        energy->is_big = false;
        energy->is_small = true;
        energy->run(src);
    }
    if(config.RUNMODE == BIG_ENERGY_STATE){
        energy->is_big = true;
        energy->is_small = false;
        energy->run(src);
    }
}

int main(int argc, char** argv) {
    signal(SIGINT, sig_handler);
    OnInit(argv[0]);
    while (keepRunning) {
        cam->read(src);
        config.camConfig.undistort(src);
        if (config.show_origin) {
            cv::imshow("origin", src);
            cv::waitKey(1);
        }
        check_mode_and_run(src);
    }
    LOG(INFO) << "exiting...";
    OnClose();
    return 0;
}