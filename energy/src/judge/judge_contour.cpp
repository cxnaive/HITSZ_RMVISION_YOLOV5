//
// Created by sun on 19-7-13.
//

#include <energy.h>

using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// 此函数用于判断找到的矩形候选区是否为扇叶
// ---------------------------------------------------------------------------------------------------------------------
bool Energy::isValidFanContour(cv::Mat &src,
                               const vector<cv::Point> &fan_contour) {
    double cur_contour_area = contourArea(fan_contour);
    if (cur_contour_area > energy_part_param_.FAN_CONTOUR_AREA_MAX ||
        cur_contour_area < energy_part_param_.FAN_CONTOUR_AREA_MIN) {
        // cout<<cur_contour_area<<" "<<energy_fan_param_.CONTOUR_AREA_MIN<<"
        // "<<energy_fan_param_.CONTOUR_AREA_MAX<<endl; cout<<"area fail."<<endl;
        return false;
        //选区面积大小不合适
    }
    RotatedRect cur_rect = minAreaRect(fan_contour);
    Size2f cur_size = cur_rect.size;
    float length = cur_size.height > cur_size.width
                       ? cur_size.height
                       : cur_size.width;  //将矩形的长边设置为长
    float width = cur_size.height < cur_size.width
                      ? cur_size.height
                      : cur_size.width;  //将矩形的短边设置为宽
    if (length < energy_part_param_.FAN_CONTOUR_LENGTH_MIN ||
        width < energy_part_param_.FAN_CONTOUR_WIDTH_MIN ||
        length > energy_part_param_.FAN_CONTOUR_LENGTH_MAX ||
        width > energy_part_param_.FAN_CONTOUR_WIDTH_MAX) {
        // cout<<"length width fail."<<endl;
        //        cout << "length: " << length << '\t' << "width: " << width <<
        //        '\t' << cur_rect.center << endl;
        return false;
        //矩形边长不合适
    }
    float length_width_ratio = length / width;  //计算矩形长宽比
    if (length_width_ratio > energy_part_param_.FAN_CONTOUR_HW_RATIO_MAX ||
        length_width_ratio < energy_part_param_.FAN_CONTOUR_HW_RATIO_MIN) {
        // cout<<"length width ratio fail."<<endl;
        //        cout << "HW: " << length_width_ratio << '\t' <<
        //        cur_rect.center << endl;
        return false;
        //长宽比不合适
    }
    //    cout << cur_contour_area / cur_size.area() << endl;
    if (cur_contour_area / cur_size.area() <
        energy_part_param_.FAN_CONTOUR_AREA_RATIO_MIN) {
        //        cout << cur_contour_area / cur_size.area() << endl;
        //        cout << "area ratio: " << cur_contour_area / cur_size.area()
        //        << '\t' << cur_rect.center << endl;
        return false;  //轮廓对矩形的面积占有率不合适
    }
    double non_zero_rate = nonZeroRateOfRotateRect(src, cur_rect);
    //    cout << non_zero_rate<<endl;
    //    if (non_zero_rate > energy_part_param_.FAN_NON_ZERO_RATE_MAX ||
    //        non_zero_rate < energy_part_param_.FAN_NON_ZERO_RATE_MIN)
    //        return false;
    //    //亮点占比不合格
    return true;
}

//----------------------------------------------------------------------------------------------------------------------
// 此函数用于判断找到的矩形候选区是否为装甲板
// ---------------------------------------------------------------------------------------------------------------------
bool Energy::isValidArmorContour(const vector<cv::Point> &armor_contour) {
    double cur_contour_area = contourArea(armor_contour);
    if (cur_contour_area > energy_part_param_.ARMOR_CONTOUR_AREA_MAX ||
        cur_contour_area < energy_part_param_.ARMOR_CONTOUR_AREA_MIN) {
        // cout<<cur_contour_area<<" "<<energy_fan_param_.CONTOUR_AREA_MIN<<"
        // "<<energy_fan_param_.CONTOUR_AREA_MAX<<endl; cout<<"area fail."<<endl;
        return false;
        //选区面积大小不合适
    }
    RotatedRect cur_rect = minAreaRect(armor_contour);
    Size2f cur_size = cur_rect.size;
    float length = cur_size.height > cur_size.width
                       ? cur_size.height
                       : cur_size.width;  //将矩形的长边设置为长
    float width = cur_size.height < cur_size.width
                      ? cur_size.height
                      : cur_size.width;  //将矩形的短边设置为宽
    if (length < energy_part_param_.ARMOR_CONTOUR_LENGTH_MIN ||
        width < energy_part_param_.ARMOR_CONTOUR_WIDTH_MIN ||
        length > energy_part_param_.ARMOR_CONTOUR_LENGTH_MAX ||
        width > energy_part_param_.ARMOR_CONTOUR_WIDTH_MAX) {
        // cout<<"length width fail."<<endl;
        //        cout << "length: " << length << '\t' << "width: " << width <<
        //        '\t' << cur_rect.center << endl;
        return false;
        //矩形边长不合适
    }

    float length_width_ratio = length / width;  //计算矩形长宽比
    if (length_width_ratio > energy_part_param_.ARMOR_CONTOUR_HW_RATIO_MAX ||
        length_width_ratio < energy_part_param_.ARMOR_CONTOUR_HW_RATIO_MIN) {
        // cout<<"length width ratio fail."<<endl;
        //        cout << "HW: " << length_width_ratio << '\t' <<
        //        cur_rect.center << endl;
        return false;
        //长宽比不合适
    }
    if (cur_contour_area / cur_size.area() <
        energy_part_param_.ARMOR_CONTOUR_AREA_RATIO_MIN) {
        //        cout << "area ratio: " << cur_contour_area / cur_size.area()
        //        << '\t' << cur_rect.center << endl;
        return false;  //轮廓对矩形的面积占有率不合适
    }
    return true;
}

