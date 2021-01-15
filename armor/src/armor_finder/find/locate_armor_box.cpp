#include <armor_finder/armor_finder.h>
#include <runtime.h>
#include <show_images/show_images.h>

#include <opencv2/highgui.hpp>
#include <vector>

bool ArmorFinder::locateArmorBox(const cv::Mat &src, const ArmorInfo &target) {
    // 获取相较于追踪区域1.25倍长宽的区域，用于获取灯条信息
    cv::Rect2d bigger_rect;
    bigger_rect.x = target.bbox.x - target.bbox.width / 4.0;
    bigger_rect.y = target.bbox.y - target.bbox.height / 4.0;
    bigger_rect.height = target.bbox.height * 1.25;
    bigger_rect.width = target.bbox.width * 1.25;
    bigger_rect &= cv::Rect2d(0, 0, 640, 480);
    cv::Mat roi = src(bigger_rect).clone();

    ArmorBox box;
    //搜索灯条
    if(findAccurateArmorBox(roi,box)){
        target_box = box;
        target_box.rect.x += bigger_rect.x;  //　添加roi偏移量
        target_box.rect.y += bigger_rect.y;
        for (auto &blob : target_box.light_blobs) {
            blob.rect.center.x += bigger_rect.x;
            blob.rect.center.y += bigger_rect.y;
        }
    }
    else{

    }
}