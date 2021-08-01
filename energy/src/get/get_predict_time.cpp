#include <energy.h>

void Energy::getTargetTime() {
    if (is_small) {
        predict_time_init = false;
    } else {
        //初始化跳过
        double time_now = rmTime.milliseconds();
        if(last_target_polar_angle_predict_time == -1000){
            last_target_polar_angle_predict_time = target_polar_angle;
            last_target_polar_angle_time_point = time_now;
            return;
        }
        //过+-180度，修正角度
        double correction_angle = 0;
        if (fabs(target_polar_angle - last_target_polar_angle_predict_time) >
            180) {
            double sig = energy_rotation_direction == CLOCKWISE ? 1 : -1;
            correction_angle += sig * 360;
        }
        //目标切换，修正角度
        if (fabs(target_polar_angle - last_target_polar_angle_predict_time - correction_angle) >
            30) {
            double change_angle =
                target_polar_angle - last_target_polar_angle_predict_time - correction_angle;
            int change_cnt = round(change_angle / (360 / 5));
            LOG(INFO) << "change target angle:" << change_angle
                      << " cnt:" << change_cnt;
            correction_angle += change_cnt * (360 / 5);
        }
        if (correction_angle != 0) {
            for (auto bg = target_angles.begin(); bg != target_angles.end();
                 ++bg) {
                bg->x += correction_angle;
            }
            last_target_polar_angle_predict_time += correction_angle;
        }

        target_angles.push_back(cv::Point(target_polar_angle, time_now));
        if (target_angles.size() > moving_average_cnt) {
            target_angles.pop_front();
        }
        if (target_angles.size() == moving_average_cnt) {
            auto p1 = target_angles.at(0);
            auto p2 = target_angles.at(moving_average_cnt - 1);
            double calc_time_point = (p2.y + p1.y) / 2000;
            double calc_spd = fabs(p2.x - p1.x) / (p2.y - p1.y) * 1000;
            if(energy_rotation_direction == CLOCKWISE) calc_spd *= -1;
            double calc_predict_time = calc_time_point - ekf.update(calc_spd);
            LOG(INFO) << "calc_predict_time: " << calc_predict_time;
            temp_predict_times.push_back(calc_predict_time);
            if (temp_predict_times.size() > 10) {
                temp_predict_times.pop_front();
            }
            if (temp_predict_times.size() == 10) {
                //计算最大值，最小值，均值
                double vsum = 0, vmax = -10000000, vmin = 10000000;
                for (auto& value : temp_predict_times) {
                    vsum += value;
                    vmax = std::max(vmax, value);
                    vmin = std::min(vmin, value);
                }
                if (vmax - vmin < 0.5) {
                    double valid_time_predict =
                        vsum / temp_predict_times.size();
                    if (predict_time_init) {
                        predict_time = valid_time_predict;
                        predict_time_init = false;
                        predict_time_cnt = 1;
                        predict_time_sum = valid_time_predict;
                    } else {
                        ++predict_time_cnt;
                        predict_time_sum += valid_time_predict;
                        predict_time = predict_time_sum / predict_time_cnt;
                    }
                }
            }
        }
        last_target_polar_angle_predict_time = target_polar_angle;
        last_target_polar_angle_time_point = time_now;
        LOG(INFO) << "predict time: " << predict_time;
    }
    // predict_time_init = false;
}