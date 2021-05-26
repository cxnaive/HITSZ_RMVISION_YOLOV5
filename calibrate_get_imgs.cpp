#include<camera/cam_wrapper.h>
#include<glog/logging.h>
#include<opencv2/opencv.hpp>
#include<string>
#include <rmconfig.h>

int img_cnt = 40;

int main(int argc, char** argv){
    FLAGS_alsologtostderr = true;
    FLAGS_colorlogtostderr = true;
    google::InitGoogleLogging(argv[0]);
    RmConfig config;
    config.init_from_file();
    Camera cam(config.camera_sn);
    cam.init(config.camConfig.roi_offset_x,config.camConfig.roi_offset_y,config.camConfig.roi_width,config.camConfig.roi_height,false);
    if(!cam.init_is_successful()){
        LOG(ERROR) << "unable to open camera";
        return 0;
    }
    cam.setParam(4000,10);
    cam.start();
    cv::Mat src;
    int cnt = 0;
    while(true){
        cam.read(src);
        cv::imshow("origin",src);
        int key = cv::waitKey(20);
        if(key == 's'){
            cv::imwrite("../cam_calibrate/" + std::to_string(cnt++)+".jpg",src);
            if(cnt == img_cnt){
                break;
            }
        }
    }
    return 0;
}