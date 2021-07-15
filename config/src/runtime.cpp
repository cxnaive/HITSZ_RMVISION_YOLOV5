#include <runtime.h>
#include <map>
#include <cmath>

double getPointLength(const cv::Point2f &p) {
    return sqrt(p.x * p.x + p.y * p.y);
}
cudaVideoWriter initVideoWriter(const std::string &filename_prefix) {
    std::string file_name = filename_prefix + ".avi";
    cudaVideoWriter video = cv::cudacodec::createVideoWriter(file_name,cv::Size(640,640),60);
    return video;
}
std::map<std::string,cudaVideoWriter> video_writers;
cudaVideoWriter getVideoWriter(std::string prefix){
    auto iter = video_writers.find(prefix);
    if(iter != video_writers.end()) return iter->second;
    cudaVideoWriter now = initVideoWriter("video/"+prefix);
    video_writers[prefix] = now;
    return now;
}
void saveVideos(cv::Mat& img,std::string prefix){
    if(img.empty()) return;
    cudaVideoWriter writer = getVideoWriter(prefix);
    writer->write(img);
}