//----------------------------------------------------------------------------------------------------------------------
// 此函数用于判断找到的矩形候选区是否为可能的风车中心R候选区
// ---------------------------------------------------------------------------------------------------------------------
bool Energy::isValidCenterRContour(const vector<cv::Point> &center_R_contour) {
    double cur_contour_area = contourArea(center_R_contour);
    if (cur_contour_area > energy_part_param_.CENTER_R_CONTOUR_AREA_MAX ||
        cur_contour_area < energy_part_param_.CENTER_R_CONTOUR_AREA_MIN) {
        // cout<<cur_contour_area<<" "<<energy_fan_param_.CONTOUR_AREA_MIN<<"
        // "<<energy_fan_param_.CONTOUR_AREA_MAX<<endl; cout<<"area fail."<<endl;
        return false;
        //选区面积大小不合适
    }
    RotatedRect cur_rect = minAreaRect(center_R_contour);
    Size2f cur_size = cur_rect.size;
    float length = cur_size.height > cur_size.width
                       ? cur_size.height
                       : cur_size.width;  //将矩形的长边设置为长
    float width = cur_size.height < cur_size.width
                      ? cur_size.height
                      : cur_size.width;  //将矩形的短边设置为宽
    if (length < energy_part_param_.CENTER_R_CONTOUR_LENGTH_MIN ||
        width < energy_part_param_.CENTER_R_CONTOUR_WIDTH_MIN ||
        length > energy_part_param_.CENTER_R_CONTOUR_LENGTH_MAX ||
        width > energy_part_param_.CENTER_R_CONTOUR_WIDTH_MAX) {
        // cout<<"length width fail."<<endl;
        //        cout << "length: " << length << '\t' << "width: " << width <<
        //        '\t' << cur_rect.center << endl;
        return false;
        //矩形边长不合适
    }
    float length_width_ratio = length / width;  //计算矩形长宽比
    if (length_width_ratio > energy_part_param_.CENTER_R_CONTOUR_HW_RATIO_MAX ||
        length_width_ratio < energy_part_param_.CENTER_R_CONTOUR_HW_RATIO_MIN) {
        // cout<<"length width ratio fail."<<endl;
        //        cout << "HW: " << length_width_ratio << '\t' <<
        //        cur_rect.center << endl;
        return false;
        //长宽比不合适
    }
    if (cur_contour_area / cur_size.area() <
        energy_part_param_.CENTER_R_CONTOUR_AREA_RATIO_MIN) {
        //        cout << "area ratio: " << cur_contour_area / cur_size.area()
        //        << '\t' << cur_rect.center << endl;
        return false;  //轮廓对矩形的面积占有率不合适
    }
    std::vector<cv::Point2f> intersection;
    if (rotatedRectangleIntersection(cur_rect, center_ROI, intersection) == 0) {
        return false;
    } else if (contourArea(intersection) <
               energy_part_param_.CENTER_R_CONTOUR_INTERSETION_AREA_MIN) {
        //        cout << "R intersection: " << contourArea(intersection) <<
        //        '\t' << cur_rect.center << endl;
        return false;
    }
    return true;
}

