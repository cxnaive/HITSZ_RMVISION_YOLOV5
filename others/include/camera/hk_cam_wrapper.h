#ifndef _DRIVER_HK_CAM_WRAPPER_H
#define _DRIVER_HK_CAM_WRAPPER_H
#include "cam_wrapper.h"
#include <MvCameraControl.h>
#include <thread>

class HKCamera:public Camera{
    friend void getRGBImage(HKCamera *cam);
private:
    std::string sn;
    float gain;
    float exposure;
    cv::Mat p_img,p_energy;
    bool is_energy;
    bool thread_running;
    bool init_success;

    int nRet; //error code
    void* m_handle;
    MV_CC_DEVICE_INFO m_stDevInfo;
    MVCC_INTVALUE m_SensorWidth,m_SensorHeight;
    MVCC_FLOATVALUE m_GainRange;
    MVCC_ENUMVALUE m_PixelFormat;
    MV_FRAME_OUT stOutFrame;
    std::thread hkcam_thread;
    std::mutex pimg_lock;
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