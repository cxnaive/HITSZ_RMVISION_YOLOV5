#ifndef ENERGY_EKF_H
#define ENERGY_EKF_H
#include <functional>

typedef std::function<double(double)> EnerygyFunc;

class EnergyEKF{
private:
    double time_interval;
    double X,P2,P1,Q,R,kg2;
    EnerygyFunc H_func,S_func;
public:
    void init(double _time_interval,EnerygyFunc _H_jacob_func,EnerygyFunc _S_func);
    double update(double speed);
};

double func_speed(double t);
double func_acc(double t);
double func_pos(double t);

#endif