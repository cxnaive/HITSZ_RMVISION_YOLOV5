#ifndef ARMOR_DETECTOR_H
#define ARMOR_DETECTOR_H

#include <vector>

#include "common.h"
#include "cuda_runtime_api.h"
#include "logging.h"

#define NMS_THRESH 0.2
#define CONF_THRESH 0.6
#define BATCH_SIZE 1
#define DEVICE 0
#define ENGINE_NAME "yolov5s.engine"

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
    static constexpr int INPUT_H = Yolo::INPUT_H;
    static constexpr int INPUT_W = Yolo::INPUT_W;
    static constexpr int CLASS_NUM = Yolo::CLASS_NUM;
    static constexpr int OUTPUT_SIZE =
        Yolo::MAX_OUTPUT_BBOX_COUNT * sizeof(Yolo::Detection) / sizeof(float) +
        1;  // we assume the yololayer outputs no more than
            // MAX_OUTPUT_BBOX_COUNT boxes that conf >= 0.1
    static constexpr const char* INPUT_BLOB_NAME = "data";
    static constexpr const char* OUTPUT_BLOB_NAME = "prob";
    static const char* id_names[18];
    float data[BATCH_SIZE * 3 * INPUT_H * INPUT_W];
    float prob[BATCH_SIZE * OUTPUT_SIZE];
    void* buffers[2];
    cudaStream_t stream;
    IRuntime* runtime;
    ICudaEngine* engine;
    IExecutionContext* context;
    int inputIndex, outputIndex;
    void doInference(IExecutionContext& context, cudaStream_t& stream,
                     void** buffers, float* input, float* output,
                     int batchSize);

   public:
    ArmorDetector();
    ~ArmorDetector();
    double total_time = 0;
    double total_cnt = 0;
    std::vector<ArmorInfo> detect(const cv::Mat& img);
    cv::Mat draw_output(cv::Mat& img, std::vector<ArmorInfo>& armors);
};
#endif