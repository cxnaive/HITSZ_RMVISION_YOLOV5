# 哈尔滨工业大学深圳 RoboMaster 2021赛季 视觉代码

本代码是哈尔滨工业大学深圳RoboMaster2021赛季车辆的视觉部分，分为三个模块:**装甲板识别**，**能量机关**，以及**封装的设备驱动和配置文件**。可以提取能量机关以外的模块并修改main函数直接作为哨兵识别代码。 

本代码统一使用**640×640**大小的图像进行处理


**如有BUG或者想交流的朋友欢迎积极联系我们**

---

运行效果：自瞄帧率120（摄像头最大帧率）,识别距离根据环境不同大约8米左右(5mm焦距镜头)。


## 一、代码运行环境

| 硬件设备                                             | 操作系统                                     | 运行库                                                       | ToolChain                                                  |
| ---------------------------------------------------- | -------------------------------------------- | ------------------------------------------------------------ | ---------------------------------------------------------- |
| Nvidia Xavier NX<br />大恒工业相机×１<br />USB转TTL×１ | Ubuntu20.04<br />Ubuntu18.04 | OpenCV4.5.1<br />opencv_contrib4.5.1<br />大恒相机驱动 <br /> GLOG <br /> TensorRT | Ubuntu18/20 : cmake3.16 + build-essential <br />|

**实际装载在步兵和哨兵上的运行环境为Ubuntu18.04(JetPack 4.4.1)。**

