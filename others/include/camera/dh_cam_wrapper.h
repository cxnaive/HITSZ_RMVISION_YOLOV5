//
// Created by erc on 1/3/20.
//

#ifndef _CAM_DRIVER_DH_CAMWRAPPER_H
#define _CAM_DRIVER_DH_CAMWRAPPER_H

#include <thread>
#include "GxIAPI.h"
#include "DxImageProc.h"
#include <glog/logging.h>
#include "iostream"
#include "cam_wrapper.h"
#include <opencv2/opencv.hpp>

class DHCamera:public Camera{
    friend void getRGBImage(DHCamera *p_cam);
    friend void GX_STDC OnFrameCallbackFun(GX_FRAME_CALLBACK_PARAM *pFrame);
private:
    std::string sn;
    int exposure, gain;
    GX_STATUS status;

    int64_t nPayLoadSize;
    uint32_t nDeviceNum;

    GX_DEV_HANDLE g_hDevice;
    GX_FRAME_DATA g_frameData;
    PGX_FRAME_BUFFER g_frameBuffer;
    int64_t g_nPixelFormat;
    int64_t g_nColorFilter;
    int64_t g_SensorHeight;
    int64_t g_SensorWidth;
    int64_t frame_cnt;
    double frame_get_time;
    cv::Mat p_img,p_energy;
    void *g_pRGBframeData;
    void *g_pRaw8Buffer;
    std::thread cam_run;
    bool thread_running;
    bool init_success;
    bool is_energy;
    
    

public:
    DHCamera(std::string sn);			        // constructor, p_img is a pointer towards a 640*640 8uc3 Mat type
    ~DHCamera();
    
    bool init(int roi_x,int roi_y,int roi_w,int roi_h,bool isEnergy) final;           // init camera lib and do settings, be called firstly
    void setParam(int exposureInput, int gainInput) final; 	// set exposure and gain
    void start() final;					                    // start video stream
    void stop() final;					                    // stop receiving frames
    void calcRoi(); //autmatic resize parameters
    bool init_is_successful() final;				            // return video is available or not
    bool read(cv::Mat &src) final;
};

#endif //RM2020_CAM_DRIVER_CAMWRAPPER_H
