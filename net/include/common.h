#ifndef YOLOV5_COMMON_H_
#define YOLOV5_COMMON_H_

#include <dirent.h>

#include <fstream>
#include <map>
#include <opencv2/opencv.hpp>
#include <sstream>
#include <vector>

#include "NvInfer.h"
#include "yololayer.h"

#define NVCHECK(status)                                          \
    do {                                                       \
        auto ret = (status);                                   \
        if (ret != 0) {                                        \
            std::cerr << "Cuda failure: " << ret << std::endl; \
            abort();                                           \
        }                                                      \
    } while (0)

using namespace nvinfer1;

cv::Mat preprocess_img(cv::Mat& img);

cv::Rect get_rect(const cv::Mat& img, float bbox[4]);

float iou(float lbox[4], float rbox[4]);

bool cmp(const Yolo::Detection& a, const Yolo::Detection& b);

void nms(std::vector<Yolo::Detection>& res, float* output, float conf_thresh,
         float nms_thresh);

// TensorRT weight files have a simple space delimited format:
// [type] [size] <data x size in hex>
std::map<std::string, Weights> loadWeights(const std::string file);

IScaleLayer* addBatchNorm2d(INetworkDefinition* network,
                            std::map<std::string, Weights>& weightMap,
                            ITensor& input, std::string lname, float eps);

ILayer* convBlock(INetworkDefinition* network,
                  std::map<std::string, Weights>& weightMap, ITensor& input,
                  int outch, int ksize, int s, int g, std::string lname);

ILayer* focus(INetworkDefinition* network,
              std::map<std::string, Weights>& weightMap, ITensor& input,
              int inch, int outch, int ksize, std::string lname);

ILayer* bottleneck(INetworkDefinition* network,
                   std::map<std::string, Weights>& weightMap, ITensor& input,
                   int c1, int c2, bool shortcut, int g, float e,
                   std::string lname);

ILayer* bottleneckCSP(INetworkDefinition* network,
                      std::map<std::string, Weights>& weightMap, ITensor& input,
                      int c1, int c2, int n, bool shortcut, int g, float e,
                      std::string lname);

ILayer* C3(INetworkDefinition* network,
           std::map<std::string, Weights>& weightMap, ITensor& input, int c1,
           int c2, int n, bool shortcut, int g, float e, std::string lname);

ILayer* SPP(INetworkDefinition* network,
            std::map<std::string, Weights>& weightMap, ITensor& input, int c1,
            int c2, int k1, int k2, int k3, std::string lname);

int read_files_in_dir(const char* p_dir_name,
                      std::vector<std::string>& file_names);

std::vector<float> getAnchors(std::map<std::string, Weights>& weightMap);

IPluginV2Layer* addYoLoLayer(INetworkDefinition* network,
                             std::map<std::string, Weights>& weightMap,
                             IConvolutionLayer* det0, IConvolutionLayer* det1,
                             IConvolutionLayer* det2);

#endif
