#include<rmtime.h>

void RmTime::init(){
    begin = std::chrono::steady_clock::now();
}

long long RmTime::nanoseconds(){
    auto now = std::chrono::steady_clock::now();
    return (now - begin).count();
}

double RmTime::milliseconds(){
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(now - begin).count();
}

double RmTime::seconds(){
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(now - begin).count();
}

void RmTime::sleep(double milli){
    std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(milli));
}