//
// Created by erc on 1/3/20.
//

#ifndef RM2020_CAM_DRIVER_CAMWRAPPER_H
#define RM2020_CAM_DRIVER_CAMWRAPPER_H

#include <thread>
#include "GxIAPI.h"
#include "DxImageProc.h"
#include "iostream"
#include "wrapper_head.h"
#include <rmconfig.h>
#include <opencv2/opencv.hpp>

class Camera:public WrapperHead{
    friend void getRGBImage(Camera *p_cam);
    friend void GX_STDC OnFrameCallbackFun(GX_FRAME_CALLBACK_PARAM *pFrame);
private:
    std::string sn;
    int exposure, gain;
    GX_STATUS status;

    int64_t nPayLoadSize;
    uint32_t nDeviceNum;

    GX_DEV_HANDLE g_hDevice;
    GX_FRAME_DATA g_frameData;
    int64_t g_nPixelFormat;
    int64_t g_nColorFilter;
    int64_t g_SensorHeight;
    int64_t g_SensorWidth;
    int64_t frame_cnt;
    double frame_get_time;
    CameraConfig camConfig;
    cv::Mat p_img,p_energy;
    void *g_pRGBframeData;
    void *g_pRaw8Buffer;
    std::thread cam_run;
    bool thread_running;
    bool init_success;
    bool is_energy;
    

public:
    Camera(std::string sn,CameraConfig config);			        // constructor, p_img is a pointer towards a 640*640 8uc3 Mat type
    ~Camera();
    
    bool init() final;                                        // init camera lib and do settings, be called firstly
    void setParam(int exposureInput, int gainInput); 	// set exposure and gain
    void setEnergy(int exposureInput, int gainInput);
    void setArmor(int exposureInput, int gainInput);
    void start();					                    // start video stream
    void stop();					                    // stop receiving frames
    void calcRoi(); //autmatic resize parameters
    bool init_is_successful();				            // return video is available or not
    bool read(cv::Mat &src) final;
};

#endif //RM2020_CAM_DRIVER_CAMWRAPPER_H
