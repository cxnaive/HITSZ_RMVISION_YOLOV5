//
// Created by xixiliadorabarry on 1/24/19.
//

#include <energy.h>

using namespace cv;
using std::cout;
using std::endl;
using std::vector;

//----------------------------------------------------------------------------------------------------------------------
// 此函数在流动条区域内寻找装甲板
// ---------------------------------------------------------------------------------------------------------------------
bool Energy::findTargetInFlowStripFan() {
    // cout<<"i come here"<<endl;
    Mat draw(480, 640, CV_8UC3, Scalar(0, 0, 0));
    for (auto &candidate_flow_strip_fan : flow_strip_fans /*可能的流动扇叶*/) {
        Point2f vertices[4];                        //定义矩形的4个顶点
        candidate_flow_strip_fan.points(vertices);  //计算矩形的4个顶点
        for (int i = 0; i < 4; i++)
            line(draw, vertices[i], vertices[(i + 1) % 4], Scalar(0, 0, 255),
                 2);
        int i = 0;
        for (i = 0; i < armors.size(); ++i) {
            std::vector<cv::Point2f> intersection;
            if (rotatedRectangleIntersection(
                    armors.at(i), candidate_flow_strip_fan, intersection) == 0)
                continue;  //返回0表示没有重合面积
            double cur_contour_area = contourArea(intersection);
            cout << cur_contour_area << endl;
            if (cur_contour_area >
                energy_part_param_
                    .TARGET_INTERSETION_CONTOUR_AREA_MIN /*扇叶与装甲板匹配时的最小重合面积*/) {
                target_armors.emplace_back(armors.at(i));
            }
        }
    }
    imshow("draw", draw);  //这个draw很奇怪，没更新一次都要按一下建。
    waitKey(1);
    cout << "target armor cnt: " << target_armors.size() << endl;
    if (target_armors.empty()) {
        if (config.show_info) cout << "find target armor false" << endl;
        return false;
    } else {
        return true;
    }
}