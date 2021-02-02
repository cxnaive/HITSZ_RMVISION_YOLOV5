#include<PidPosition.h>

#define MINMAX(value, min, max) \
    value = ((value) < (min)) ? (min) : ((value) > (max) ? (max) : (value))

PidPosition::PidPosition(double _KP,double _KI,double _KD,double _sum_max,double _output_max){
    KP = _KP;
    KI = _KI;
    KD = _KD;
    sum_max = _sum_max;
    output_max = _output_max;
    sum = output = last_error = 0;
}

double PidPosition::updateError(double error){
    sum += error;
    MINMAX(sum,-sum_max,sum_max);
    output = KP * error + KI * sum + KD * (error - last_error);
    last_error = error;
    return output;
}

void PidPosition::clear(){
    sum = output = last_error = 0;
}