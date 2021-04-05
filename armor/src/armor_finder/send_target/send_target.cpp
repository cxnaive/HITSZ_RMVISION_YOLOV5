#include <armor_finder/armor_finder.h>
#include <rmserial.h>
#include <runtime.h>

#include <cmath>

static bool sendTarget(RmSerial &serial, double x, double y, double z,
                       uint16_t shoot_delay) {
    static short x_tmp, y_tmp, z_tmp;
    // uint8_t buff[10];
    SendData data;
    data.start_flag = 's';
    data.x = static_cast<float>(x);
    data.y = static_cast<float>(y);
    data.z = static_cast<float>(z);
    data.u = shoot_delay;
    data.end_flag = 'e';
    if(config.log_send_target ) LOG(INFO) << "Target:" << data.x << " " << data.y;
    return serial.send_data(data);
}
bool ArmorFinder::sendBoxPosition(uint16_t shoot_delay) {
    if (shoot_delay) {
        LOG(INFO) << "next box" << shoot_delay << " ms";
    }
    static int fps_cnt = 0;
    static double last_time = 0;

    cv::Point3d trans;
    double dist;
    double yaw, pitch;
    if (config.use_pnp) {
        auto pnp_result = target_box.armorSolvePnP();
        trans = pnp_result.second;
        dist = trans.z;
        yaw = atan(trans.x / trans.z) * 180 / PI;
        pitch = atan(trans.y / trans.z) * 180 / PI;
    } else {
        dist = target_box.getBoxDistance();
        cv::Point2f center = target_box.getCenter();
        yaw = atan((center.x - config.IMAGE_CENTER_X + config.ARMOR_DELTA_X) / config.camConfig.fx) *
              180 / PI;
        pitch = atan((center.y - config.IMAGE_CENTER_Y) / config.camConfig.fy) *
                180 / PI;
    }
    double dpitch = config.ARMOR_PITCH_DELTA_K * dist + config.ARMOR_PITCH_DELTA_B;
    pitch -= dpitch;

    if (config.log_send_target) {
        LOG(INFO) << "Target: " << yaw <<" "<< -pitch;
    }
    // calc_fps
    ++fps_cnt;
    double now_time = rmTime.seconds();
    if (now_time - last_time > 2) {
        LOG(INFO) << "Armor fps: " << fps_cnt / (now_time - last_time);
        last_time = now_time;
        fps_cnt = 0;
    }
    //使用PID控制
    yaw = YawPID.updateError(yaw);
    pitch = PitchPID.updateError(pitch);

    return sendTarget(serial, yaw, -pitch, 1, shoot_delay);
}

bool ArmorFinder::sendLostBox() { 
    PitchPID.clear();
    YawPID.clear();
    return sendTarget(serial, 0, 0, 0, 0); 
}