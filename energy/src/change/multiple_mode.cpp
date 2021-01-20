//
// Created by sun on 19-7-23.
//

#include <energy.h>

using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// 此函数用于切换预测模式和猜测模式，但最终未使用
// ---------------------------------------------------------------------------------------------------------------------
void Energy::multipleMode(cv::Mat &src) {
    if (is_predicting) {
        getPredictPoint(target_point);
        getAimPoint(predict_point);
        judgeShoot();
        sendEnergy();
    } else if (is_guessing && stayGuessing()) {
        findFans(src);
        if (config.show_energy_extra) showFans("fans", src);
        if (config.save_mark) writeDownMark(src);
        guessTarget();
        if (config.show_energy_extra) showGuessTarget("guess", src);
        getPredictPoint(guess_point);
        getAimPoint(predict_point);
        sendEnergy();
    }
}