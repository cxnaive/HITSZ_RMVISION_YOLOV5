#include "ArmorDetector.h"

static Logger gLogger;
const char* ArmorDetector::id_names[16] = {
    "R1", "B1", "R2", "B2", "R3",  "B3",  "R4",  "B4",
    "R5", "B5", "R7", "B7", "R10", "B10", "R11", "B11"};
ArmorDetector::ArmorDetector() {
    cudaSetDevice(DEVICE);
    char* trtModelStream{nullptr};
    size_t size{0};
    std::ifstream file(ENGINE_NAME, std::ios::binary);
    if (file.good()) {
        file.seekg(0, file.end);
        size = file.tellg();
        file.seekg(0, file.beg);
        trtModelStream = new char[size];
        assert(trtModelStream);
        file.read(trtModelStream, size);
        file.close();
    }
    runtime = createInferRuntime(gLogger);
    assert(runtime != nullptr);
    // int dlacore = runtime->getNbDLACores() - 1;
    // std::cout << "DLA cores:" << dlacore << std::endl;
    // runtime->setDLACore(dlacore);
    engine = runtime->deserializeCudaEngine(trtModelStream, size);
    assert(engine != nullptr);
    context = engine->createExecutionContext();
    assert(context != nullptr);
    delete[] trtModelStream;
    assert(engine->getNbBindings() == 2);

    // In order to bind the buffers, we need to know the names of the input and
    // output tensors. Note that indices are guaranteed to be less than
    // IEngine::getNbBindings()
    inputIndex = engine->getBindingIndex(INPUT_BLOB_NAME);
    outputIndex = engine->getBindingIndex(OUTPUT_BLOB_NAME);
    assert(inputIndex == 0);
    assert(outputIndex == 1);
    // Create GPU buffers on device
    CHECK(cudaMalloc(&buffers[inputIndex],
                     BATCH_SIZE * 3 * INPUT_H * INPUT_W * sizeof(float)));
    CHECK(cudaMalloc(&buffers[outputIndex],
                     BATCH_SIZE * OUTPUT_SIZE * sizeof(float)));

    CHECK(cudaStreamCreate(&stream));
}

ArmorDetector::~ArmorDetector() {
    // Release stream and buffers
    cudaStreamDestroy(stream);
    CHECK(cudaFree(buffers[inputIndex]));
    CHECK(cudaFree(buffers[outputIndex]));
    // Destroy the engine
    context->destroy();
    engine->destroy();
    runtime->destroy();
}

void ArmorDetector::doInference(IExecutionContext& context,
                                cudaStream_t& stream, void** buffers,
                                float* input, float* output, int batchSize) {
    // DMA input batch data to device, infer on the batch asynchronously, and
    // DMA output back to host
    CHECK(cudaMemcpyAsync(buffers[0], input,
                          batchSize * 3 * INPUT_H * INPUT_W * sizeof(float),
                          cudaMemcpyHostToDevice, stream));
    context.enqueue(batchSize, buffers, stream, nullptr);
    CHECK(cudaMemcpyAsync(output, buffers[1],
                          batchSize * OUTPUT_SIZE * sizeof(float),
                          cudaMemcpyDeviceToHost, stream));
    cudaStreamSynchronize(stream);
}

std::vector<ArmorInfo> ArmorDetector::detect(cv::Mat& img) {
    auto start = std::chrono::system_clock::now();
    // cv::Mat pr_img = preprocess_img(img);
    cv::Mat pr_img;
    cv::copyMakeBorder(img,pr_img,80,80,0,0,cv::BORDER_CONSTANT,cv::Scalar(128,128,128));
    int i = 0;
    for (int row = 0; row < INPUT_H; ++row) {
        uchar* uc_pixel = pr_img.data + row * pr_img.step;
        for (int col = 0; col < INPUT_W; ++col) {
            data[i] = (float)uc_pixel[2] / 255.0;
            data[i + INPUT_H * INPUT_W] = (float)uc_pixel[1] / 255.0;
            data[i + 2 * INPUT_H * INPUT_W] = (float)uc_pixel[0] / 255.0;
            uc_pixel += 3;
            ++i;
        }
    }
    // cv::Mat blob_img = cv::dnn::blobFromImage(pr_img,1.0,cv::Size(),cv::Scalar(),true) / 255.0;
    
    doInference(*context, stream, buffers, data, prob, BATCH_SIZE);
    std::vector<Yolo::Detection> res;
    nms(res, &prob[0], CONF_THRESH, NMS_THRESH);
    
    std::vector<ArmorInfo> ans;
    for (auto it : res) {
        cv::Rect r = get_rect(img, it.bbox);
        ans.push_back((ArmorInfo){r, (int)it.class_id, it.conf});
    }
    auto end = std::chrono::system_clock::now();
    total_time +=
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();
    ++total_cnt;
    return ans;
}

cv::Mat ArmorDetector::draw_output(cv::Mat& img,
                                   std::vector<ArmorInfo>& armors) {
    cv::Mat out = img.clone();
    char label[100];
    for (auto it : armors) {
        cv::rectangle(out, it.bbox, cv::Scalar(0x27, 0xC1, 0x36), 2);
        sprintf(label, "%s %.2f", id_names[it.id], it.conf);
        float wx = it.bbox.x;
        float wy = it.bbox.y;
        if(wy < 10) wy += it.bbox.height + 12;
        cv::putText(out, label, cv::Point(wx, wy - 1),
                    cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0xFF, 0xFF, 0xFF), 2);
    }
    return out;
}