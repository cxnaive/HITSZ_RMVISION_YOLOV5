//
// Created by erc on 1/3/20.
//

#include <camera/cam_wrapper.h>
#include <glog/logging.h>
#include <chrono>
#include <mutex>
#include <thread>


std::mutex mtx;

void ProcessData(void *pImageBuf, void *pImageRaw8Buf, void *pImageRGBBuf,
                 int nImageWidth, int nImageHeight, int nPixelFormat,
                 int nPixelColorFilter) {
    switch (nPixelFormat) {
        //当数据格式为12位时，位数转换为4-11
        case GX_PIXEL_FORMAT_BAYER_GR12:
        case GX_PIXEL_FORMAT_BAYER_RG12:
        case GX_PIXEL_FORMAT_BAYER_GB12:
        case GX_PIXEL_FORMAT_BAYER_BG12:
            //将12位格式的图像转换为8位格式
            DxRaw16toRaw8(pImageBuf, pImageRaw8Buf, nImageWidth, nImageHeight,
                          DX_BIT_4_11);
            //将Raw8图像转换为RGB图像以供显示
            DxRaw8toRGB24(pImageRaw8Buf, pImageRGBBuf, nImageWidth,
                          nImageHeight, RAW2RGB_NEIGHBOUR,
                          DX_PIXEL_COLOR_FILTER(nPixelColorFilter), false);
            break;

            //当数据格式为10位时，位数转换为2-9
        case GX_PIXEL_FORMAT_BAYER_GR10:
        case GX_PIXEL_FORMAT_BAYER_RG10:
        case GX_PIXEL_FORMAT_BAYER_GB10:
        case GX_PIXEL_FORMAT_BAYER_BG10:
            ////将10位格式的图像转换为8位格式,有效位数2-9
            DxRaw16toRaw8(pImageBuf, pImageRaw8Buf, nImageWidth, nImageHeight,
                          DX_BIT_2_9);
            //将Raw8图像转换为RGB图像以供显示
            DxRaw8toRGB24(pImageRaw8Buf, pImageRGBBuf, nImageWidth,
                          nImageHeight, RAW2RGB_NEIGHBOUR,
                          DX_PIXEL_COLOR_FILTER(nPixelColorFilter), false);
            break;

        case GX_PIXEL_FORMAT_BAYER_GR8:
        case GX_PIXEL_FORMAT_BAYER_RG8:
        case GX_PIXEL_FORMAT_BAYER_GB8:
        case GX_PIXEL_FORMAT_BAYER_BG8:
            //将Raw8图像转换为RGB图像以供显示

            DxRaw8toRGB24(pImageBuf, pImageRGBBuf, nImageWidth, nImageHeight,
                          RAW2RGB_NEIGHBOUR,
                          DX_PIXEL_COLOR_FILTER(nPixelColorFilter),
                          false);  // RAW2RGB_ADAPTIVE
            break;

        case GX_PIXEL_FORMAT_MONO12:
            //将12位格式的图像转换为8位格式
            DxRaw16toRaw8(pImageBuf, pImageRaw8Buf, nImageWidth, nImageHeight,
                          DX_BIT_4_11);
            //将Raw8图像转换为RGB图像以供显示
            DxRaw8toRGB24(pImageRaw8Buf, pImageRGBBuf, nImageWidth,
                          nImageHeight, RAW2RGB_NEIGHBOUR,
                          DX_PIXEL_COLOR_FILTER(NONE), false);
            break;

        case GX_PIXEL_FORMAT_MONO10:
            //将10位格式的图像转换为8位格式
            DxRaw16toRaw8(pImageBuf, pImageRaw8Buf, nImageWidth, nImageHeight,
                          DX_BIT_4_11);
            //将Raw8图像转换为RGB图像以供显示
            DxRaw8toRGB24(pImageRaw8Buf, pImageRGBBuf, nImageWidth,
                          nImageHeight, RAW2RGB_NEIGHBOUR,
                          DX_PIXEL_COLOR_FILTER(NONE), false);
            break;

        case GX_PIXEL_FORMAT_MONO8:
            //将Raw8图像转换为RGB图像以供显示
            DxRaw8toRGB24(pImageBuf, pImageRGBBuf, nImageWidth, nImageHeight,
                          RAW2RGB_NEIGHBOUR, DX_PIXEL_COLOR_FILTER(NONE),
                          false);
            break;

        default:
            break;
    }
}

