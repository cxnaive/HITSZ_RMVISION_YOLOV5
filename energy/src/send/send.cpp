//
// Created by xixiliadorabarry on 1/24/19.
//
#include <energy.h>

using namespace std;

#define MINMAX(value, min, max) \
    value = ((value) < (min)) ? (min) : ((value) > (max) ? (max) : (value))

//----------------------------------------------------------------------------------------------------------------------
// 此函数用于发送能量机关数据
// ---------------------------------------------------------------------------------------------------------------------
void Energy::sendEnergy() {
    // sum_yaw += yaw_rotation;
    // sum_pitch += pitch_rotation;
    // float yaw_I_component = YAW_AIM_KI * sum_yaw;
    // float pitch_I_component = PITCH_AIM_KI * sum_pitch;
    // MINMAX(yaw_I_component, -2, 2);
    // MINMAX(pitch_I_component, -2, 2);

    // double tmp_yaw = yaw_rotation;
    // double tmp_pitch = pitch_rotation;
    // /* yaw_rotation = YAW_AIM_KP * yaw_rotation + YAW_AIM_KI * sum_yaw +
    //                 YAW_AIM_KD * (yaw_rotation - last_yaw);
    //  pitch_rotation = PITCH_AIM_KP * pitch_rotation + PITCH_AIM_KI *
    //  sum_pitch +
    //                   PITCH_AIM_KD * (pitch_rotation - last_pitch);
    //                   */

    // last_yaw = tmp_yaw;
    // last_pitch = tmp_pitch;
    static double last_time = rmTime.seconds();
    static int fps_cnt = 0;
    double now_time = rmTime.seconds();
    fps_cnt += 1;
    if (now_time - last_time > 2) {
        LOG(INFO) << "energy send fps:" << fps_cnt / (now_time - last_time);
        fps_cnt = 0;
        last_time = now_time;
    }
    if (change_target) {
        sendTarget(serial, yaw_rotation, pitch_rotation, 0, 3);  //表示目标切换
    } else if (is_guessing) {
        sendTarget(serial, yaw_rotation, pitch_rotation, 0, 4);  //表示猜测模式
    } else {
        sendTarget(serial, yaw_rotation, pitch_rotation, 0,
                   shoot);  //跟随或发弹
    }
}

//----------------------------------------------------------------------------------------------------------------------
// 此函数用于发送数据给主控板
// ---------------------------------------------------------------------------------------------------------------------
void Energy::sendTarget(RmSerial &serial, float x, float y, float z,
                        uint16_t u) {
    // short x_tmp, y_tmp, z_tmp;
    // uint8_t buff[10];
    SendData data;
    data.start_flag = 's';
    data.x = static_cast<float>(x);
    data.y = static_cast<float>(y);
    data.z = static_cast<float>(z);
    data.u = u;
    data.end_flag = 'e';
    serial.send_data(data);
    send_cnt += 1;
    // x_tmp = static_cast<short>(x * (32768 - 1) / 100);
    // y_tmp = static_cast<short>(y * (32768 - 1) / 100);
    // z_tmp = static_cast<short>(z * (32768 - 1) / 100);
    // buff[0] = 's';
    // buff[1] = static_cast<char>((x_tmp >> 8) & 0xFF);
    // buff[2] = static_cast<char>((x_tmp >> 0) & 0xFF);
    // buff[3] = static_cast<char>((y_tmp >> 8) & 0xFF);
    // buff[4] = static_cast<char>((y_tmp >> 0) & 0xFF);
    // buff[5] = static_cast<char>((z_tmp >> 8) & 0xFF);
    // buff[6] = static_cast<char>((z_tmp >> 0) & 0xFF);
    // buff[7] = static_cast<char>((u >> 8) & 0xFF);
    // buff[8] = static_cast<char>((u >> 0) & 0xFF);
    // buff[9] = 'e';
    // serial.send_data(buff, sizeof(buff));

    //    LOGM(STR_CTR(WORD_LIGHT_PURPLE, "send"));
}