相机驱动下载地址：[相机驱动](https://www.daheng-imaging.com/)

OpenCV下载地址：[OpenCV](https://github.com/opencv)

OpenCV安装教程 : [linux](https://docs.opencv.org/3.4.11/d7/d9f/tutorial_linux_install.html)  [Windows](https://docs.opencv.org/3.4.11/d3/d52/tutorial_windows_install.html)

TensorRT : [TensorRT](https://developer.nvidia.com/zh-cn/tensorrt)

GLOG : [GLOG](https://github.com/google/glog)

## 二、程序编译运行以及调试方式

### 1.编译运行

* Ubuntu20/18（在项目文件夹下）

```shell
mkdir build
cd build
cmake ..
make -j6
sudo ./rmcv
```

### 3.工作条件

* 对于自瞄，由于使用了数字识别，务必保证光照充足，图像上数字清晰可见。光照不足时，调整摄像头曝光或增益数值，直到数字清晰可见。
* 务必保证摄像头焦距正确，同时镜片干净无污物。

## 三、文件目录结构
```
.
├── armor                       // 存放自瞄主要算法代码
│   ├── include                 // 自瞄头文件
│   └── src                     // 自瞄源码
├── CMakeLists.txt              // cmake工程文件
├── energy                      // 存放能量机关主要算法代码
│   ├── include                 // 能量机关头文件
│   └── src                     // 能量机关源码
├── main.cpp                    // 主函数
├── net                         // 存放目标识别网络代码
│   ├── include                 // 网络头文件
│   └── src                     // 网络源码(包含CUDA代码)
├── config                      // 存放程序"配置"代码
│   ├── include                 // "配置"和运行时头文件
│   └── src                     // "配置"源码
├── com                         // 存放程序与下位机或另一程序通信代码
│   ├── include                 // 通信代码头文件
│   └── src                     // 通信代码源码
├── others                      // 存放摄像头、Json、串口等第三方库
│   ├── include                 // others头文件
│   └── src                     // others源码
├── cam_calibrate               // 相机标定、内参计算相关相关
│   ├── cam_pre.py              // 标定内参计算程序
│   └── cam_run.py                     // 去除畸变效果展示程序
```
## 四、关键类解析

| 类名            | 主要成员                                                     | 主要接口               | 类的作用                                                |
| --------------- | ------------------------------------------------------------ | ---------------------- | ------------------------------------------------------- |
| ArmorFinder     | 过多，不做赘述                                               | void run(cv::Mat &src) | 将一帧图像中装甲板的detection以及数据发送封装为一个类   |
| Energy          | 过多，不做赘述                                               | void run(cv::Mat &src) | 将一帧图像中能量机关的detection以及数据发送封装为一个类 |
| EnergyPartParam | 过多，不做赘述                                               | 无                     | 能量机关所有参数的集合                                  |
| LightBlob       | 灯条位置<br />灯条颜色                                   | 无                     | 灯条类定义                                              |
| ArmorBox        | 装甲板位置<br />装甲板的两个灯条<br />装甲板颜色<br />装甲板数字id | 无                     | 装甲板类定义                                            |
| ArmorInfo | 目标识别网络输出的装甲板信息，只有类别和坐标信息 | 无 |装甲板信息定义
## 五、程序运行基本流程

　　　　　　　　　　　　↗  大能量机关 ↘

各项初始化→读取当前状态  → 小能量机关  → 回到读取状态

　　↓　　　　　　　　　↘　　自瞄　　↗

数据接收线程

## 六、识别方式

### 1.自瞄装甲板识别方式

​    首先使用多目标识别网络得到装甲板的类别和坐标，然后根据优先级选择要击打的装甲板，再对选定装甲板区域图像进行通道拆分以及二值化操作，再进行开闭运算，通过边缘提取和条件限制得出可能为灯条的部分。通过灯条再图像上的位置进行PNP解算或公式计算得到装甲板距离，计算弹道补偿后发送给云台主控板。
​
### 2.能量机关识别方式

​    首先对图像进行颜色提取（HSV空间），然后进行一定腐蚀和膨胀，通过边缘提取和条件限制得出待击打叶片（锤子形）。在待击打叶片范围内进一步用类似方法寻找目标装甲板和流动条，在二者连线上寻找中心的“R”。根据目标装甲板坐标和中心坐标计算极坐标系下的目标角度，进而预测待击打点的坐标（小符为装甲板本身，大符需要旋转）。最后将待击打点坐标和图像中心的差值转换为yaw和pitch轴角度，增加一环PID后发送给云台主控板。

## 七、通信协议

### 1.通信方式

使用USB转TTL进行串口通信

### 2.数据接收结构体

```c++
struct McuData {
    float curr_yaw;      // 当前云台yaw角度
    float curr_pitch;    // 当前云台pitch角
    uint8_t state;       // 当前状态，自瞄-大符-小符
    uint8_t mark;        // 云台角度标记位
    uint8_t anti_top;    // 是否为反陀螺模式
    uint8_t enemy_color; // 敌方颜色
    int delta_x;         // 能量机关x轴补偿量
    int delta_y;         // 能量机关y轴补偿量
};
```

每个数据帧后使用字符```'\n'```作为帧尾标志

### 3.数据发送结构体

```c++
struct SendData {
    char start_flag;      // 帧头标志，字符's'
    int16_t yaw;          // float类型的偏移角度(以度为单位)/100*(32768-1)
    int16_t pitch;        // float类型的偏移角度(以度为单位)/100*(32768-1)
    uint16_t shoot_delay; // 反陀螺模式下的发射延迟
    char end_flag;        // 帧尾标识，字符'e'
};
```

实际发送代码中没有使用这个结构体，而是使用uint8_t类型数组直接赋值代替

## 八、代码命名规范

函数名：使用首字母小写的驼峰命名法

类型名：使用首字母大写的驼峰命名法

变量名：使用下划线分割命名法

## 九、未来的优化方向
* 由于陀螺这样的机械设计基本上是强队的标准配置，所以我们也首次尝试了从视觉层面对陀螺有一个专门的击打方式，但实际应用场景较少(当前仅针对原地旋转的陀螺)，且对操作手不太友好(要求手动对准陀螺中心)，所以没有取得很大的实战价值。**所以第三个优化方向便是一个有着自动击打动态陀螺的系统**。

---
**觉得对你有帮助请点个STAR哦:)**
