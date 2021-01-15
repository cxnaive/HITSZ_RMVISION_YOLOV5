#ifndef H_RMTIME_H
#define H_RMTIME_H
#include <chrono>
#include <thread>

//系统时间工具类
class RmTime{
public:
    std::chrono::steady_clock::time_point begin;
    void init();
    long long nanoseconds();
    double milliseconds();
    double seconds();
    void sleep(double milli);
};

#endif