//
// Created by xinyang on 19-3-27.
//

#include <armor_finder/armor_finder.h>
#include <runtime.h>
#include <show_images/show_images.h>

bool ArmorFinder::stateSearchingTarget(cv::Mat& src) {
    auto res = filterArmorInfoByColor(netDetector.detect(src),src);
    if (res.size() == 0) {
        target_box = ArmorBox();
        anti_switch_cnt++;
        return false;
    }
    std::sort(
        res.begin(), res.end(), [&](const ArmorInfo& a, const ArmorInfo& b) {
            // if (a.id == b.id) {
            //     auto d1 = (a.bbox.x - config.IMAGE_CENTER_X) *
            //                   (a.bbox.x - config.IMAGE_CENTER_X) +
            //               (a.bbox.y - config.IMAGE_CENTER_Y) *
            //                   (a.bbox.y - config.IMAGE_CENTER_Y);
            //     auto d2 = (b.bbox.x - config.IMAGE_CENTER_X) *
            //                   (b.bbox.x - config.IMAGE_CENTER_X) +
            //               (b.bbox.y - config.IMAGE_CENTER_Y) *
            //                   (b.bbox.y - config.IMAGE_CENTER_Y);
            //     return d1 < d2;
            // } else {
            //     if (enemy_color == ENEMY_BLUE)
            //         return prior_blue[id2name[a.id]] <
            //                prior_blue[id2name[b.id]];
            //     else
            //         return prior_red[id2name[a.id]] < prior_red[id2name[b.id]];
            // }
            std::map<std::string, int>* prior = enemy_color == ENEMY_BLUE ? &prior_blue:&prior_red;
            double score_a = (*prior)[id2name[a.id]] * a.conf * a.bbox.area();
            double score_b = (*prior)[id2name[b.id]] * b.conf * b.bbox.area();
            return score_a < score_b;
        });
    ArmorInfo target = res[0];

    //寻找灯条信息，精确定位装甲版
    locateArmorBox(src, target);
    //判断当前目标和上次有效目标是否为同一个目标,并给３帧的时间，试图找到相同目标。即刚发生目标切换内的３帧内不发送目标位置。可以一定程度避免频繁多目标切换。
    if (last_box.rect != cv::Rect2d() &&
        (getPointLength(last_box.getCenter() - target_box.getCenter()) >
         last_box.rect.height * 2.0) &&
        anti_switch_cnt++ < 3) {
                target_box = ArmorBox();
        LOG(INFO) << "avoid_quick_switch!";
        return false;
    } else {
        anti_switch_cnt = 0;
        return true;
    }
}