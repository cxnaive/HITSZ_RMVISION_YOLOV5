﻿//
// Created by xixiliadorabarry on 1/24/19.
//
#ifndef ENERGY_H
#define ENERGY_H

#include <iostream>
#include <vector>
#include <queue>
#include <runtime.h>
#include <opencv2/opencv.hpp>
#include <constants.h>
#include <stdio.h>
#include "param_struct_define.h"
#include <rmserial.h>
using std::vector;

class Energy {
public:
    Energy(RmSerial &u, int &color);//构造函数，参数为串口和颜色
    ~Energy();//默认析构函数

    bool is_big;//大符模式为true
    bool is_small;//小符模式为true

    void run(cv::Mat &src);

    RmSerial &serial;//串口
    void setEnergyInit();//设置能量机关初始化
    void sendEnergy();//发送能量机关数据
    void sendTarget(RmSerial& serial, float x, float y, float z, uint16_t u);//发送数据


private:
    EnergyPartParam energy_part_param_;//能量机关的参数设置

    bool is_guessing;//当前处于发弹到新目标出现的过程，则为true，此时猜测下一个目标
    bool is_predicting;//当前处于新目标出现到发弹的过程，则为true，此时正常击打
    bool energy_mode_init;//大小符状态判断
    bool energy_rotation_init;//若仍在判断风车旋转方向，则为true
    bool start_guess;//进入猜测状态的标志
    bool change_target;//目标切换的标志

    int &ally_color;//我方颜色

    int curr_fps;//帧率
    int send_cnt;//向主控板发送的数据总次数
    int fans_cnt;//扇叶个数
    int last_fans_cnt;//上一帧的扇叶个数
    int guess_devide;//刚进入猜测状态时，猜测目标点在极坐标中的分区
    int energy_rotation_direction;//风车旋转方向
    int clockwise_rotation_init_cnt;//装甲板顺时针旋转次数
    int anticlockwise_rotation_init_cnt;//装甲板逆时针旋转次数
    int last_mode;//上一帧的能量机关状态
    // int manual_delta_x, manual_delta_y;//手动微调量
    int extra_delta_x, extra_delta_y;//在风车运动到最高点附近的额外补偿量

    float target_polar_angle;//待击打装甲板的极坐标角度
    float last_target_polar_angle_judge_change;//上一帧待击打装甲板的极坐标角度（用于判断目标切换）
    float last_target_polar_angle_judge_rotation;//上一帧待击打装甲板的极坐标角度（用于判断旋向）
    float guess_polar_angle;//猜测的下一个目标装甲板极坐标角度
    float last_base_angle;//上一帧的各扇叶在0区（0°~72°）的基础角度
    float predict_rad;//预测提前角
    float predict_rad_norm;//预测提前角的绝对值
    float attack_distance;//步兵与风车平面距离
    float center_delta_yaw, center_delta_pitch;//对心时相差的角度
    float yaw_rotation, pitch_rotation;//云台yaw轴和pitch轴应该转到的角度
    float shoot;//给主控板的指令，1表示跟随，2表示发射，3表示目标切换,4表示猜测模式
    float last_yaw, last_pitch;//PID中微分项
    float sum_yaw, sum_pitch;//yaw和pitch的累计误差，即PID中积分项

    double time_start_guess;//进入猜测模式的时间

    cv::RotatedRect centerR;//风车中心字母R的可能候选区
    cv::RotatedRect flow_strip;//图像中所有流动条（理论上只有一个）
    cv::RotatedRect flow_strip_fan;//图像中所有流动条所在扇叶（理论上只有一个）
    cv::RotatedRect center_ROI;//风车中心候选区
    cv::RotatedRect target_armor;//目标装甲板（理论上仅一个）

    cv::Point circle_center_point;//风车圆心坐标
    cv::Point target_point;//目标装甲板中心坐标
    cv::Point guess_point;
    cv::Point predict_point;//预测的击打点坐标

    cv::Mat src_blue, src_red, src_green;//通道分离中的三个图像通道

    std::vector<cv::RotatedRect> fans;//图像中所有扇叶
    std::vector<cv::RotatedRect> armors;//图像中所有可能装甲板（可能存在误识别)
    std::vector<cv::RotatedRect> flow_strip_fans;//可能的流动扇叶
    std::vector<cv::RotatedRect> target_armors;//可能的目标装甲板
    std::vector<cv::RotatedRect> flow_strips;//可能的流动条
    std::vector<cv::Point> all_target_armor_centers;//记录全部的装甲板中心，用于风车圆心和半径的计算

