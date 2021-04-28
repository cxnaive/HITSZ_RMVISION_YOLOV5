#include <KalmanPosFilter.h>
#include <opencv2/opencv.hpp>
#include <glog/logging.h>
void KalmanPosFilter::init(cv::Point2f target){
    KF = cv::KalmanFilter(4,2,0);
    KF.transitionMatrix = (cv::Mat_<float>(4,4) << 1,0,1,0,0,1,0,1,0,0,1,0,0,0,0,1);
    //LOG(INFO) << "KalmanFilter trans: "<< KF.transitionMatrix;
    cv::setIdentity(KF.measurementMatrix);
    //LOG(INFO) << "KalmanFilter measure: "<< KF.measurementMatrix;
    cv::setIdentity(KF.processNoiseCov, cv::Scalar::all(1e-4));
    cv::setIdentity(KF.measurementNoiseCov, cv::Scalar::all(1e-5));
    //cv::setIdentity(KF.errorCovPost, cv::Scalar::all(1));
    KF.statePost = (cv::Mat_<float>(4,1) << target.x,target.y,0,0);
    KF.statePre = (cv::Mat_<float>(4,1) << target.x,target.y,0,0);
}

void KalmanPosFilter::update(cv::Point2f target){
    KF.correct((cv::Mat_<float>(2,1) << target.x,target.y));
}

cv::Point2f KalmanPosFilter::predict(){
    cv::Mat result = KF.predict();
    //LOG(INFO) << "KF result:" << result;
    float* data = (float*) result.data;
    return cv::Point2f(data[0],data[1]);
}