#include <energy.h>

using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// 此函数用于对心
// ---------------------------------------------------------------------------------------------------------------------
void Energy::getCenter() {
    int compensate_yaw = 0, compensate_pitch = 0;
    if (config.ENEMY_COLOR == ENEMY_BLUE) {
        compensate_yaw = RED_COMPENSATE_YAW;
        compensate_pitch = RED_COMPENSATE_PITCH;
    } else if (config.ENEMY_COLOR == ENEMY_RED) {
        compensate_yaw = BLUE_COMPENSATE_YAW;
        compensate_pitch = BLUE_COMPENSATE_PITCH;
    }
    double dx = -(circle_center_point.x - 320 - compensate_yaw);
    double dy = -(circle_center_point.y - 320 - compensate_pitch);

    yaw_rotation = atan(dx / config.camConfig.fx) * 180 / PI;
    pitch_rotation = atan(dy / config.camConfig.fy) * 180 / PI;
    if (abs(yaw_rotation) < 0.7 && abs(pitch_rotation) < 0.7) {
        shoot = 2;
    } else
        shoot = 1;
}
