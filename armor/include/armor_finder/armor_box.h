

#ifndef _ARMOR_BOX_H_
#define _ARMOR_BOX_H_

#include <opencv2/opencv.hpp>
#include <armor_finder/light_blobs.h>

/******************* 装甲板类定义　**********************/
class ArmorBox {
   public:
    typedef enum {
        FRONT,
        SIDE,
        UNKNOWN
    } BoxOrientation;  //背一下单词Orientation  [ˌɔːriənˈteɪʃn]      方向

    cv::Rect2d rect;
    LightBlobs light_blobs;  //灯条
    uint8_t box_color;
    int id;

    explicit ArmorBox(const cv::Rect &pos = cv::Rect2d(),
                      const LightBlobs &blobs = LightBlobs(), uint8_t color = 0,
                      int i = 0);
    //注意：函数声明时，后面跟个const
    //限定了函数中不能有任何改变其所属对象成员变量值的功能，如果有则会在编译阶段就报错。
    cv::Point2f getCenter() const;    // 获取装甲板中心
    double getBlobsDistance() const;  // 获取两个灯条中心间距
    double lengthDistanceRatio() const;  // 获取灯条中心距和灯条长度的比值
    double getBoxDistance() const;  // 获取装甲板到摄像头的距离
    std::vector<cv::Point2f> getArmorPoints() const;
    std::pair<cv::Point3d,cv::Point3d> armorSolvePnP() const;
    //BoxOrientation getOrientation() const;  // 获取装甲板朝向(误差较大，已弃用)
    bool operator<(const ArmorBox &box) const;  // 装甲板优先级比较
};

typedef std::vector<ArmorBox> ArmorBoxes;

#endif