#include<serial/serial.h>

#ifndef H_RMSERIAL
#define H_RMSERIAL

class RmSerial{
public:
    serial::Serial *active_port;
    bool init();
    bool isConnected(){
        return active_port->isOpen();
    }
    bool send_data(uint8_t* data,size_t size);
    bool recieve_data();
};
#endif