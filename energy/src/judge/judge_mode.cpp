//
// Created by sun on 19-7-12.
//

#include <energy.h>

using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// 此函数用于判断大小符
// ---------------------------------------------------------------------------------------------------------------------
void Energy::judgeMode() {
    getRecentTargetArmorCenters();
    if (recent_target_armor_centers.size() < 30) {
        return;
    } else {
        if (abs(recent_target_armor_centers.back() -
                recent_target_armor_centers.front()) > 10) {
            if (last_mode != BIG) {
                is_big = true;
                is_small = false;
                energy_mode_init = false;
                LOG(INFO) << "start big!";
            }
            last_mode = BIG;
        } else {
            if (last_mode != SMALL) {
                is_big = false;
                is_small = true;
                energy_mode_init = false;
                LOG(INFO) << "start small!";
            }
            last_mode = SMALL;
        }
    }
}