//
// Created by xinyang on 19-3-27.
//
#define LOG_LEVEL LOG_NONE
#include <armor_finder/armor_finder.h>
#include <runtime.h>
#include <show_images/show_images.h>

bool ArmorFinder::stateTrackingTarget(cv::Mat &src) {
    cv::Rect rectpos = target_box.rect;
    if (!tracker->update(src, rectpos)) {  // 使用KCFTracker进行追踪
        target_box = ArmorBox();
        LOG(WARNING) << "Track fail!";
        return false;
    }
    cv::Rect2d pos = (cv::Rect2d)rectpos;
    if ((pos & cv::Rect2d(0, 0, 640, 480)) != pos) {
        target_box = ArmorBox();
        LOG(WARNING) << "Track out range!";
        return false;
    }

    // 获取相较于追踪区域两倍长款的区域，用于重新搜索，获取灯条信息
    cv::Rect2d bigger_rect;
    bigger_rect.x = pos.x - pos.width / 2.0;
    bigger_rect.y = pos.y - pos.height / 2.0;
    bigger_rect.height = pos.height * 2;
    bigger_rect.width = pos.width * 2;
    bigger_rect &= cv::Rect2d(0, 0, 640, 480);
    cv::Mat roi = src(bigger_rect).clone();

    ArmorBox box;
    // 在区域内重新搜索。
    if (findAccurateArmorBox(roi,
                     box)) {  // 如果成功获取目标，则利用搜索区域重新更新追踪器
        target_box = box;
        target_box.rect.x += bigger_rect.x;  //　添加roi偏移量
        target_box.rect.y += bigger_rect.y;
        for (auto &blob : target_box.light_blobs) {
            blob.rect.center.x += bigger_rect.x;
            blob.rect.center.y += bigger_rect.y;
        }
        tracker = TrackerToUse::create();
        tracker->init(src, target_box.rect);
    } else {  // 如果没有成功搜索目标，则判断是否跟丢。
        
    }
    return true;
}
