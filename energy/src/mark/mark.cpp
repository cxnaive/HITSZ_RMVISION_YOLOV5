//
// Created by sun on 19-7-5.
//

#include <energy.h>

using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// 此函数用于记录操作手的微调dx和dy
// ---------------------------------------------------------------------------------------------------------------------
void Energy::writeDownMark(cv::Mat &src) {
    if (fans_cnt >= 2) {
        // FILE *fp_delta = fopen("/Mark/delta.txt", "w");
        // if (fp_delta) {
        //     fprintf(fp_delta, "delta_x: %d, delta_y: %d\n",
        //             config.MCU_DELTA_X + manual_delta_x,
        //             config.MCU_DELTA_Y + manual_delta_y);
        //     fclose(fp_delta);
        // }
        config.MANUAL_DELTA_X += config.MCU_DELTA_X;
        config.MANUAL_DELTA_Y += config.MCU_DELTA_Y;
    }
    // FILE *fp_data = fopen("/Mark/data.txt", "a");
    // if (fp_data) {
    //     if (mcu_data.mark == 1) {
    //         fprintf(fp_data, "PID: %s\t", "new");
    //     } else {
    //         fprintf(fp_data, "PID: %s\t", "default");
    //     }
    //     if (is_big) {
    //         fprintf(fp_data, "state: %s\t", "big");
    //     } else if (is_small) {
    //         fprintf(fp_data, "state: %s\t", "small");
    //     }
    //     fprintf(fp_data, "fps: %d\t", curr_fps);
    //     fprintf(fp_data, "fans_cnt: %d\t", fans_cnt);
    //     fprintf(fp_data, "shoot: %d\t", int(shoot));
    //     fprintf(fp_data, "yaw: %lf , pitch: %lf\t", yaw_rotation,
    //             pitch_rotation);
    //     fprintf(fp_data, "delta_x: %d, delta_y: %d\n",
    //             mcu_data.delta_x + manual_delta_x,
    //             mcu_data.delta_y + manual_delta_y);
    //     fclose(fp_data);
    // }
}