void GX_STDC OnFrameCallbackFun(GX_FRAME_CALLBACK_PARAM *pFrame) {
    if (pFrame->status == GX_FRAME_STATUS_SUCCESS) {
        auto start = std::chrono::steady_clock::now();
        Camera *cam = (Camera *)pFrame->pUserParam;
        ProcessData((void *)pFrame->pImgBuf, cam->g_pRaw8Buffer,
                    cam->g_pRGBframeData, pFrame->nWidth, pFrame->nHeight,
                    pFrame->nPixelFormat, cam->g_nColorFilter);

        
        // cam->full_gpu.upload(cam->full);
        
        // cv::cuda::resize(cam->full_gpu,cam->resize_gpu,cv::Size(640,640));
        // cv::cuda::cvtColor(cam->resize_gpu,cam->resize_gpu,cv::COLOR_RGB2BGR);
        mtx.lock();
        // cv::resize(cam->full,cam->p_img,cv::Size(640,640),cv::INTER_NEAREST);
        // cv::cvtColor(cam->p_img,cam->p_img,cv::COLOR_RGB2BGR);
        // cam->resize_gpu.download(cam->p_img);
        memcpy(cam->p_img.data, cam->g_pRGBframeData, 3 * (cam->nPayLoadSize));
        cv::cvtColor(cam->p_img,cam->p_img,cv::COLOR_RGB2BGR);
        mtx.unlock();
        auto end = std::chrono::steady_clock::now();
        cam->frame_cnt ++;
        cam->frame_get_time += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        if(cam->frame_cnt == 500){
            LOG(INFO) << "average camera delay(ms):" << cam->frame_get_time / cam->frame_cnt;
            cam->frame_get_time = cam->frame_cnt = 0;
        }
    }
    return;
}

void getRGBImage(Camera *p_cam) {
    while (1) {
        GX_STATUS status;
        status = GXGetImage(p_cam->g_hDevice, &p_cam->g_frameData, 100);
        if (!p_cam->thread_running) {
            return;
        }
        ProcessData(p_cam->g_frameData.pImgBuf, p_cam->g_pRaw8Buffer,
                    p_cam->g_pRGBframeData, p_cam->g_frameData.nWidth,
                    p_cam->g_frameData.nHeight, p_cam->g_nPixelFormat,
                    p_cam->g_nColorFilter);

        cv::Mat temp(p_cam->camConfig.roi_height, p_cam->camConfig.roi_width,
                     CV_8UC3);

        memcpy(temp.data, p_cam->g_pRGBframeData, 3 * (p_cam->nPayLoadSize));

        cv::resize(temp, temp, cv::Size(640, 640));

        mtx.lock();
        cv::cvtColor(temp,p_cam->p_img,cv::COLOR_RGB2BGR);
        mtx.unlock();
    }
}

Camera::Camera(std::string sn, CameraConfig config)
    : sn(sn),
      exposure(5000),
      gain(0),
      thread_running(false),
      camConfig(config),
      frame_cnt(0),
      frame_get_time(0),
      init_success(false) {
    p_img = cv::Mat(640, 640, CV_8UC3);
    // full = cv::Mat(camConfig.roi_height,camConfig.roi_width,CV_8UC3);
    // full_gpu = cv::cuda::GpuMat(camConfig.roi_height,camConfig.roi_width,CV_8UC3);
    // resize_gpu = cv::cuda::GpuMat(640,640,CV_8UC3);
};

Camera::~Camera() {
    if (init_success) {
        stop();
        if (g_frameData.pImgBuf != NULL) {
            free(g_frameData.pImgBuf);
        }
        if (g_pRGBframeData != NULL) {
            free(g_pRGBframeData);
        }
        GXCloseDevice(g_hDevice);
    }

    GXCloseLib();
}
std::string gc_device_typename[5] = {
    "GX_DEVICE_CLASS_UNKNOWN", "GX_DEVICE_CLASS_USB2", "GX_DEVICE_CLASS_GEV",
    "GX_DEVICE_CLASS_U3V", "GX_DEVICE_CLASS_SMART"};
