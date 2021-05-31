#include <camera/hk_cam_wrapper.h>
#include <glog/logging.h>

#include <iomanip>
HKCamera::HKCamera(std::string sn) {
    p_img = cv::Mat(640, 640, CV_8UC3);
    p_energy = cv::Mat(1024, 1024, CV_8UC3);
    exposure = 4000;
    gain = 8;
}

HKCamera::~HKCamera() {
    if (!m_handle) return;
    nRet = MV_CC_CloseDevice(m_handle);
    if (nRet != MV_OK) {
        printf("error: CloseDevice fail [%x]\n", nRet);
        return;
    }
    //销毁句柄，释放资源
    nRet = MV_CC_DestroyHandle(m_handle);
    if (nRet != MV_OK) {
        printf("error: DestroyHandle fail [%x]\n", nRet);
        return;
    }
}

void update_bool(int code, bool& flag) {
    if (code != MV_OK) flag = true;
}
#define UPDB(x) (update_bool(x, set_failed))

bool HKCamera::init(int roi_x, int roi_y, int roi_w, int roi_h, bool isEnergy) {
    unsigned int nTLayerType = MV_GIGE_DEVICE | MV_USB_DEVICE;
    MV_CC_DEVICE_INFO_LIST m_stDevList = {0};
    nRet = MV_CC_EnumDevices(nTLayerType, &m_stDevList);
    if (nRet != MV_OK) {
        LOG(ERROR) << "unable to enum camera devices! Error code:" << nRet;
        return false;
    }
    if (m_stDevList.nDeviceNum == 0) {
        LOG(ERROR) << "no camera found!";
        return false;
    }
    //按SN选择设备
    MV_CC_DEVICE_INFO* target_device = NULL;
    for (int i = 0; i < m_stDevList.nDeviceNum; ++i) {
        MV_CC_DEVICE_INFO* now = m_stDevList.pDeviceInfo[i];
        std::string serial_number;
        if (now->nTLayerType == MV_USB_DEVICE) {
            unsigned char* serial_number_char =
                now->SpecialInfo.stUsb3VInfo.chSerialNumber;
            serial_number = reinterpret_cast<char*>(serial_number_char);
            LOG(INFO) << "Camera #" << i << ":"
                      << "usb "
                      << "SN:" << serial_number;
            if (serial_number == sn) {
                target_device = now;
            }
        }
    }
    if (target_device == NULL) {
        LOG(ERROR) << "SN number not found!";
        return false;
    }
    //创建句柄
    memcpy(&m_stDevInfo, target_device, sizeof(MV_CC_DEVICE_INFO));
    nRet = MV_CC_CreateHandle(&m_handle, &m_stDevInfo);
    if (nRet != MV_OK) {
        LOG(ERROR) << "failed to create handle! Error code:" << nRet;
        return false;
    }
    //打开设备
    unsigned int nAccessMode = MV_ACCESS_Exclusive;
    unsigned short nSwitchoverKey = 0;
    nRet = MV_CC_OpenDevice(m_handle, nAccessMode, nSwitchoverKey);
    if (nRet != MV_OK) {
        LOG(ERROR) << "failed to open device! Error code:" << nRet;
        return false;
    }

    MV_CC_GetIntValue(m_handle, "Width", &m_SensorWidth);
    MV_CC_GetIntValue(m_handle, "Height", &m_SensorHeight);
    LOG(INFO) << "HKCamera Sensor: " << m_SensorWidth.nMax << " X "
              << m_SensorHeight.nMax << " nInc:" << m_SensorWidth.nInc;

    bool set_failed = false;
    UPDB(MV_CC_SetIntValue(m_handle, "Width", roi_w));
    UPDB(MV_CC_SetIntValue(m_handle, "Height", roi_h));
    UPDB(MV_CC_SetIntValue(m_handle, "OffsetX", roi_x));
    UPDB(MV_CC_SetIntValue(m_handle, "OffsetY", roi_y));
    UPDB(MV_CC_SetEnumValue(m_handle, "TriggerMode", MV_TRIGGER_MODE_OFF));
    UPDB(MV_CC_SetEnumValue(m_handle, "ExposureMode",
                            MV_EXPOSURE_AUTO_MODE_OFF));
    UPDB(MV_CC_SetEnumValue(m_handle, "GainAuto", MV_GAIN_MODE_OFF));
    UPDB(MV_CC_SetBoolValue(m_handle, "BlackLevelEnable", false));
    UPDB(MV_CC_SetEnumValue(m_handle, "BalanceWhiteAuto",
                            MV_BALANCEWHITE_AUTO_CONTINUOUS));
    UPDB(MV_CC_SetEnumValue(m_handle, "AcquisitionMode",
                            MV_ACQ_MODE_CONTINUOUS));

    //获取实际增益值范围
    MV_CC_GetFloatValue(m_handle, "Gain", &m_GainRange);
    LOG(INFO) << "HKCamera Gain Range: " << m_GainRange.fMin << "~"
              << m_GainRange.fMax;

    //获取像素格式
    MV_CC_GetEnumValue(m_handle, "PixelFormat", &m_PixelFormat);
    LOG(INFO) << "HKCamera Pixel Format: " << std::hex
              << m_PixelFormat.nCurValue;

    UPDB(MV_CC_SetFloatValue(m_handle, "ExposureTime", exposure));
    UPDB(MV_CC_SetFloatValue(m_handle, "Gain", gain));
    UPDB(MV_CC_SetFloatValue(m_handle, "BlackLevel", 0));

    if (set_failed) {
        LOG(ERROR) << "failed to set some parameters!";
        return false;
    }

    init_success = true;
    thread_running = false;
    is_energy = isEnergy;
    return true;
}

