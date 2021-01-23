#include <rmserial.h>
#include <runtime.h>

#include <thread>

// receive data
std::mutex receive_mtx;
McuConfig receive_config_data;

RmSerial::~RmSerial() { stop_thread(); }
bool RmSerial::send_data(uint8_t* data, size_t size) {
    return active_port->write(data, size) == size;
}
bool RmSerial::send_data(const SendData& data) {
    return send_data((uint8_t*)(&data), sizeof(SendData));
}

void proccess_data(uint8_t* s, uint8_t* e) {
    if (e - s != sizeof(McuData)) {
        LOG(ERROR) << "Invalid MCU Data (Invalid Size:" << e - s << ")";
        return;
    }
    McuData mcu_data;
    memcpy(&mcu_data, s, sizeof(McuData));
    if (mcu_data.start_flag != 's') {
        LOG(ERROR) << "Invalid MCU Data (Invalid Data)";
        return;
    }
    receive_mtx.lock();
    switch (mcu_data.type) {
        case MCU_PAN_TYPE:
            readPanMcuData(&mcu_data, &receive_config_data.curr_yaw,
                           &receive_config_data.curr_pitch);
            if (config.show_mcu_info)
                LOG(INFO) << "Recieve Mcu Pan: "
                          << "Yaw: " << receive_config_data.curr_yaw
                          << " Pitch: " << receive_config_data.curr_pitch;
            break;
        case MCU_CONFIG_TYPE:
            readConfigMcuData(&mcu_data, &receive_config_data.state,
                              &receive_config_data.anti_top,
                              &receive_config_data.enemy_color);
            if (config.show_mcu_info)
                LOG(INFO) << "Recieve Mcu Config: "
                          << "State: " << receive_config_data.state
                          << " Anti top: " << (int)receive_config_data.anti_top
                          << " Enemy color:" << (int)receive_config_data.enemy_color;
            break;
        case MCU_ENERGY_TYPE:
            readEnergyMcuData(&mcu_data, &receive_config_data.delta_x,
                              &receive_config_data.delta_y);
            if (config.show_mcu_info)
                LOG(INFO) << "Recieve Mcu Energy: "
                          << "Delta x: " << receive_config_data.delta_x
                          << " Delta y: " << receive_config_data.delta_y;         
            break;
        case MCU_SPEED_TYPE:
            readSpeedMcuData(&mcu_data, &receive_config_data.bullet_speed);
            if (config.show_mcu_info)
                LOG(INFO) << "Recieve Mcu Bullet Speed: " << receive_config_data.bullet_speed; 
            break;
        default:
            break;
    }
    receive_mtx.unlock();
}

void recieve_data(RmSerial* rm_serial) {
    LOG(INFO) << "recieve thread started!";
    static uint8_t buff[100];
    uint8_t* buffer_tail = buff;
    serial::Serial* port = rm_serial->active_port;
    while (rm_serial->thread_running) {
        size_t wait_in_buffer = port->available();
        if (wait_in_buffer) {
            port->read(buffer_tail, wait_in_buffer);
            buffer_tail += wait_in_buffer;
            if (buffer_tail[-1] == 'e') {
                *buffer_tail = 0;
                proccess_data(buff, buffer_tail);
                buffer_tail = buff;
            }
        }
    }
}

void RmSerial::start_thread() {
    if (init_success) {
        thread_running = true;
        std::thread task(recieve_data, this);
        task.detach();
    }
}
void RmSerial::stop_thread() {
    if (init_success) {
        thread_running = false;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

bool RmSerial::init() {
    LOG(INFO) << "Serial Send Size:" << sizeof(SendData);
    LOG(INFO) << "Serial Recieve Size:" << sizeof(McuData);
    active_port = new serial::Serial(config.uart_port, 115200,
                                     serial::Timeout::simpleTimeout(1000));
    init_success = true;
    //初始化数据接受结构体
    receive_config_data.anti_top = config.ANTI_TOP;
    receive_config_data.bullet_speed = config.BULLET_SPEED;
    receive_config_data.delta_x = config.MCU_DELTA_X;
    receive_config_data.delta_y = config.MCU_DELTA_Y;
    receive_config_data.enemy_color = config.ENEMY_COLOR;
    receive_config_data.state = config.RUNMODE;
    //开启数据接受线程
    start_thread();
    if (active_port->isOpen()) {
        LOG(INFO) << "Successfully initialized port " << config.uart_port;
        return true;
    } else {
        LOG(ERROR) << "failed to initialize port " << config.uart_port;
        return false;
    }
}
