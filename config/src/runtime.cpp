#include <runtime.h>
#include <map>
#include <cmath>

double getPointLength(const cv::Point2f &p) {
    return sqrt(p.x * p.x + p.y * p.y);
}
cv::VideoWriter initVideoWriter(const std::string &filename_prefix) {
    std::string file_name = filename_prefix + ".avi";
    cv::VideoWriter video(file_name,cv::VideoWriter::fourcc('P','I','M','1'),60,cv::Size(640,640));
    return video;
}
std::map<std::string,cv::VideoWriter> video_writers;
cv::VideoWriter getVideoWriter(std::string prefix){
    auto iter = video_writers.find(prefix);
    if(iter != video_writers.end()) return iter->second;
    cv::VideoWriter now = initVideoWriter("video/"+prefix);
    video_writers[prefix] = now;
    return now;
}
void saveVideos(cv::Mat& img,std::string prefix){
    if(img.empty()) return;
    cv::VideoWriter writer = getVideoWriter(prefix);
    writer.write(img);
}