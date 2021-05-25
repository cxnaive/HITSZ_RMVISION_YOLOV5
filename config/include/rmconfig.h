#include<glog/logging.h>
#include<json/json.h>
#include<constants.h>
#include<fstream>
#include<opencv2/opencv.hpp>
#ifndef H_RMCONFIG
#define H_RMCONFIG
//程序运行时各参数设置类
class CameraConfig{
public:
    double fx,fy,cx,cy,k1,k2,p1,p2,k3;
    double FOCUS_PIXEL;
    int64_t roi_width;
    int64_t roi_height;
    int64_t roi_offset_x;
    int64_t roi_offset_y;

    cv::Mat mtx,dist,mapx,mapy;
    cv::Mat getCameraMatrix(){
        return mtx;
    }   
    cv::Mat getDistCoeff(){
        return dist;
    }
    void init();
    void undistort(cv::Mat& frame){
        remap(frame,frame,mapx,mapy, cv::INTER_LINEAR);
    }
    
};
class RmConfig{
public:
    const char* configPath = "rmconfig.json";
    //config
    //
    bool show_origin = false;
    bool show_armor_box = false;
    bool show_light_box = false;
    bool show_class_id = false;
    bool show_net_box = false;
    bool wait_uart = false;
    bool use_pnp = false;
    bool save_video = false;
    bool show_light_blobs = false;
    bool save_labelled_boxes = false;
    bool show_pnp_axies = false;
    bool log_send_target = false;
    bool use_video = false;
    std::string video_path = "video.mp4";
    std::string camera_sn = "NULL";
    std::set<std::string> ignore_types = {"RE","BE"};
    //energy config
    bool show_energy_extra = false;
    bool show_energy = false;
    bool save_mark = false;
    bool show_process = false;
    bool show_info = false;
    //serial
    bool show_mcu_info = false;
    std::string uart_port = "/dev/ttyUSB0";
    //graphic
    bool has_show = false;
    //DATA
    int ARMOR_CAMERA_EXPOSURE = 4000;
    int ARMOR_CAMERA_GAIN = 8;
    int ENERGY_CAMERA_EXPOSURE = 4000;
    int ENERGY_CAMERA_GAIN = 5;
    int ENEMY_COLOR = ENEMY_RED;
    double ENERGY_SMALL_SPEED = 60;
    double ENERGY_DELAY_TIME = 270;
    int ANTI_TOP = 0;
    int ARMOR_H = 58;
    int ARMOR_W = 230;
    int IMAGE_CENTER_X = 320;
    int IMAGE_CENTER_Y = 320;
    char RUNMODE = ARMOR_STATE;
    double MCU_DELTA_X = 0;
    double MCU_DELTA_Y = 0;
    double MCU_YAW_SPEED = 0;
    double MCU_PITCH_SPEED = 0;
    double MANUAL_DELTA_X = 0;
    double MANUAL_DELTA_Y = 0;
    float BULLET_SPEED = 0;
    double ARMOR_DELTA_X = 0;
    double ARMOR_DELTA_Y = 0;
    //double ARMOR_PITCH_DELTA_K = 0;
    //double ARMOR_PITCH_DELTA_B = 0;
    std::vector<double> ARMOR_PITCH_DELTA_K = {0};
    std::vector<double> ARMOR_PITCH_DELTA_B = {0};
    std::vector<double> ARMOR_BULLET_SPEED_SET = {0};
    double ARMOR_PITCH_KP = 1;
    double ARMOR_PITCH_KI = 0;
    double ARMOR_PITCH_KD = 0;
    double ARMOR_YAW_KP = 1;
    double ARMOR_YAW_KI = 0;
    double ARMOR_YAW_KD = 0;
    double PROG_DELAY = 13;
    //Camera
    CameraConfig camConfig;
    

    void init_from_file();
    void write_to_file();
};

#endif