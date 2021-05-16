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
            predict_rad = + config.ENERGY_SMALL_SPEED * config.ENERGY_DELAY_TIME / 1000.0;
        else if (energy_rotation_direction == -1)
            predict_rad = -config.ENERGY_SMALL_SPEED * config.ENERGY_DELAY_TIME / 1000.0;
        rotate(target_point);
    } else if (is_big){
        double t0 = rmTime.milliseconds() / 1000 - predict_time;
        double t1 = t0 + config.ENERGY_DELAY_TIME / 1000.0;
        double abs_angle = func_pos(t1) - func_pos(t0);
        if (energy_rotation_direction == 1)
            predict_rad = abs_angle;
        else if (energy_rotation_direction == -1)
            predict_rad = -abs_angle;
        rotate(target_point);
    }
}
