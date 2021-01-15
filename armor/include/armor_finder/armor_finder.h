//
// Created by xinyang on 19-3-27.
//

#ifndef _ARMOR_FINDER_H_
#define _ARMOR_FINDER_H_

#include <armor_finder/armor_box.h>
#include <armor_finder/light_blobs.h>
#include <rmserial.h>
#include <runtime.h>
#include <functional>
#include <ArmorDetector.h>
#include <string>
#include <map>
#include <opencv2/core.hpp>

#define BLOB_RED ENEMY_RED
#define BLOB_BLUE ENEMY_BLUE

#define BOX_RED ENEMY_RED
#define BOX_BLUE ENEMY_BLUE

extern std::map<int, std::string> id2name;  //装甲板id到名称的map
extern std::map<std::string, int> name2id;  //装甲板名称到id的map
extern std::map<std::string, int> prior_blue;
extern std::map<std::string, int> prior_red;
extern std::map<std::string, int> name2color; //装甲板名称到颜色的map

/********************* 自瞄类定义 **********************/
class ArmorFinder {
   public:
    ArmorFinder(const int &color, RmSerial &u, const int &anti_top);
    ~ArmorFinder() = default;

   private:

    typedef enum {
        SEARCHING_STATE,
        TRACKING_STATE,
        STANDBY_STATE
    } State;  // 自瞄状态枚举定义

    double frame_time;       // 当前帧对应时间
    const int &enemy_color;  // 敌方颜色，引用外部变量，自动变化
    const int &is_anti_top;  // 进入反陀螺，引用外部变量，自动变化
    State state;             // 自瞄状态对象实例
    ArmorBox target_box, last_box;  // 目标装甲板
    int anti_switch_cnt;            // 防止乱切目标计数器
    ArmorDetector netDetector; //YOLOV5 多目标识别寻找装甲版
    int contour_area;  // 装甲区域亮点个数，用于数字识别未启用时判断是否跟丢（已弃用）
    int tracking_cnt;  // 记录追踪帧数，用于定时退出追踪
    RmSerial &serial;  // 串口对象，引用外部变量，用于和能量机关共享同一个变量
    double last_front_time;  // 上次陀螺正对时间
    int anti_top_cnt;
    RoundQueue<double, 4> top_periodms;  // 陀螺周期循环队列
    std::vector<double> time_seq;             // 一个周期内的时间采样点
    std::vector<float> angle_seq;             // 一个周期内的角度采样点

    bool findLightBlobs(const cv::Mat &src, LightBlobs &light_blobs);
    bool findAccurateArmorBox(const cv::Mat &src, ArmorBox &box);
    bool locateArmorBox(const cv::Mat &src, const ArmorInfo & target); //根据多目标识别的结果寻找灯条信息，精确定位装甲版
    bool matchArmorBoxes(const cv::Mat &src, const LightBlobs &light_blobs,
                         ArmorBoxes &armor_boxes);
    std::vector<ArmorInfo> filterArmorInfoByColor(const std::vector<ArmorInfo>& armors,const cv::Mat &src); //根据颜色过滤装甲板信息
    bool stateSearchingTarget(cv::Mat &src);  // searching state主函数
    bool stateTrackingTarget(cv::Mat &src);   // tracking state主函数
    bool stateStandBy();  // stand by state主函数（已弃用）

    void antiTop();  // 反小陀螺

    bool sendBoxPosition(uint16_t shoot, double dist = -1);  // 和主控板通讯
   public:
    void run(cv::Mat &src);  // 自瞄主函数
};

#endif /* _ARMOR_FINDER_H_ */
