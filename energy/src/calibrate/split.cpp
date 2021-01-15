﻿//
// Created by xixiliadorabarry on 1/24/19.
//
#include <energy.h>

using namespace cv;
using std::cout;
using std::endl;
using std::vector;

//----------------------------------------------------------------------------------------------------------------------
// 此函数用于分离拜耳阵列
// ---------------------------------------------------------------------------------------------------------------------
void Energy::splitBayerBG(cv::Mat src, cv::Mat &blue, cv::Mat &red) {
    uchar *data;
    uchar *bayer_data[2];
    for (int i = 0; i < src.rows; ++i) {
        data = src.ptr<uchar>(i);
        bayer_data[0] = blue.ptr<uchar>(i / 2);
        for (int j = 0; j < blue.cols; ++j, data += 2) {
            bayer_data[0][j] = *data;
        }
        data = src.ptr<uchar>(++i) + 1;
        bayer_data[1] = red.ptr<uchar>(i / 2);
        for (int j = 0; j < red.cols; ++j, data += 2) {
            bayer_data[1][j] = *data;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// 此函数对图像进行通道分离处理
// ---------------------------------------------------------------------------------------------------------------------
void Energy::imagePreprocess(cv::Mat &src) {
    if (src.type() == CV_8UC1) {
        splitBayerBG(src, src_blue, src_red);
        if (ally_color == ALLY_RED) {
            //            src = src_red - src_blue;
            src = src_red;
        } else if (ally_color == ALLY_BLUE) {
            //            src = src_blue - src_red;
            src = src_blue;
        }
    }

    else if (src.type() == CV_8UC3) {
        std::vector<Mat> channels;
        split(src, channels);
        resize(channels.at(0), src_blue,
               Size(src.size().width / 2, src.size().height / 2));
        resize(channels.at(1), src_green,
               Size(src.size().width / 2, src.size().height / 2));
        resize(channels.at(2), src_red,
               Size(src.size().width / 2, src.size().height / 2));
        if (ally_color == ALLY_RED) {
            //            src = src_red-src_blue;
            src = src_red;
        } else if (ally_color == ALLY_BLUE) {
            //            src = src_blue-src_red;
            src = src_blue;
        }
    }
    cv::resize(src, src, cv::Size(src.size().width * 2, src.size().height * 2),
               2);
    threshold(src, src, energy_part_param_.SPLIT_GRAY_THRESH, 255,
              THRESH_BINARY);
}
