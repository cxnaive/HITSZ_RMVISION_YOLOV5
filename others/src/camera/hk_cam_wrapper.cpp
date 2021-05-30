#include <camera/hk_cam_wrapper.h>
#include <glog/logging.h>
HKCamera::HKCamera(std::string sn){
    p_img = cv::Mat(640,640,CV_8UC3);
    p_energy = cv::Mat(1024,1024,CV_8UC3);
}

bool HKCamera::init(int roi_x,int roi_y,int roi_w,int roi_h,bool isEnergy){
    unsigned int nTLayerType = MV_GIGE_DEVICE | MV_USB_DEVICE;
    MV_CC_DEVICE_INFO_LIST m_stDevList = {0};
    int nRet = MV_CC_EnumDevices(nTLayerType,&m_stDevList);
    if(nRet != MV_OK){
        LOG(ERROR) << "unable to enum camera devices!";
        return false;
    }
    if(m_stDevList.nDeviceNum == 0){
        LOG(ERROR) << "no camera found!";
        return false; 
    }
    //按SN选择设备
    for(int i = 0;i < m_stDevList.nDeviceNum;++i){
        MV_CC_DEVICE_INFO* now = m_stDevList.pDeviceInfo[i];
        std::string serial_number;
        if(now->nTLayerType == MV_USB_DEVICE){
            unsigned char* serial_number_char = now->SpecialInfo.stUsb3VInfo.chSerialNumber;
            serial_number = reinterpret_cast<char*>(serial_number_char);
            LOG(INFO) << "Camera #" << i <<":" << "usb " << "SN:" << serial_number;
        }
    }

}

void HKCamera::setParam(int exposureInput, int gainInput){

}

void HKCamera::start(){

} 				                   
void HKCamera::stop(){

} 					                 
void HKCamera::calcRoi(){

}
bool HKCamera::init_is_successful(){
    return init_success;
}            
bool HKCamera::read(cv::Mat &src){

}