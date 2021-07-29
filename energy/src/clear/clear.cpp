//
// Created by sun on 19-7-11.
//

#include <energy.h>

using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// 此函数用于清空各vector
// ---------------------------------------------------------------------------------------------------------------------
void Energy::clearAll() {
    fans.clear();
    armors.clear();
    flow_strip_fans.clear();
    target_armors.clear();
    flow_strips.clear();
}

//----------------------------------------------------------------------------------------------------------------------
// 此函数用于图像预处理
// ---------------------------------------------------------------------------------------------------------------------
void Energy::initImage(cv::Mat &src) {
    // if (src.type() == CV_8UC3) {
    //     cvtColor(src, src, COLOR_BGR2GRAY);
    // }
    if (config.ENEMY_COLOR == ENEMY_BLUE) {
        // threshold(src, src, energy_part_param_.RED_GRAY_THRESH, 255,
        //           THRESH_BINARY);
        cv::cvtColor(src, src, COLOR_BGR2HSV);
        std::vector<int> lower = {68, 130, 130};
        std::vector<int> upper = {124, 255, 255};
        cv::inRange(src, lower, upper, src);

    } else if (config.ENEMY_COLOR == ENEMY_RED) {
        // threshold(src, src, energy_part_param_.BLUE_GRAY_THRESH, 255,
        //           THRESH_BINARY);
        cv::Mat hsv_src, mask1;
        cv::cvtColor(src, hsv_src, COLOR_BGR2HSV);
        std::vector<int> lower = {6, 130, 130};
        std::vector<int> upper = {30, 255, 255};
        cv::inRange(hsv_src, lower, upper, src);
        //lower[0] = 156;
        //upper[0] = 180;
        //cv::inRange(hsv_src, lower, upper, mask1);
        src += mask1;
    }
    if (config.show_process) imshow("bin", src);
    if (config.show_energy_extra || config.show_process) waitKey(1);
}