    std::queue<float> recent_target_armor_centers;//记录最近一段时间的装甲板中心，用于判断大符还是小符


    void initEnergy();//能量机关初始化
    void initEnergyPartParam();//能量机关参数初始化
    void initRotation();//顺逆时针初始化

    void clearAll();//清空各vector
    void initImage(cv::Mat &src);//图像预处理

    bool stayGuessing();//保持在猜测模式

    int findFans(const cv::Mat &src);//寻找图中所有扇叶
    int findArmors(const cv::Mat &src);//寻找图中所有装甲板
    bool findCenterR(const cv::Mat &src);//寻找图中可能的风车中心字母R
    bool findFlowStrip(const cv::Mat &src);//寻找图中的流动条
    bool findCenterROI(const cv::Mat &src);//框取中心R候选区
    bool findFlowStripFan(const cv::Mat &src);//寻找图中的流动条所在扇叶
    bool findFlowStripWeak(const cv::Mat &src);//弱识别寻找图中的流动条
    bool findTargetInFlowStripFan();//在已发现的流动条区域中寻找待击打装甲板

    bool isValidFanContour(cv::Mat &src, const vector<cv::Point> &fan_contour);//扇叶矩形尺寸要求
    bool isValidArmorContour(const vector<cv::Point> &armor_contour);//装甲板矩形尺寸要求
    bool isValidCenterRContour(const vector<cv::Point> &center_R_contour);//风车中心选区尺寸要求
    bool isValidFlowStripContour(const vector<cv::Point> &flow_strip_contour);//流动条扇叶矩形尺寸要求
    bool isValidFlowStripFanContour(cv::Mat &src, const vector<cv::Point> &flow_strip_fan_contour);//流动条扇叶矩形尺寸要求

    void showFans(std::string windows_name, const cv::Mat &src);//显示扇叶
    void showArmors(std::string windows_name, const cv::Mat &src);//显示装甲板
    void showCenterR(std::string windows_name, const cv::Mat &src);//显示风车中心候选区R
    void showFlowStrip(std::string windows_name, const cv::Mat &src);//显示流动条
    void showFlowStripFan(std::string windows_name, const cv::Mat &src);//显示流动条所在扇叶
    void showGuessTarget(std::string windows_name, const cv::Mat &src);//显示猜测点位

    int devide(float angle);//将极坐标分为五个区域，判断一个角度处于哪个区域
    int linePointX(const cv::Point2f &p1, const cv::Point2f &p2, int y);//计算直线上一点横坐标
    void rotate(cv::Point target_point);//获取预测点位
    double pointDistance(cv::Point point_1, cv::Point point_2);//计算两点距离
    double nonZeroRateOfRotateRect(cv::Mat &bin, const cv::RotatedRect &rotatedRect);//计算旋转矩形内亮点占比

    void writeDownMark(cv::Mat &src);//记录操作手的手动微调

    bool guessTarget();//获得猜测击打点位
    void changeTarget();//判断目标是否改变
    void getCenter();//对心
    void multipleMode(cv::Mat &src);//多模式切换
    void getTargetPolarAngle();//获得目标装甲板极坐标角度
    void getPredictPoint(cv::Point target_point);//获取预测点位
    void getAimPoint(cv::Point target_point);//通过自瞄逻辑计算点位
    void getRecentTargetArmorCenters();//记录近30帧目标装甲板中心坐标

    void judgeMode();//判断大符还是小符
    void judgeShoot();//判断是否可以发弹
    bool isGuessingTimeout();//判断猜测模式是否超时（没打中）

    void splitBayerBG(cv::Mat src, cv::Mat &blue, cv::Mat &red);//拜耳阵列分离
    void imagePreprocess(cv::Mat &src);//图像通道分离

    void FanStruct(cv::Mat &src);//腐蚀和膨胀
    void ArmorStruct(cv::Mat &src);//腐蚀和膨胀
    void FlowStripFanStruct(cv::Mat &src);//腐蚀和膨胀
    void FlowStripStruct(cv::Mat &src);//腐蚀和膨胀
    void CenterRStruct(cv::Mat &src);//腐蚀和膨胀
};

#endif //ENERGY_H

