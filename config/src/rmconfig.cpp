#include <rmconfig.h>

void CameraConfig::init() {
    mtx = (cv::Mat_<double>(3, 3) << fx, 0, cx, 0, fy, cy, 0, 0, 1);
    dist = (cv::Mat_<double>(1, 5) << k1, k2, p1, p2, k3);
    cv::initUndistortRectifyMap(mtx, dist, cv::noArray(), mtx,
                                cv::Size(640, 640), CV_32FC1, mapx, mapy);
    FOCUS_PIXEL = (fx + fy) / 2;
}

void RmConfig::init_from_file() {
    std::ifstream ifs;
    ifs.open(configPath);
    if (!ifs.is_open()) {
        LOG(WARNING) << "config file not found! use default values";
        return;
    }
    JSONCPP_STRING errs;
    Json::CharReaderBuilder readerBuilder;
    Json::Value root;

    if (!Json::parseFromStream(readerBuilder, ifs, &root, &errs)) {
        LOG(WARNING) << "reader parse error: " << errs;
        LOG(WARNING) << "config file load error! use default values.";
        return;
    }
    // config
    Json::Value config = root["config"];
    show_origin = config["show_origin"].asBool();
    show_armor_box = config["show_armor_box"].asBool();
    show_light_box = config["show_light_bo"].asBool();
    show_class_id = config["show_class_id"].asBool();
    show_net_box = config["show_net_box"].asBool();
    wait_uart = config["wait_uart"].asBool();
    use_pnp = config["use_pnp"].asBool();
    use_video = config["use_video"].asBool();
    save_video = config["save_video"].asBool();
    show_light_blobs = config["show_light_blobs"].asBool();
    save_labelled_boxes = config["save_labelled_boxes"].asBool();
    show_pnp_axies = config["show_pnp_axies"].asBool();
    log_send_target = config["log_send_target"].asBool();
    show_energy_extra = config["show_energy_extra"].asBool();
    show_energy = config["show_energy"].asBool();
    save_mark = config["save_mark"].asBool();
    show_process = config["show_process"].asBool();
    show_info = config["show_info"].asBool();
    show_mcu_info = config["show_mcu_info"].asBool();
    uart_port = config["uart_port"].asString();
    video_path = config["video_path"].asString();
    camera_sn = config["camera_sn"].asString();
    has_show = show_origin || show_armor_box || show_light_box ||
               show_light_blobs || (show_pnp_axies && use_pnp) || show_energy ||
               show_energy_extra || show_process || show_net_box;
    // data
    Json::Value data = root["config_data"];
    ARMOR_CAMERA_GAIN = data["ARMOR_CAMERA_GAIN"].asInt();
    ARMOR_CAMERA_EXPOSURE = data["ARMOR_CAMERA_EXPOSURE"].asInt();
    ENERGY_CAMERA_GAIN = data["ENERGY_CAMERA_GAIN"].asInt();
    ENERGY_CAMERA_EXPOSURE = data["ENERGY_CAMERA_EXPOSURE"].asInt();
    ENEMY_COLOR = data["ENEMY_COLOR"].asInt();
    ANTI_TOP = data["ANTI_TOP"].asInt();
    MANUAL_DELTA_X = data["MANUAL_DELTA_X"].asDouble();
    MANUAL_DELTA_Y = data["MANUAL_DELTA_Y"].asDouble();
    ARMOR_DELTA_X = data["ARMOR_DELTA_X"].asDouble();
    ARMOR_PITCH_K = data["ARMOR_PITCH_K"].asDouble();
    ARMOR_PITCH_B = data["ARMOR_PITCH_B"].asDouble();

    // camera
    Json::Value camera = root["camera"];
    camConfig.fx = camera["fx"].asDouble();
    camConfig.fy = camera["fy"].asDouble();
    camConfig.cx = camera["cx"].asDouble();
    camConfig.cy = camera["cy"].asDouble();
    camConfig.k1 = camera["k1"].asDouble();
    camConfig.k2 = camera["k2"].asDouble();
    camConfig.p1 = camera["p1"].asDouble();
    camConfig.p2 = camera["p2"].asDouble();
    camConfig.k3 = camera["k3"].asDouble();
    camConfig.roi_height = camera["roi_height"].asInt64();
    camConfig.roi_width = camera["roi_width"].asInt64();
    camConfig.roi_offset_x = camera["roi_offset_x"].asInt64();
    camConfig.roi_offset_y = camera["roi_offset_y"].asInt64();
    camConfig.init();

    ifs.close();
}

void RmConfig::write_to_file() {
    Json::Value root;
    // config
    Json::Value config;
    config["show_origin"] = show_origin;
    config["show_armor_box"] = show_armor_box;
    config["show_light_box"] = show_light_box;
    config["show_net_box"] = show_net_box;
    config["show_class_id"] = show_class_id;
    config["wait_uart"] = wait_uart;
    config["use_pnp"] = use_pnp;
    config["use_video"] = use_video;
    config["save_video"] = save_video;
    config["show_light_blobs"] = show_light_blobs;
    config["save_labelled_boxes"] = save_labelled_boxes;
    config["show_pnp_axies"] = show_pnp_axies;
    config["log_send_target"] = log_send_target;
    config["show_energy_extra"] = show_energy_extra;
    config["show_energy"] = show_energy;
    config["save_mark"] = save_mark;
    config["show_process"] = show_process;
    config["show_info"] = show_info;
    config["show_mcu_info"] = show_mcu_info;
    config["uart_port"] = uart_port;
    config["video_path"] = video_path;
    config["camera_sn"] = camera_sn;

    // data
    Json::Value data;
    data["ARMOR_CAMERA_GAIN"] = ARMOR_CAMERA_GAIN;
    data["ARMOR_CAMERA_EXPOSURE"] = ARMOR_CAMERA_EXPOSURE;
    data["ENERGY_CAMERA_GAIN"] = ENERGY_CAMERA_GAIN;
    data["ENERGY_CAMERA_EXPOSURE"] = ENERGY_CAMERA_EXPOSURE;
    data["ENEMY_COLOR"] = ENEMY_COLOR;
    data["ANTI_TOP"] = ANTI_TOP;
    data["MANUAL_DELTA_X"] = MANUAL_DELTA_X;
    data["MANUAL_DELTA_Y"] = MANUAL_DELTA_Y;
    data["ARMOR_DELTA_X"] = ARMOR_DELTA_X;
    data["ARMOR_PITCH_K"] = ARMOR_PITCH_K;
    data["ARMOR_PITCH_B"] = ARMOR_PITCH_B;

    // camera
    Json::Value camera;
    camera["fx"] = camConfig.fx;
    camera["fy"] = camConfig.fy;
    camera["cx"] = camConfig.cx;
    camera["cy"] = camConfig.cy;
    camera["k1"] = camConfig.k1;
    camera["k2"] = camConfig.k2;
    camera["p1"] = camConfig.p1;
    camera["p2"] = camConfig.p2;
    camera["k3"] = camConfig.k3;
    camera["roi_height"] = camConfig.roi_height;
    camera["roi_width"] = camConfig.roi_width;
    camera["roi_offset_x"] = camConfig.roi_offset_x;
    camera["roi_offset_y"] = camConfig.roi_offset_y;

    root["config"] = config;
    root["config_data"] = data;
    root["camera"] = camera;

    std::ofstream ofs;
    ofs.open(configPath);
    Json::StreamWriterBuilder builder;
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    writer->write(root, &ofs);
    ofs.close();
}