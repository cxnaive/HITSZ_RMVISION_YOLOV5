//
// Created by sun on 19-7-11.
//

#include <energy.h>

using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// 此函数用于判断guess模式是否超时
// ---------------------------------------------------------------------------------------------------------------------
bool Energy::isGuessingTimeout() {
    double cur_time = rmTime.milliseconds();
    return cur_time - time_start_guess > 1000;
};