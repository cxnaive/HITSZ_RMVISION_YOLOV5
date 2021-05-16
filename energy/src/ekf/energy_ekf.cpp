#include <energy_ekf.h>
#include <cmath>

void EnergyEKF::init(double _time_interval,EnerygyFunc _H_jacob_func,EnerygyFunc _S_func){
    time_interval = _time_interval;
    H_func = _H_jacob_func;
    S_func = _S_func;
    X = P2 = P1 = kg2 = 0;
    Q = 0.01;
    R = 30000;
}

double EnergyEKF::update(double speed){
    X = X + time_interval;
    P2 = P1 + Q;
    double tmp = H_func(X);
    kg2 = P2 * tmp / (P2 * tmp * tmp + R);
    P1 = (1 - kg2 * tmp) * P2;
    X = X + kg2 * (speed - S_func(X));
    return X;
}

double func_speed(double t){
    double spd_rad = 0.785 * sin(t * 1.884) + 1.305;
    return spd_rad * 180 / M_PI;
}

double func_acc(double t){
    double spd_rad = 0.785 * 1.884 * cos(t * 1.884);
    return spd_rad * 180 / M_PI;
}

double func_pos(double t){
    double polar_angle = -0.785 / 1.884 * cos(1.884 * t) + 1.305 * t;
    return polar_angle * 180 / M_PI; 
}