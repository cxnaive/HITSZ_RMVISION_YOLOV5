//
// Created by sun on 19-7-11.
//

#include <energy.h>

using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// 此函数用于判断云台坐标系下是否可以发弹
// ---------------------------------------------------------------------------------------------------------------------
void Energy::judgeShoot() {
    if (abs(yaw_rotation) < 1 && abs(pitch_rotation) < 1) {
        double time_now = rmTime.milliseconds();
        if (time_now - last_shoot_time_point > shoot_delay) {
            shoot = 2;
            last_shoot_time_point = time_now;
        } else {
            shoot = 1;
        }
        //        is_predicting = false;
        //        is_guessing = true;
        //        start_guess = true;
        //        getsystime(time_start_guess);
        //        LOGM(STR_CTR(WORD_LIGHT_RED, "Start Guessing!"));
    } else
        shoot = 1;
}
