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
    bool wait_uart = false;
    bool save_video = false;
    bool show_light_blobs = false;
    bool save_labelled_boxes = false;
    bool show_pnp_axies = false;
    bool log_send_target = false;
    //energy config
    bool show_energy = false;
    bool save_mark = false;
    bool show_process = false;
    bool show_info = false;
    std::string uart_port = "/dev/ttyUSB0";

    //DATA
    int ARMOR_CAMERA_EXPOSURE = 4000;
    int ARMOR_CAMERA_GAIN = 8;
    int ENERGY_CAMERA_EXPOSURE = 4000;
    int ENERGY_CAMERA_GAIN = 5;
    int ENEMY_COLOR = ENEMY_RED;
    int ANTI_TOP = 0;
    int ARMOR_H = 58;
    int ARMOR_W = 230;
    int IMAGE_CENTER_X = 320;
    int IMAGE_CENTER_Y = 240;
    char RUNMODE = ARMOR_STATE;
    double MCU_DELTA_X = 0;
    double MCU_DELTA_Y = 0;
    double MANUAL_DELTA_X = 0;
    double MANUAL_DELTA_Y = 0;
    
    //Camera
    CameraConfig camConfig;

    void init_from_file();
    void write_to_file();
};

#endif