void getRGBImage(HKCamera* cam) {
    memset(&cam->stOutFrame, 0, sizeof(MV_FRAME_OUT));
    MV_CC_PIXEL_CONVERT_PARAM stConvertParam = {0};
    MV_FRAME_OUT_INFO_EX stImageInfo;
    while (cam->thread_running) {
        cam->nRet = MV_CC_GetImageBuffer(cam->m_handle, &cam->stOutFrame, 1000);
        if (cam->nRet == MV_OK) {
            auto start = std::chrono::steady_clock::now();
            printf("Get One Frame: Width[%d], Height[%d], nFrameNum[%d]\n",
                   cam->stOutFrame.stFrameInfo.nWidth,
                   cam->stOutFrame.stFrameInfo.nHeight,
                   cam->stOutFrame.stFrameInfo.nFrameNum);
            //像素格式转换
            stImageInfo = cam->stOutFrame.stFrameInfo;
            stConvertParam.nWidth = stImageInfo.nWidth;
            stConvertParam.nHeight = stImageInfo.nHeight;
            stConvertParam.pSrcData = cam->stOutFrame.pBufAddr;
            stConvertParam.nSrcDataLen = stImageInfo.nFrameLen;
            stConvertParam.enSrcPixelType = stImageInfo.enPixelType;
            stConvertParam.enDstPixelType = PixelType_Gvsp_BGR8_Packed;
            if (cam->is_energy)
                stConvertParam.pDstBuffer = cam->p_energy.data;
            else
                stConvertParam.pDstBuffer = cam->p_img.data;
            stConvertParam.nDstBufferSize =
                stImageInfo.nWidth * stImageInfo.nHeight * 3;
            //1024*1024
            if (cam->is_energy) {
                cam->nRet =
                    MV_CC_ConvertPixelType(cam->m_handle, &stConvertParam);
                if (cam->nRet != MV_OK) {
                    LOG(ERROR) << "Error converting pixel format!";
                }
                cam->pimg_lock.lock();
                cv::resize(cam->p_energy, cam->p_img, cv::Size(640, 640),
                           cv::INTER_NEAREST);
                cam->pimg_lock.unlock();
            } else {
                //640*640
                cam->pimg_lock.lock();
                cam->nRet =
                    MV_CC_ConvertPixelType(cam->m_handle, &stConvertParam);
                cam->pimg_lock.unlock();
                if (cam->nRet != MV_OK) {
                    LOG(ERROR) << "Error converting pixel format!";
                }
            }
            auto end = std::chrono::steady_clock::now();
            cam->nRet = MV_CC_FreeImageBuffer(cam->m_handle, &cam->stOutFrame);
            if (cam->nRet != MV_OK) {
                LOG(ERROR) << "Free Image Buffer fail! nRet: " << cam->nRet;
            }
            cam->frame_cnt++;
            cam->frame_get_time +=
                std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                      start)
                    .count();
            if (cam->frame_cnt == 500) {
                LOG(INFO) << "average hkcamera delay(ms):"
                          << cam->frame_get_time / cam->frame_cnt;
                cam->frame_get_time = cam->frame_cnt = 0;
            }

        } else {
            LOG(ERROR) << "Error hkcamera no data!";
        }
    }
}

void HKCamera::setParam(int exposureInput, int gainInput) {
    if (init_success) {
        exposure = exposureInput;
        gain = gainInput;
        MV_CC_SetFloatValue(m_handle, "ExposureTime", exposure);
        MV_CC_SetFloatValue(m_handle, "Gain", gain);
    }
}

void HKCamera::start() {
    if (init_success) {
        nRet = MV_CC_StartGrabbing(m_handle);
        if (nRet != MV_OK) {
            LOG(ERROR) << "failed to start grabbing image!";
            return;
        }
        thread_running = true;
        hkcam_thread = std::thread(getRGBImage, this);
    }
}
void HKCamera::stop() {
    if (init_success) {
        thread_running = false;
        hkcam_thread.join();
        nRet = MV_CC_StartGrabbing(m_handle);
        if (nRet != MV_OK) {
            LOG(ERROR) << "failed to stop grabbing image!";
            return;
        }
    }
}
void HKCamera::calcRoi() {}
bool HKCamera::init_is_successful() { return init_success; }
bool HKCamera::read(cv::Mat& src) {
    pimg_lock.lock();
    // p_img.copyTo(src);
    cv::swap(p_img, src);
    pimg_lock.unlock();
    return true;
}