bool Camera::init() {
    GXInitLib();
    GXUpdateDeviceList(&nDeviceNum, 1000);
    if (nDeviceNum >= 1) {
        GX_DEVICE_BASE_INFO pBaseinfo[nDeviceNum];
        size_t nSize = nDeviceNum * sizeof(GX_DEVICE_BASE_INFO);
        status = GXGetAllDeviceBaseInfo(pBaseinfo, &nSize);
        bool found_device = false;
        for (int i = 0; i < nDeviceNum; ++i) {
            LOG(INFO) << "device: SN:" << pBaseinfo[i].szSN
                      << " NAME:" << pBaseinfo[i].szDisplayName << " TYPE:"
                      << gc_device_typename[pBaseinfo[i].deviceClass];
            if (std::string(pBaseinfo[i].szSN) == sn) found_device = true;
        }
        if (!found_device) {
            LOG(ERROR) << "No device found with SN:" << sn;
            return false;
        }
        GX_OPEN_PARAM stOpenParam;
        stOpenParam.accessMode = GX_ACCESS_EXCLUSIVE;
        stOpenParam.openMode = GX_OPEN_SN;
        stOpenParam.pszContent = const_cast<char *>(sn.c_str());
        status = GXOpenDevice(&stOpenParam, &g_hDevice);

        GXGetInt(g_hDevice, GX_INT_SENSOR_WIDTH, &g_SensorWidth);
        GXGetInt(g_hDevice, GX_INT_SENSOR_HEIGHT, &g_SensorHeight);
        LOG(WARNING) << "Camera Sensor: " << g_SensorWidth << " X "
                     << g_SensorHeight;

        GXSetEnum(g_hDevice, GX_ENUM_EXPOSURE_AUTO, GX_EXPOSURE_AUTO_OFF);
        GXSetEnum(g_hDevice, GX_ENUM_GAIN_AUTO, GX_GAIN_AUTO_OFF);
        GXSetEnum(g_hDevice, GX_ENUM_BLACKLEVEL_AUTO, GX_BLACKLEVEL_AUTO_OFF);
        GXSetEnum(g_hDevice, GX_ENUM_BALANCE_WHITE_AUTO,
                  GX_BALANCE_WHITE_AUTO_CONTINUOUS);
        GXSetEnum(g_hDevice, GX_ENUM_DEAD_PIXEL_CORRECT,
                  GX_DEAD_PIXEL_CORRECT_OFF);

        GXSetEnum(g_hDevice, GX_ENUM_ACQUISITION_MODE, GX_ACQ_MODE_CONTINUOUS);
        GXSetInt(g_hDevice, GX_INT_ACQUISITION_SPEED_LEVEL, 4);

        GXSetInt(g_hDevice, GX_INT_OFFSET_X, camConfig.roi_offset_x);
        GXSetInt(g_hDevice, GX_INT_OFFSET_Y, camConfig.roi_offset_y);
        GXSetInt(g_hDevice, GX_INT_WIDTH, camConfig.roi_width);
        GXSetInt(g_hDevice, GX_INT_HEIGHT, camConfig.roi_height);

        GXSetFloat(g_hDevice, GX_FLOAT_EXPOSURE_TIME, exposure);
        GXSetFloat(g_hDevice, GX_FLOAT_GAIN, gain);
        GXSetFloat(g_hDevice, GX_FLOAT_BLACKLEVEL, 0);
        //设置增益模式
        GXSetEnum(g_hDevice, GX_ENUM_GAIN_SELECTOR, GX_GAIN_SELECTOR_ALL);
        //获取实际增益范围
        GX_FLOAT_RANGE gainRange;
        GXGetFloatRange(g_hDevice, GX_FLOAT_GAIN, &gainRange);
        LOG(WARNING) << "Camera Gain Range: " << gainRange.dMin << "~"
                     << gainRange.dMax << " step size:" << gainRange.dInc;
        GXGetInt(g_hDevice, GX_INT_PAYLOAD_SIZE, &nPayLoadSize);

        g_frameData.pImgBuf = malloc(nPayLoadSize);
        g_pRGBframeData = malloc(nPayLoadSize * 3);

        GXGetEnum(g_hDevice, GX_ENUM_PIXEL_FORMAT, &g_nPixelFormat);
        GXGetEnum(g_hDevice, GX_ENUM_PIXEL_COLOR_FILTER, &g_nColorFilter);

        init_success = true;
        return true;
    } else {
        return false;
    }
}

void Camera::setParam(int exposureInput, int gainInput) {
    if (init_success) {
        exposure = exposureInput;
        gain = gainInput;
        stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        GXSetFloat(g_hDevice, GX_FLOAT_EXPOSURE_TIME, exposure);
        GXSetFloat(g_hDevice, GX_FLOAT_GAIN, gain);
        start();
    }
}
void Camera::start() {
    if (init_success) {
        GXRegisterCaptureCallback(g_hDevice, this, OnFrameCallbackFun);
        GXSendCommand(g_hDevice, GX_COMMAND_ACQUISITION_START);
        // thread_running = true;
        // std::thread task(getRGBImage, this);
        // task.detach();
    }
}

void Camera::stop() {
    if (init_success) {
        // thread_running = false;
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        GXSendCommand(g_hDevice, GX_COMMAND_ACQUISITION_STOP);
        GXUnregisterCaptureCallback(g_hDevice);
    }
}

bool Camera::init_is_successful() { return init_success; }

bool Camera::read(cv::Mat &src) {
    mtx.lock();
    // p_img.copyTo(src);
    cv::swap(p_img, src);
    mtx.unlock();
    return true;
}