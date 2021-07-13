#ifndef _DRIVER_HK_CAM_WRAPPER_H
#define _DRIVER_HK_CAM_WRAPPER_H
#ifdef USE_HK
#include <MvCameraControl.h>

#include <chrono>
#include <thread>

#include "cam_wrapper.h"

class HKCamera : public Camera {
    friend void getRGBImage(HKCamera *cam);

   private:
    std::string sn;
    cv::Mat p_img, p_energy;
    bool is_energy;
    bool thread_running;
    bool init_success;

    int nRet;  // error code
    void *m_handle;
    MV_CC_DEVICE_INFO m_stDevInfo;
    MVCC_INTVALUE m_SensorWidth, m_SensorHeight;
    MVCC_FLOATVALUE m_GainRange;
    MVCC_ENUMVALUE m_PixelFormat;
    MV_FRAME_OUT stOutFrame;
    std::thread hkcam_thread;
    std::mutex pimg_lock;
    int64_t frame_cnt;
    double frame_get_time;
    std::chrono::steady_clock::time_point fps_time_point;

   public:
    HKCamera(std::string sn);
    ~HKCamera();
    bool init(int roi_x, int roi_y, int roi_w, int roi_h, float exposure,
              float gain, bool isEnergy)
        final;  // init camera lib and do settings, be called firstly
    void setParam(float exposure, float gain) final;  // set exposure and gain
    void start() final;                               // start video stream
    void stop() final;                                // stop receiving frames
    void calcRoi();                   // autmatic resize parameters
    bool init_is_successful() final;  // return video is available or not
    bool read(cv::Mat &src) final;
};
#endif
#endif