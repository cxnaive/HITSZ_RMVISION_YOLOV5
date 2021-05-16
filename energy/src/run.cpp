//
// Created by xixiliadorabarry on 3/5/19.
//
#include <energy.h>

using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// 此函数为能量机关模式主控制流函数，且步兵仅拥有云台摄像头
// 距离设置在econstants.h中
// ---------------------------------------------------------------------------------------------------------------------
void Energy::run(cv::Mat &src) {
    clearAll();
    initImage(src);
    if (findArmors(src) < 1) {
        sendEnergyLost();
        return;
    }

    if (config.show_energy_extra) showArmors("armor", src);

    if (!findFlowStripFan(src)) {
        if (!findFlowStripWeak(src)) {
            sendEnergyLost();
            return;
        }
    } else {
        if (config.show_energy_extra) showFlowStripFan("strip fan", src);
        if (!findTargetInFlowStripFan()) {
            sendEnergyLost();
            return;  //// 在在流动条区域内寻找不到装甲板就返回
        }
        if (!findFlowStrip(src)) {
            sendEnergyLost();
            return;  //找不到流动条就返回
        }
    }
    findCenterROI(src);
    if (config.show_energy_extra || config.show_energy)
        showFlowStrip("strip", src);
    if (!findCenterR(src)) {
        sendEnergyLost();
        return;
    }
    if (config.show_energy_extra) showCenterR("R", src);
    fans_cnt = findFans(src);
    if (config.show_energy_extra) showFans("fans", src);

    changeTarget();
    getTargetPolarAngle();
    getTargetTime();
    if (energy_rotation_init) {
        initRotation();
        sendEnergyLost();
        return;
    }
    if(predict_time_init){
        sendEnergyLost();
        return;
    }
    // LOG(INFO) << predict_time;
    getPredictPoint(target_point);

    // cv::Mat rgb;
    // cv::cvtColor(src,rgb,cv::COLOR_GRAY2BGR);
    // cv::circle(rgb,predict_point,2,cv::Scalar(0,0,255),3);
    // cv::imshow("predict target",rgb);

    getAimPoint(predict_point);
    judgeShoot();
    sendEnergy();
    if (config.save_mark) writeDownMark(src);
}
