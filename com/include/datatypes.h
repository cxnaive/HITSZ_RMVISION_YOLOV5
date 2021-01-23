#ifndef H_RM_SERIAL_DATA_TYPES
#define H_RM_SERIAL_DATA_TYPES
#include <stdint.h>
#pragma pack(push, 1)
typedef struct _SendData {
    uint8_t start_flag;
    float x;     // Yaw
    float y;     // Pitch
    float z;     // state
    uint16_t u;  // shoot_delay;
    uint8_t end_flag;
} SendData;

#define MCU_PAN_TYPE 0
#define MCU_CONFIG_TYPE 1
#define MCU_ENERGY_TYPE 2
#define MCU_SPEED_TYPE 3

typedef struct _McuData {
    uint8_t start_flag;
    uint8_t type;
    uint32_t x;
    uint32_t y;
    uint8_t end_flag;
} McuData;

typedef struct _PanData {
    uint8_t start_flag;
    uint8_t type;
    float yaw;
    float pitch;
    uint8_t end_flag;
} PanData;

typedef struct _ConfigData {
    uint8_t start_flag;
    uint8_t type;
    uint8_t state;
    uint8_t anti_top;
    uint8_t enemy_color;
    uint8_t empty_buff[5];
    uint8_t end_flag;
} ConfigData;

typedef struct _EnergyData {
    uint8_t start_flag;
    uint8_t type;
    int delta_x;
    int delta_y;
    uint8_t end_flag;
} EnergyData;

typedef struct _SpeedData {
    uint8_t start_flag;
    uint8_t type;
    float speed;
    float empty_buff;
    uint8_t end_flag;
} SpeedData;

#pragma pack(pop)

McuData generatePanMcuData(float yaw, float pitch);
McuData generateConfigMcuData(uint8_t state, uint8_t anti_top,
                              uint8_t enemy_color);
McuData generateEnergyMcuData(int delta_x, int delta_y);
McuData generateSpeedMcuData(float speed);

void readPanMcuData(McuData* data, float* yaw, float* pitch);
void readConfigMcuData(McuData* data, uint8_t* state, uint8_t* anti_top,
                       uint8_t* enemy_color);
void readEnergyMcuData(McuData* data, int* delta_x, int* delta_y);
void readSpeedMcuData(McuData* data, float* speed);
#endif