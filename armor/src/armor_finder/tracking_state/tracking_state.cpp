//
// Created by xinyang on 19-3-27.
//
#include <armor_finder/armor_finder.h>
#include <runtime.h>
#include <show_images/show_images.h>

bool ArmorFinder::stateTrackingTarget(cv::Mat &src) {
    auto res = filterArmorInfoByColor(netDetector.detect(src),src);
    ArmorInfo target;
    double dist_min = last_box.rect.height * 2.0;
    for (auto it : res) {
        if (it.id == last_box.id) {
            double dist_now =
                getPointLength(last_box.getCenter() - it.getCenter());
            if(dist_now < dist_min){
                dist_min = dist_now;
                target = it;
            }
        }
    }
    //脱离追踪
    if(target.bbox == cv::Rect()){
        target_box = ArmorBox();
        LOG(INFO) << "out of track!";
        return false;
    }
    //寻找灯条信息，精确定位装甲版
    locateArmorBox(src, target);
    return true;
}
