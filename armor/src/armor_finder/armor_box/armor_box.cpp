
#include <armor_finder/armor_finder.h>
#include <runtime.h>

ArmorBox::ArmorBox(const cv::Rect &pos, const LightBlobs &blobs, uint8_t color,
                   int i)
    : rect(pos), light_blobs(blobs), box_color(color), id(i){};
std::pair<cv::Point3d, cv::Point3d> ArmorBox::armorSolvePnP() const {
    static double x = config.ARMOR_W / 2.0;
    static double y = config.ARMOR_H / 2.0;
    //装甲板世界坐标，从右下开始逆时针 4 点。
    static std::vector<cv::Point3f> world_points = {
        cv::Point3f(-x, -y, 0), cv::Point3f(-x, y, 0), cv::Point3f(x, y, 0),
        cv::Point3f(x, -y, 0)};
    cv::Mat Rvec, Tvec, rotM, invrotM, P;
    // PnP解算
    cv::solvePnP(world_points, getArmorPoints(), config.camConfig.mtx,
                 config.camConfig.dist, Rvec, Tvec, false,
                 cv::SOLVEPNP_ITERATIVE);
    //旋转向量转换为旋转矩阵
    cv::Rodrigues(Rvec, rotM);
    //再转换为欧拉角
    double theta_x = atan2(rotM.at<double>(2, 1), rotM.at<double>(2, 2));
    double theta_y = atan2(-rotM.at<double>(2, 0),
                           sqrt(rotM.at<double>(2, 1) * rotM.at<double>(2, 1) +
                                rotM.at<double>(2, 2) * rotM.at<double>(2, 2)));
    double theta_z = atan2(rotM.at<double>(1, 0), rotM.at<double>(0, 0));
    //相机坐标系下装甲板的旋转以及中心点位置信息
    std::pair<cv::Point3d, cv::Point3d> ans;
    ans.first = {theta_x * (180 / PI), theta_y * (180 / PI),
                 theta_z * (180 / PI)};
    ans.second = {Tvec.at<double>(0), Tvec.at<double>(1), Tvec.at<double>(2)};

    if (config.show_pnp_axies) {
        cv::invert(rotM, invrotM);
        P = -invrotM * Tvec;
        std::vector<cv::Point3f> refer_world_points = {
            cv::Point3f(0, 0, 0), cv::Point3f(ans.second.z / 10, 0, 0),
            cv::Point3f(0, ans.second.z / 10, 0),
            cv::Point3f(0, 0, ans.second.z / 10)};
        std::vector<cv::Point2f> refer_image_points;
        cv::projectPoints(refer_world_points, Rvec, Tvec, config.camConfig.mtx,
                          config.camConfig.dist, refer_image_points);
        cv::Mat pnp = src.clone();
        cv::line(pnp, refer_image_points[0], refer_image_points[1],
                 cv::Scalar(0, 0, 255), 2);
        cv::line(pnp, refer_image_points[0], refer_image_points[2],
                 cv::Scalar(0, 255, 0), 2);
        cv::line(pnp, refer_image_points[0], refer_image_points[3],
                 cv::Scalar(255, 0, 0), 2);
        cv::imshow("pnp", pnp);
    }

    return ans;
}
//获取实际的装甲板从右下开始逆时针的四个边界点
std::vector<cv::Point2f> ArmorBox::getArmorPoints() const {
    if (light_blobs.size() != 2) {
        auto o = rect.tl();
        float w = rect.width, h = rect.height;
        return {cv::Point2f(o.x, o.y + h), o, cv::Point2f(o.x + w, o.y),
                cv::Point2f(o.x + w, o.y + h)};
    }
    cv::Point2f points[4], tmp[4];
    auto rectL = light_blobs.at(0).rect;
    auto rectR = light_blobs.at(1).rect;
    if (rectL.center.x > rectR.center.x) std::swap(rectL, rectR);
    if (rectL.size.width > rectL.size.height) {
        // rectL.size.width *= 2;
        rectL.points(tmp);
        points[0] = tmp[1];
        points[1] = tmp[2];
    } else {
        // rectL.size.height *= 2;
        rectL.points(tmp);
        points[0] = tmp[0];
        points[1] = tmp[1];
    }

    if (rectR.size.width > rectR.size.height) {
        // rectR.size.width *= 2;
        rectR.points(tmp);
        points[2] = tmp[3];
        points[3] = tmp[0];
    } else {
        // rectR.size.height *= 2;
        rectR.points(tmp);
        points[2] = tmp[2];
        points[3] = tmp[3];
    }
    return {points[0], points[1], points[2], points[3]};
}

// 获取装甲板中心点
cv::Point2f ArmorBox::getCenter() const {
    return cv::Point2f(rect.x + rect.width / 2, rect.y + rect.height / 2);
}

// 获取两个灯条中心点的间距
double ArmorBox::getBlobsDistance() const {
    if (light_blobs.size() == 2) {
        auto &x = light_blobs[0].rect.center;
        auto &y = light_blobs[1].rect.center;
        return sqrt((x.x - y.x) * (x.x - y.x) + (x.y - y.y) * (x.y - y.y));
    } else {
        return 0;
    }
}

// 获取灯条长度和间距的比例
double ArmorBox::lengthDistanceRatio() const {
    if (light_blobs.size() == 2) {
        return std::max(light_blobs[0].length, light_blobs[1].length) /
               getBlobsDistance();
    } else {
        return 100;
    }
}

// 计算装甲板到摄像头的距离
double ArmorBox::getBoxDistance() const {
    if (config.use_pnp) {
        auto ans = armorSolvePnP();
        return ans.second.z;
    } else {
        auto points = getArmorPoints();
        double x = getPointLength(points[0] - points[1]);
        return config.camConfig.FOCUS_PIXEL * config.ARMOR_H / x;
    }
}

// 装甲板的优先级比较
bool ArmorBox::operator<(const ArmorBox &box) const {
    if (id != box.id) {
        if (box_color == BOX_BLUE) {
            return prior_blue[id2name[id]] < prior_blue[id2name[box.id]];
        } else {
            return prior_red[id2name[id]] < prior_red[id2name[box.id]];
        }
    } else {
        auto d1 =
            (rect.x - config.IMAGE_CENTER_X) *
                (rect.x - config.IMAGE_CENTER_X) +
            (rect.y - config.IMAGE_CENTER_Y) * (rect.y - config.IMAGE_CENTER_Y);
        auto d2 = (box.rect.x - config.IMAGE_CENTER_X) *
                      (box.rect.x - config.IMAGE_CENTER_X) +
                  (box.rect.y - config.IMAGE_CENTER_Y) *
                      (box.rect.y - config.IMAGE_CENTER_Y);
        return d1 < d2;
    }
}
