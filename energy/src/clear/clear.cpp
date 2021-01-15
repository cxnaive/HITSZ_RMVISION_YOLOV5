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
    if (src.type() == CV_8UC3) {
        cvtColor(src, src, COLOR_BGR2GRAY);
    }
    if (config.ENEMY_COLOR == ENEMY_BLUE) {
        threshold(src, src, energy_part_param_.RED_GRAY_THRESH, 255,
                  THRESH_BINARY);
    } else if (config.ENEMY_COLOR == ENEMY_RED) {
        threshold(src, src, energy_part_param_.BLUE_GRAY_THRESH, 255,
                  THRESH_BINARY);
    }
    if (config.show_process) imshow("bin", src);
    if (config.show_energy || config.show_process) waitKey(1);
}
