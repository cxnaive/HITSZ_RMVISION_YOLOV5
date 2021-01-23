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
void Energy::run(cv::Mat &raw_src) {
    Mat src = raw_src.clone();
    clearAll();
    initImage(src);
    if (findArmors(src) < 1) return;
    
    if (config.show_energy_extra) showArmors("armor", src);

    if (!findFlowStripFan(src)) {
        if (!findFlowStripWeak(src)) return;
    } else {
        if (config.show_energy_extra) showFlowStripFan("strip fan", src);
        if (!findTargetInFlowStripFan())
            return;  //// 在在流动条区域内寻找不到装甲板就返回
        if (!findFlowStrip(src)) return;  //找不到流动条就返回
    }
    findCenterROI(src);
    if (config.show_energy_extra || config.show_energy) showFlowStrip("strip", src);
    if (!findCenterR(src)) return;
    if (config.show_energy_extra) showCenterR("R", src);
    fans_cnt = findFans(src);
    if (config.show_energy_extra) showFans("fans", src);

    changeTarget();
    getTargetPolarAngle();
    if (is_big && energy_rotation_init) {
        initRotation();
        return;
    }
    getPredictPoint(target_point);
    getAimPoint(predict_point);
    judgeShoot();
    sendEnergy();
    if (config.save_mark) writeDownMark(src);
}
