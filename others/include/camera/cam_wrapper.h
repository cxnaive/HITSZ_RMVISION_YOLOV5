#ifndef _H_RM_CAMERA_
#define _H_RM_CAMERA_
#include <opencv2/opencv.hpp>

class Camera {
   public:
    virtual bool init(int roi_x, int roi_y, int roi_w, int roi_h,
                      float exposure, float gain, bool isEnergy) = 0;
    virtual void setParam(float exposureInput, float gainInput) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool init_is_successful() = 0;
    virtual bool read(cv::Mat &src) = 0;
};

#endif