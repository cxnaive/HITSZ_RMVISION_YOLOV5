#ifndef ARMOR_DETECTOR_H
#define ARMOR_DETECTOR_H

#include <vector>
#include "Trt.h"
#include <opencv2/opencv.hpp>

class ArmorInfo {
   public:
    cv::Rect bbox;
    int id;
    double conf;
    ArmorInfo():bbox(cv::Rect()),id(0),conf(0){};
    ArmorInfo(cv::Rect _bbox, int _id, double _conf)
        : bbox(_bbox), id(_id), conf(_conf) {}
    cv::Point2f getCenter();
};

class ArmorDetector {
   private:
    float* input;
    Trt* onnx_net;
    float confThreshold;  // Confidence threshold
    float nmsThreshold;   // Non-maximum suppression threshold
    int inpWidth;         // Width of network's input image
    int inpHeight;        // Height of network's input image
    int net_box_num;
    std::string classesFile;
    std::string onnx_file;
    std::string trt_file;
    std::vector<std::string> classes;
    std::vector<float> output;
    std::vector<ArmorInfo> PostProcess(const cv::Mat& frame, std::vector<float>& output);
   public:
    ArmorDetector();
    ~ArmorDetector();
    double total_time = 0;
    double total_cnt = 0;
    std::vector<ArmorInfo> detect(const cv::Mat& img);
    cv::Mat draw_output(cv::Mat& img, std::vector<ArmorInfo>& armors);
};
#endif