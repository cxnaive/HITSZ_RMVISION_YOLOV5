//
// Created by xixiliadorabarry on 1/24/19.
//

#include <energy.h>

using namespace cv;
using std::cout;
using std::endl;
using std::vector;

//----------------------------------------------------------------------------------------------------------------------
// 此函数获取预测点坐标
// ---------------------------------------------------------------------------------------------------------------------
void Energy::getPredictPoint(cv::Point target_point) {
    if (is_small) {
        if (energy_rotation_direction == 1)
            predict_rad = config.ENERGY_SMALL_SPEED * config.ENERGY_DELAY_TIME / 1000.0;
        else if (energy_rotation_direction == -1)
            predict_rad = -config.ENERGY_SMALL_SPEED * config.ENERGY_DELAY_TIME / 1000.0;
        rotate(target_point);
    } else if (is_big)
        predict_point = target_point;
}
