#include <rmserial.h>
#include <runtime.h>

bool RmSerial::send_data(uint8_t* data,size_t size){
    
}

bool RmSerial::recieve_data(){
    return false;   
}

bool RmSerial::init(){
    active_port = new serial::Serial(config.uart_port,115200,serial::Timeout::simpleTimeout(1000));
    if(active_port->isOpen()){
        LOG(INFO) << "Successfully initialized port  " << config.uart_port;
        return true;
    }
    else{
        LOG(ERROR) << "failed to initialize port  " << config.uart_port;
        return false;
    }
}