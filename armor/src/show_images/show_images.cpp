#include <armor_finder/armor_finder.h>
#include <runtime.h>
#include <show_images/show_images.h>

#include <opencv2/highgui.hpp>

using namespace cv;

void drawLightBlobs(cv::Mat &src, const LightBlobs &blobs) {
    for (const auto &blob : blobs) {
        Scalar color(0, 255, 0);
        if (blob.blob_color == BLOB_RED)
            color = Scalar(0, 0, 255);
        else if (blob.blob_color == BLOB_BLUE)
            color = Scalar(255, 0, 0);
        cv::Point2f vertices[4];
        blob.rect.points(vertices);
        for (int j = 0; j < 4; j++) {
            cv::line(src, vertices[j], vertices[(j + 1) % 4], color, 2);
        }
    }
}

void drawPoints4(cv::Mat &src, const std::vector<Point2f> &points,
                 Scalar color = Scalar(128, 0, 128)) {
    for (int i = 0; i < 4; ++i) {
        cv::line(src, points[i], points[(i + 1) % 4], color, 1);
    }
}

/**************************
 *     显示多个装甲板区域（YOLOV5 NET）    *
 **************************/
void showNetBoxes(std::string windows_name, const cv::Mat &src,
                  const std::vector<ArmorInfo> &armors) {
    static Mat image2show;
    if (src.type() == CV_8UC1) {  // 黑白图像
        cvtColor(src, image2show, COLOR_GRAY2RGB);
    } else if (src.type() == CV_8UC3) {  // RGB 彩色
        image2show = src.clone();
    }
    for (auto &box : armors) {
        if (name2color[id2name[box.id]] == BOX_BLUE) {
            rectangle(image2show, box.bbox, Scalar(0, 255, 0), 1);
            putText(image2show, id2name[box.id],
                    Point(box.bbox.x + 2, box.bbox.y + 2),
                    cv::FONT_HERSHEY_TRIPLEX, 1, Scalar(255, 0, 0));
        } else if (name2color[id2name[box.id]] == BOX_RED) {
            rectangle(image2show, box.bbox, Scalar(0, 255, 0), 1);
            putText(image2show, id2name[box.id],
                    Point(box.bbox.x + 2, box.bbox.y + 2),
                    cv::FONT_HERSHEY_TRIPLEX, 1, Scalar(0, 0, 255));
        } else {
            LOG(INFO) << "Invalid box id:" << box.id << "!";
        }
    }
    imshow(windows_name, image2show);
}

/**************************
 * 显示单个装甲板区域及其类别 *
 **************************/
void showArmorBox(std::string windows_name, const cv::Mat &src,
                  const ArmorBox &box) {
    static Mat image2show;
    if (box.rect == cv::Rect2d()) {
        imshow(windows_name, src);
    }
    if (src.type() == CV_8UC1) {  // 黑白图像
        cvtColor(src, image2show, COLOR_GRAY2RGB);
    } else if (src.type() == CV_8UC3) {  // RGB 彩色
        image2show = src.clone();
    }
    drawLightBlobs(image2show, box.light_blobs);

    rectangle(image2show, box.rect, Scalar(0, 255, 0), 1);
    drawPoints4(image2show, box.getArmorPoints());
    char dist[10];
    sprintf(dist, "%.1f", box.getBoxDistance());
    if (name2color[id2name[box.id]] == BOX_BLUE) {
        rectangle(image2show, box.rect, Scalar(0, 255, 0), 1);
        putText(image2show, id2name[box.id] + " " + dist,
                Point(box.rect.x + 2, box.rect.y + 2), cv::FONT_HERSHEY_TRIPLEX,
                1, Scalar(255, 0, 0));
    } else if (name2color[id2name[box.id]] == BOX_RED) {
        rectangle(image2show, box.rect, Scalar(0, 255, 0), 1);
        putText(image2show, id2name[box.id] + " " + dist,
                Point(box.rect.x + 2, box.rect.y + 2), cv::FONT_HERSHEY_TRIPLEX,
                1, Scalar(0, 0, 255));
    } else {
        LOG(INFO) << "Invalid box id:" << box.id << "!";
    }
    imshow(windows_name, image2show);
    if (config.save_video) {
        //saveVideos(image2show, "ArmorBox");
    }
}

/**************************
 *      显示多个灯条区域     *
 **************************/
void showLightBlobs(std::string windows_name, const cv::Mat &src,
                    const LightBlobs &light_blobs) {
    static Mat image2show;

    if (src.type() == CV_8UC1) {  // 黑白图像
        cvtColor(src, image2show, COLOR_GRAY2RGB);
    } else if (src.type() == CV_8UC3) {  // RGB 彩色
        image2show = src.clone();
    }

    for (const auto &light_blob : light_blobs) {
        Scalar color(0, 255, 0);
        if (light_blob.blob_color == BLOB_RED)
            color = Scalar(0, 0, 255);
        else if (light_blob.blob_color == BLOB_BLUE)
            color = Scalar(255, 0, 0);
        cv::Point2f vertices[4];
        light_blob.rect.points(vertices);
        for (int j = 0; j < 4; j++) {
            cv::line(image2show, vertices[j], vertices[(j + 1) % 4], color, 2);
        }
    }
    imshow(windows_name, image2show);
}

void showTrackSearchingPos(std::string window_names, const cv::Mat &src,
                           const cv::Rect2d pos) {
    static Mat image2show;
    if (src.type() == CV_8UC1) {  // 黑白图像
        cvtColor(src, image2show, COLOR_GRAY2RGB);
    } else if (src.type() == CV_8UC3) {  // RGB 彩色
        image2show = src.clone();
    }
    rectangle(image2show, pos, Scalar(0, 255, 0), 1);
    imshow(window_names, image2show);
}