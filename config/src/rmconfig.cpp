#include <rmconfig.h>

void CameraConfig::init()
{
    mtx = (cv::Mat_<double>(3, 3) << fx, 0, cx, 0, fy, cy, 0, 0, 1);
    dist = (cv::Mat_<double>(1, 5) << k1, k2, p1, p2, k3);
    cv::initUndistortRectifyMap(mtx, dist, cv::noArray(), mtx,
                                cv::Size(640, 640), CV_32FC1, mapx, mapy);
    FOCUS_PIXEL = (fx + fy) / 2;
}

void RmConfig::init_from_file()
{
    std::ifstream ifs;
    ifs.open(configPath);
    if (!ifs.is_open())
    {
        LOG(WARNING) << "config file not found! use default values";
        return;
    }
    JSONCPP_STRING errs;
    Json::CharReaderBuilder readerBuilder;
    Json::Value root;

    if (!Json::parseFromStream(readerBuilder, ifs, &root, &errs))
    {
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
    ignore_types.clear();
    std::string ignore_info = "";
    for (int i = 0; i < config["ignore_types"].size(); ++i)
    {
        std::string ignore_item = config["ignore_types"][i].asString();
        ignore_types.insert(ignore_item);
        ignore_info = ignore_info + ignore_item + " ";
    }
    LOG(WARNING) << "NOTE: ignored types: " << ignore_info;
    // data
    Json::Value data = root["config_data"];
    std::string mode_sting = data["RUNMODE"].asString();
    LOG(INFO) << "mode string:" << mode_sting;
    if (mode_sting.length() == 1)
    {
        char op = mode_sting[0];
        if (op == ARMOR_STATE || op == SMALL_ENERGY_STATE || op == BIG_ENERGY_STATE)
            RUNMODE = op;
        else
            RUNMODE = ARMOR_STATE;
    }
    else
        RUNMODE = ARMOR_STATE;
    ARMOR_CAMERA_GAIN = data["ARMOR_CAMERA_GAIN"].asInt();
    ARMOR_CAMERA_EXPOSURE = data["ARMOR_CAMERA_EXPOSURE"].asInt();
    ENERGY_CAMERA_GAIN = data["ENERGY_CAMERA_GAIN"].asInt();
    ENERGY_CAMERA_EXPOSURE = data["ENERGY_CAMERA_EXPOSURE"].asInt();
    ENEMY_COLOR = data["ENEMY_COLOR"].asInt();
    ENERGY_DELAY_TIME = data["ENERGY_DELAY_TIME"].asDouble();
    ENERGY_EXTRA_DELTA_Y = data["ENERGY_EXTRA_DELTA_Y"].asDouble();
    ENERGY_EXTRA_DELTA_X = data["ENERGY_EXTRA_DELTA_X"].asDouble();
    ANTI_TOP = data["ANTI_TOP"].asInt();
    MANUAL_DELTA_X = data["MANUAL_DELTA_X"].asDouble();
    MANUAL_DELTA_Y = data["MANUAL_DELTA_Y"].asDouble();
    //ARMOR_DELTA_X = data["ARMOR_DELTA_X"].asDouble();
    ARMOR_DELTA_Y = data["ARMOR_DELTA_Y"].asDouble();
    // ARMOR_PITCH_DELTA_K = data["ARMOR_PITCH_DELTA_K"].asDouble();
    // ARMOR_PITCH_DELTA_B = data["ARMOR_PITCH_DELTA_B"].asDouble();
    ARMOR_YAW_KP = data["ARMOR_YAW_KP"].asDouble();
    ARMOR_YAW_KI = data["ARMOR_YAW_KI"].asDouble();
    ARMOR_YAW_KD = data["ARMOR_YAW_KD"].asDouble();
    ARMOR_PITCH_KP = data["ARMOR_PITCH_KP"].asDouble();
    ARMOR_PITCH_KI = data["ARMOR_PITCH_KI"].asDouble();
    ARMOR_PITCH_KD = data["ARMOR_PITCH_KD"].asDouble();
    PROG_DELAY = data["PROG_DELAY"].asDouble();

    std::string k_info = "";
    std::string b_info = "";
    std::string s_info = "";
    std::string delta_x_info = "";
    if (data["ARMOR_PITCH_DELTA_K"].size() !=
            data["ARMOR_PITCH_DELTA_B"].size() ||
        data["ARMOR_PITCH_DELTA_K"].size() !=
            data["ARMOR_BULLET_SPEED_SET"].size() ||
        data["ARMOR_PITCH_DELTA_K"].size() != data["ARMOR_DELTA_X"].size() ||
        data["ARMOR_PITCH_DELTA_K"].size() == 0)
    {
        LOG(ERROR) << "Invalid armor pitch settings!";
        k_info = b_info = s_info = delta_x_info = "0";
    }
    else
    {
        ARMOR_PITCH_DELTA_K.clear();
        ARMOR_PITCH_DELTA_B.clear();
        ARMOR_BULLET_SPEED_SET.clear();
        ARMOR_DELTA_X.clear();

        for (int i = 0; i < data["ARMOR_PITCH_DELTA_K"].size(); ++i)
        {
            double nowk = data["ARMOR_PITCH_DELTA_K"][i].asDouble();
            double nowb = data["ARMOR_PITCH_DELTA_B"][i].asDouble();
            double nows = data["ARMOR_BULLET_SPEED_SET"][i].asDouble();
            double now_delta_x = data["ARMOR_DELTA_X"][i].asDouble();
            ARMOR_PITCH_DELTA_K.push_back(nowk);
            ARMOR_PITCH_DELTA_B.push_back(nowb);
            ARMOR_BULLET_SPEED_SET.push_back(nows);
            ARMOR_DELTA_X.push_back(now_delta_x);
            k_info = k_info + std::to_string(nowk) + " ";
            b_info = b_info + std::to_string(nowb) + " ";
            s_info = s_info + std::to_string(nows) + " ";
            delta_x_info = delta_x_info + std::to_string(now_delta_x);
        }
    }
    LOG(WARNING) << "NOTE: pitch delta k: " << k_info;
    LOG(WARNING) << "NOTE: pitch delta b: " << b_info;
    LOG(WARNING) << "NOTE: bullet speed set: " << s_info;
    LOG(WARNING) << "NOTE: delta x set:" << delta_x_info;

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

void RmConfig::write_to_file()
{
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
    config["ignore_types"].clear();
    for (auto &item : ignore_types)
    {
        config["ignore_types"].append(item);
    }

    // data
    Json::Value data;
    data["RUNMODE"] = std::string(1, RUNMODE);
    data["ARMOR_CAMERA_GAIN"] = ARMOR_CAMERA_GAIN;
    data["ARMOR_CAMERA_EXPOSURE"] = ARMOR_CAMERA_EXPOSURE;
    data["ENERGY_CAMERA_GAIN"] = ENERGY_CAMERA_GAIN;
    data["ENERGY_CAMERA_EXPOSURE"] = ENERGY_CAMERA_EXPOSURE;
    data["ENEMY_COLOR"] = ENEMY_COLOR;
    data["ENERGY_DELAY_TIME"] = ENERGY_DELAY_TIME;
    data["ENERGY_EXTRA_DELTA_Y"] = ENERGY_EXTRA_DELTA_Y;
    data["ENERGY_EXTRA_DELTA_X"] = ENERGY_EXTRA_DELTA_X;
    data["ANTI_TOP"] = ANTI_TOP;
    data["MANUAL_DELTA_X"] = MANUAL_DELTA_X;
    data["MANUAL_DELTA_Y"] = MANUAL_DELTA_Y;
    //data["ARMOR_DELTA_X"] = ARMOR_DELTA_X;
    data["ARMOR_DELTA_Y"] = ARMOR_DELTA_Y;
    // data["ARMOR_PITCH_DELTA_K"] = ARMOR_PITCH_DELTA_K;
    // data["ARMOR_PITCH_DELTA_B"] = ARMOR_PITCH_DELTA_B;
    data["ARMOR_YAW_KP"] = ARMOR_YAW_KP;
    data["ARMOR_YAW_KI"] = ARMOR_YAW_KI;
    data["ARMOR_YAW_KD"] = ARMOR_YAW_KD;
    data["ARMOR_PITCH_KP"] = ARMOR_PITCH_KP;
    data["ARMOR_PITCH_KI"] = ARMOR_PITCH_KI;
    data["ARMOR_PITCH_KD"] = ARMOR_PITCH_KD;
    data["PROG_DELAY"] = PROG_DELAY;
    for (int i = 0; i < ARMOR_PITCH_DELTA_K.size(); ++i)
    {
        data["ARMOR_PITCH_DELTA_K"].append(ARMOR_PITCH_DELTA_K[i]);
        data["ARMOR_PITCH_DELTA_B"].append(ARMOR_PITCH_DELTA_B[i]);
        data["ARMOR_BULLET_SPEED_SET"].append(ARMOR_BULLET_SPEED_SET[i]);
        data["ARMOR_DELTA_X"].append(ARMOR_DELTA_X[i]);
    }

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