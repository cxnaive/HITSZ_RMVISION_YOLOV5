#include <armor_finder/armor_finder.h>
#include <rmserial.h>
#include <runtime.h>

#include <cmath>

inline static int bullet_speed_config_index(){
    int idx = 0;
    float mindis = 1000;
    for(int i = 0;i < config.ARMOR_BULLET_SPEED_SET.size();++i){
        float disnow = fabs(config.BULLET_SPEED - config.ARMOR_BULLET_SPEED_SET[i]);
        if(disnow < mindis){
            idx = i;
            mindis = disnow;
        }
    }
    return idx;
}

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
        pitch = atan((center.y - config.IMAGE_CENTER_Y + config.ARMOR_DELTA_Y) / config.camConfig.fy) *
                180 / PI;
    }
    int config_index = bullet_speed_config_index();
    double dpitch = config.ARMOR_PITCH_DELTA_K[config_index] * dist + config.ARMOR_PITCH_DELTA_B[config_index];
    pitch -= dpitch;
    
    //Apply filter
    ArmorPosFilter.update(cv::Point2f(yaw,pitch));
    cv::Vec4f res = ArmorPosFilter.predict();
    cv::Point2f pred_pos(res[0],res[1]);
    cv::Point2f pred_speed(res[0],res[1]);
    if (getPointLength(pred_pos - cv::Point2f(yaw,pitch)) < 5){
        yaw = pred_pos.x;
        pitch = pred_pos.y;
    }
    
    // Predict target pos
    // double target_yaw_speed = config.MCU_YAW_SPEED;
    // double dyaw = target_yaw_speed * (dist / 1000 / config.BULLET_SPEED);
    // yaw += dyaw;

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

    return sendTarget(serial, yaw, -pitch, dist, 1);
}

bool ArmorFinder::sendLostBox() { 
    PitchPID.clear();
    YawPID.clear();
    return sendTarget(serial, 0, 0, 0, 0); 
}