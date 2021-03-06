﻿//
// Created by xixiliadorabarry on 1/24/19.
//
#include "energy.h"

using namespace cv;
using std::cout;
using std::endl;
using std::vector;


//----------------------------------------------------------------------------------------------------------------------
// 此函数为能量机关构造函数，只要程序不重启就不会重新构造
// ---------------------------------------------------------------------------------------------------------------------
Energy::Energy(RmSerial &u, int &color) : serial(u), ally_color(color),
                                            src_blue(SRC_HEIGHT, SRC_WIDTH, CV_8UC1),
                                            src_red(SRC_HEIGHT, SRC_WIDTH, CV_8UC1) {
    initEnergy();
    initEnergyPartParam();

    is_big = false;
    is_small = false;
}


//----------------------------------------------------------------------------------------------------------------------
// 此函数为能量机关析构函数，设置为默认
// ---------------------------------------------------------------------------------------------------------------------
Energy::~Energy() = default;

void Energy::sendEnergyLost(){
    sendTarget(serial,0,0,0,0);
}
//----------------------------------------------------------------------------------------------------------------------
// 此函数为大能量机关再初始化函数
// ---------------------------------------------------------------------------------------------------------------------
void Energy::setEnergyInit() {
    initEnergy();
    initEnergyPartParam();

    // FILE *fp = fopen("/Mark/delta.txt", "r");  //注意这里，要自己创个文件输入补偿
    // if (fp) {
    //     fscanf(fp, "delta_x: %d, delta_y: %d", &manual_delta_x, &manual_delta_y);
    //     fclose(fp);
    // }
}
