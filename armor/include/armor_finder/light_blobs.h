#ifndef _LIGHT_BLOB_H_
#define _LIGHT_BLOB_H_
#include <opencv2/opencv.hpp>

/******************* 灯条类定义 ***********************/
class LightBlob {
   public:
    cv::RotatedRect rect;  //灯条位置
    double area_ratio;
    double length;       //灯条长度
    uint8_t blob_color;  //灯条颜色

    LightBlob(cv::RotatedRect &r, double ratio, uint8_t color)
        : rect(r), area_ratio(ratio), blob_color(color) {
        length = std::max(rect.size.height, rect.size.width);
    };
    LightBlob() = default;
};

typedef std::vector<LightBlob> LightBlobs;


#endif