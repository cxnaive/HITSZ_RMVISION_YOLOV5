#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#define PI (3.14159265459)

#define ENEMY_BLUE 0
#define ENEMY_RED 1

#define ALLY_BLUE ENEMY_RED
#define ALLY_RED ENEMY_BLUE

#define BIG_ENERGY_STATE 'b'
#define SMALL_ENERGY_STATE 's'
#define ARMOR_STATE 'a'

#define RED_COMPENSATE_YAW (0)
#define RED_COMPENSATE_PITCH (0)
#define BLUE_COMPENSATE_YAW (0)
#define BLUE_COMPENSATE_PITCH (0)
// #define EXTRA_DELTA_X (0)
// #define EXTRA_DELTA_Y (0)
// #define YAW_AIM_KD (0.4)
// #define YAW_AIM_KP (0.75)
// #define YAW_AIM_KI (0.01)
// #define PITCH_AIM_KD (0.4)
// #define PITCH_AIM_KP (0.75)
// #define PITCH_AIM_KI (0.01)

const int BIG = 1;
const int SMALL = 0;
const int SRC_WIDTH = 320;
const int SRC_HEIGHT = 320;
const int CLOCKWISE = 1;
const int ANTICLOCKWISE = -1;
// const float ATTACK_DISTANCE = 718.0;//cm
const float ATTACK_DISTANCE = 750.0;               // cm
const double ARMOR_CENTER_TO_CYCLE_CENTER = 75.0;  // cm

#endif /* _CONSTANTS_H */