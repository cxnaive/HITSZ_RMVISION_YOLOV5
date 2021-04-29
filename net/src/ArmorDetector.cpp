#include "ArmorDetector.h"

#include <fstream>
#include <iostream>

#include "glog/logging.h"

ArmorDetector::ArmorDetector() {
    classesFile = "rmcv.names";
    onnx_file = "mbv3.onnx";
    trt_file = "mbv3.engine";
    confThreshold = 0.3;
    nmsThreshold = 0.1;
    inpHeight = 640;
    inpWidth = 640;
    net_box_num = 25200;
    // Load names
    std::ifstream ifs(classesFile.c_str());
    std::string line;
    while (getline(ifs, line)) {
        classes.push_back(line);
    }
    // load engine
    onnx_net = new Trt();
    onnx_net->CreateEngine(onnx_file, trt_file, std::vector<std::string>(), 1,
                           1, std::vector<std::vector<float>>());
    input = new float[1 * 3 * inpHeight * inpWidth];
}

ArmorDetector::~ArmorDetector() {
    if (input) delete input;
    if (onnx_net) delete onnx_net;
}

std::vector<ArmorInfo> ArmorDetector::PostProcess(const cv::Mat& frame, std::vector<float>& output) {
    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<cv::Rect> bboxes;
    double ratio_w = frame.cols / 640.0;
    double ratio_h = frame.rows / 640.0;
    // cout << "out size: " << output.size() << endl;
    int class_num = classes.size();
    for (int i = 0; i < net_box_num; ++i) {
        int data_idx = i * (class_num + 6);
        float obj_conf = output[data_idx + 4];
        if (obj_conf > confThreshold) {
            int max_id = output[data_idx + 5];
            // cout << "id:" << max_id << endl;
            classIds.push_back(max_id);
            confidences.push_back(obj_conf);
            int centerX = (int)(output[data_idx] * ratio_w);
            int centerY = (int)(output[data_idx + 1] * ratio_h);
            int width = (int)(output[data_idx + 2] * ratio_w);
            int height = (int)(output[data_idx + 3] * ratio_h);
            int left = centerX - width / 2;
            int top = centerY - height / 2;
            // cout << left <<" "<< top <<" "<< width <<" "<< height << endl;
            bboxes.push_back(cv::Rect(left, top, width, height));
        }
    }
    std::vector<int> indices;
    std::vector<ArmorInfo> res;
    cv::dnn::NMSBoxes(bboxes, confidences, confThreshold, nmsThreshold, indices);
    for (size_t i = 0; i < indices.size(); ++i) {
        int idx = indices[i];
        cv::Rect box = bboxes[idx];
        res.push_back(ArmorInfo(box,classIds[idx],confidences[idx]));
    }
    return res;
}

std::vector<ArmorInfo> ArmorDetector::detect(const cv::Mat& img) {
    auto start = std::chrono::system_clock::now();
    int i = 0;
    for (int row = 0; row < inpHeight; ++row) {
        uchar* uc_pixel = img.data + row * img.step;
        for (int col = 0; col < inpWidth; ++col) {
            input[i] = (float)uc_pixel[2] / 255.0;
            input[i + inpHeight * inpWidth] = (float)uc_pixel[1] / 255.0;
            input[i + 2 * inpHeight * inpWidth] = (float)uc_pixel[0] / 255.0;
            uc_pixel += 3;
            ++i;
        }
    }
    onnx_net->DataTransferHost(input, 1 * 3 * inpWidth * inpHeight, 0);
    onnx_net->Forward();
    onnx_net->DataTransfer(output, 1, false);
    auto res = PostProcess(img, output);

    auto end = std::chrono::system_clock::now();
    total_time +=
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();
    ++total_cnt;
    return res;
}

cv::Mat ArmorDetector::draw_output(cv::Mat& img,
                                   std::vector<ArmorInfo>& armors) {
    cv::Mat out = img.clone();
    for (auto it : armors) {
        cv::rectangle(out, it.bbox, cv::Scalar(255,178,50), 1);
        std::string label = classes[it.id] + cv::format(" %.2f", it.conf);
        float wx = it.bbox.x;
        float wy = it.bbox.y;
        if (wy < 10) wy += it.bbox.height + 12;
        cv::putText(out, label, cv::Point(wx, wy - 1), cv::FONT_HERSHEY_SIMPLEX,
                    0.75, cv::Scalar(0xFF, 0xFF, 0xFF), 1);
    }
    return out;
}

cv::Point2f ArmorInfo::getCenter() {
    return cv::Point2f(bbox.x + bbox.width / 2, bbox.y + bbox.height / 2);
}