//----------------------------------------------------------------------------------------------------------------------
// 此函数用于判断找到的矩形候选区是否为流动条扇叶
// ---------------------------------------------------------------------------------------------------------------------
bool Energy::isValidFlowStripFanContour(
    cv::Mat &src, const vector<cv::Point> &flow_strip_fan_contour) {
    double cur_contour_area = contourArea(flow_strip_fan_contour);
    if (cur_contour_area > energy_part_param_.FLOW_STRIP_FAN_CONTOUR_AREA_MAX ||
        cur_contour_area < energy_part_param_.FLOW_STRIP_FAN_CONTOUR_AREA_MIN) {
        // cout<<"area fail."<<endl;
        //        cout << "area: " << cur_contour_area << '\t' << endl;
        return false;
        //选区面积大小不合适
    }
    //    cout << "area: " << cur_contour_area << endl;
    RotatedRect cur_rect = minAreaRect(flow_strip_fan_contour);
    Size2f cur_size = cur_rect.size;
    float length = cur_size.height > cur_size.width
                       ? cur_size.height
                       : cur_size.width;  //将矩形的长边设置为长
    float width = cur_size.height < cur_size.width
                      ? cur_size.height
                      : cur_size.width;  //将矩形的短边设置为宽
    if (length < energy_part_param_.FLOW_STRIP_FAN_CONTOUR_LENGTH_MIN ||
        width < energy_part_param_.FLOW_STRIP_FAN_CONTOUR_WIDTH_MIN ||
        length > energy_part_param_.FLOW_STRIP_FAN_CONTOUR_LENGTH_MAX ||
        width > energy_part_param_.FLOW_STRIP_FAN_CONTOUR_WIDTH_MAX) {
        //        cout<<"length width fail."<<endl;
        //        cout << "length: " << length << '\t' << "width: " << width <<
        //        '\t' << cur_rect.center << endl;
        return false;
        //矩形边长不合适
    }
    float length_width_ratio = length / width;  //计算矩形长宽比
    if (length_width_ratio >
            energy_part_param_.FLOW_STRIP_FAN_CONTOUR_HW_RATIO_MAX ||
        length_width_ratio <
            energy_part_param_.FLOW_STRIP_FAN_CONTOUR_HW_RATIO_MIN) {
        // cout<<"length width ratio fail."<<endl;
        //        cout << "HW: " << length_width_ratio << '\t' <<
        //        cur_rect.center << endl;
        return false;
        //长宽比不合适
    }
    //    cout << cur_contour_area / cur_size.area() << endl;
    if (cur_contour_area / cur_size.area() <
            energy_part_param_.FLOW_STRIP_FAN_CONTOUR_AREA_RATIO_MIN ||
        cur_contour_area / cur_size.area() >
            energy_part_param_.FLOW_STRIP_FAN_CONTOUR_AREA_RATIO_MAX) {
        //        cout << "area ratio: " << cur_contour_area / cur_size.area()
        //        << '\t' << cur_rect.center << endl;
        return false;
    }
    //    轮廓对矩形的面积占有率不合适
    double non_zero_rate = nonZeroRateOfRotateRect(src, cur_rect);
    //    cout<<non_zero_rate<<endl;
    //    if (non_zero_rate >
    //    energy_part_param_.FLOW_STRIP_FAN_NON_ZERO_RATE_MAX ||
    //        non_zero_rate <
    //        energy_part_param_.FLOW_STRIP_FAN_NON_ZERO_RATE_MIN){
    //        //cout<<non_zero_rate<<endl;
    //        return false;
    //    }
    //亮点占比不合格
    return true;
}

//----------------------------------------------------------------------------------------------------------------------
// 此函数用于判断找到的矩形候选区是否为流动条
// ---------------------------------------------------------------------------------------------------------------------
bool Energy::isValidFlowStripContour(
    const vector<cv::Point> &flow_strip_contour) {
    double cur_contour_area = contourArea(flow_strip_contour);
    if (cur_contour_area > energy_part_param_.FLOW_STRIP_CONTOUR_AREA_MAX ||
        cur_contour_area < energy_part_param_.FLOW_STRIP_CONTOUR_AREA_MIN) {
        //        cout<<"area fail."<<endl;

        return false;
        //选区面积大小不合适
    }
    RotatedRect cur_rect = minAreaRect(flow_strip_contour);
    Size2f cur_size = cur_rect.size;
    float length = cur_size.height > cur_size.width
                       ? cur_size.height
                       : cur_size.width;  //将矩形的长边设置为长
    float width = cur_size.height < cur_size.width
                      ? cur_size.height
                      : cur_size.width;  //将矩形的短边设置为宽
    if (length < energy_part_param_.FLOW_STRIP_CONTOUR_LENGTH_MIN ||
        width < energy_part_param_.FLOW_STRIP_CONTOUR_WIDTH_MIN ||
        length > energy_part_param_.FLOW_STRIP_CONTOUR_LENGTH_MAX ||
        width > energy_part_param_.FLOW_STRIP_CONTOUR_WIDTH_MAX) {
        //        cout<<"length width fail."<<endl;
        //        if(length>30)cout << "length: " << length << '\t' << "width: "
        //        << width << '\t' << cur_rect.center << endl;
        return false;
        //矩形边长不合适
    }
    //    cout << length << '\t' << width << endl;
    float length_width_ratio = length / width;  //计算矩形长宽比
    if (length_width_ratio >
            energy_part_param_.FLOW_STRIP_CONTOUR_HW_RATIO_MAX ||
        length_width_ratio <
            energy_part_param_.FLOW_STRIP_CONTOUR_HW_RATIO_MIN) {
        //        cout<<"hw fail."<<endl;
        //        if(length_width_ratio>3.5)cout << "HW: " << length_width_ratio
        //        << '\t' << cur_rect.center << endl;
        return false;
        //长宽比不合适
    }
    if (cur_contour_area / cur_size.area() <
        energy_part_param_.FLOW_STRIP_CONTOUR_AREA_RATIO_MIN) {
        //        if(cur_contour_area / cur_size.area()>0.5)cout << "area ratio:
        //        " << cur_contour_area / cur_size.area() << '\t' <<
        //        cur_rect.center << endl;
        return false;  //轮廓对矩形的面积占有率不合适
    }
    return true;
}
