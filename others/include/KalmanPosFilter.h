#ifndef _KALMAN_POS_FILTER_H
#define _KALMAN_POS_FILTER_H
#include <opencv2/video/tracking.hpp>

class KalmanPosFilter {
   public:
    cv::KalmanFilter KF;
    void init(cv::Point2f target);
    void update(cv::Point2f target);
    cv::Vec4f predict();
};

#endif