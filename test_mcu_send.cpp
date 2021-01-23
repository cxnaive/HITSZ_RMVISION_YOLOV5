#include <armor_finder/armor_finder.h>
#include <camera/cam_wrapper.h>
#include <camera/video_wrapper.h>
#include <energy.h>
#include <glog/logging.h>
#include <rmconfig.h>
#include <rmserial.h>
#include <rmtime.h>

#include <csignal>
#include <opencv2/opencv.hpp>

//程序运行时信息
RmConfig config;
//程序运行时时间类
RmTime rmTime;
//串口实例
RmSerial rmSerial;
//摄像头实例
Camera* cam = nullptr;
// Video实例
VideoWrapper* video = nullptr;
// ArmorFinder实例
ArmorFinder* armor_finder;
// Energy 实例
Energy* energy;
//运行时原图实例
cv::Mat src;
int main() {
    serial::Serial port("/dev/ttyACM0", 115200,
                        serial::Timeout::simpleTimeout(1000));
    float yaw, pitch;
    while (std::cin >> yaw >> pitch) {
        McuData now = generatePanMcuData(yaw, pitch);
        port.write((uint8_t*)(&now), sizeof(McuData));
    }
}