#include <armor_finder/armor_finder.h>
#include <runtime.h>
#include <cmath>
#include <rmserial.h>

static bool sendTarget(RmSerial &serial, double x, double y, double z, uint16_t shoot_delay,double y_offset = 0) {
    static short x_tmp, y_tmp, z_tmp;
    uint8_t buff[10];
    x_tmp = static_cast<short>(x);
    y_tmp = static_cast<short>(y);
    z_tmp = static_cast<short>(z);

    buff[0] = 's';
    buff[1] = static_cast<char>((x_tmp >> 8) & 0xFF);
    buff[2] = static_cast<char>((x_tmp >> 0) & 0xFF);
    buff[3] = static_cast<char>((y_tmp >> 8) & 0xFF);
    buff[4] = static_cast<char>((y_tmp >> 0) & 0xFF);
    buff[5] = static_cast<char>((z_tmp >> 8) & 0xFF);
    buff[6] = static_cast<char>((z_tmp >> 0) & 0xFF);
    buff[7] = static_cast<char>((shoot_delay >> 8) & 0xFF);
    buff[8] = static_cast<char>((shoot_delay >> 0) & 0xFF);
    buff[9] = 'e';
    //   cout << (short)(buff[3]<<8 | buff[4]) << endl;
    return serial.send_data(buff, sizeof(buff));
}
bool ArmorFinder::sendBoxPosition(uint16_t shoot_delay,double dist) {
    if (dist == -1 && target_box.rect == cv::Rect2d()) return false;
    if (shoot_delay) {
        LOG(INFO) << "next box" << shoot_delay << " ms";
    }
    static int fps_cnt = 0;
    static double last_time = 0;
    if(dist == -1) {
        auto pnp_result = target_box.armorSolvePnP();
        cv::Point3d trans = pnp_result.second;
        dist = trans.z;
        if (config.log_send_target){
            LOG(INFO) << "PNP: " << trans;
        }
        //calc_fps
        ++fps_cnt;
        double now_time = rmTime.seconds();
        if(now_time - last_time > 2){
            LOG(INFO) << "Armor fps: " << fps_cnt / (now_time - last_time);
            last_time = now_time;
            fps_cnt = 0;
        }
        double y_offset = 0;
        double yaw = atan(trans.x / trans.z) * 180 / PI;        
        double pitch = atan(trans.y / trans.z) * 180 / PI;
        return sendTarget(serial, yaw, -pitch, dist, shoot_delay,y_offset);
    }
    return sendTarget(serial, 0, 0, 0, shoot_delay, 0);
}