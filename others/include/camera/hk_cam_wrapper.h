#ifndef _DRIVER_HK_CAM_WRAPPER_H
#define _DRIVER_HK_CAM_WRAPPER_H
#include "cam_wrapper.h"
#include <MvCameraControl.h>

class HKCamera:public Camera{
private:
    std::string sn;
    float gain;
    int exposure;
    cv::Mat p_img,p_energy;
    bool is_energy;
    bool thread_running;
    bool init_success;
public:
    HKCamera(std::string sn);
    ~HKCamera();
    bool init(int roi_x,int roi_y,int roi_w,int roi_h,bool isEnergy) final;           // init camera lib and do settings, be called firstly
    void setParam(int exposureInput, int gainInput) final; 	// set exposure and gain
    void start() final;					                    // start video stream
    void stop() final;					                    // stop receiving frames
    void calcRoi(); //autmatic resize parameters
    bool init_is_successful() final;				            // return video is available or not
    bool read(cv::Mat &src) final;
};

#endif