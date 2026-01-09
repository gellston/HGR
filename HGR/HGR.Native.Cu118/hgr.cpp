
#include "hgr/hgr.h"
#include "hgr/memoryPool.h"

// Models
#include "models/ghost3d.h"

#include <onnxruntime_cxx_api.h>
#include <opencv2/opencv.hpp>

#include <mutex>
#include <vector>
#include <string>
#include <tuple>
#include <algorithm>
#include <cmath>
#include <limits>

namespace hgrapi {
    namespace v1 {

        static constexpr int kNumClasses = 27;

        static constexpr const char* GESTURES[kNumClasses] = {
            "Doing other things","No gesture","Drumming Fingers","Pulling Hand In","Pulling Two Fingers In",
            "Pushing Hand Away","Pushing Two Fingers Away","Rolling Hand Backward","Rolling Hand Forward",
            "Shaking Hand","Sliding Two Fingers Down","Sliding Two Fingers Left","Sliding Two Fingers Right",
            "Sliding Two Fingers Up","Stop Sign","Swiping Down","Swiping Left","Swiping Right","Swiping Up",
            "Thumb Down","Thumb Up","Turning Hand Clockwise","Turning Hand Counterclockwise",
            "Zooming In With Full Hand","Zooming In With Two Fingers","Zooming Out With Full Hand",
            "Zooming Out With Two Fingers"
        };

        static int argmax_index(const std::vector<float>& v) {
            if (v.empty()) return -1;
            return (int)std::distance(v.begin(), std::max_element(v.begin(), v.end()));
        }

    }
}


#pragma region IMPL
namespace hgrapi {
    namespace v1 {
        class impl_hgr {
        public:
            std::unique_ptr<Ort::Env> env;
            Ort::SessionOptions so;
            std::unique_ptr<Ort::Session> session;
            bool initialized = false;

            std::weak_ptr<hgrapi::v1::memoryPool> pool;

            // Fixed model input: [1, 3, T, H, W]
            int32_t model_t = 0;
            int32_t model_h = 0;
            int32_t model_w = 0;

            std::string input_name;
            std::string output_name;

            // EMA smoothing
            float ema_alpha = 0.7f;       // 0~1 (lower -> more sluggish)
            bool  ema_inited = false;
            std::vector<float> ema_probs;

            std::mutex mtx_run;

            impl_hgr() {}
        };
    }
}
#pragma endregion


#pragma region Constructor/Destructor
hgrapi::v1::hgr::hgr() : impl(new hgrapi::v1::impl_hgr()) {}

hgrapi::v1::hgr::hgr(hgrapi::v1::memoryPool_ptr pool) : impl(new hgrapi::v1::impl_hgr()) {
    this->impl->pool = pool;
}

hgrapi::v1::hgr::~hgr() {
    try { this->shutdown(); }
    catch (...) {}
}
#pragma endregion


#pragma region Public Functions

void hgrapi::v1::hgr::setup(const std::string& path, hgrapi::v1::device _device) {
    std::scoped_lock lock(this->impl->mtx_run);

    if (impl->initialized) {
        throw std::runtime_error("hgr::setup() called twice. Call shutdown() first.");
    }

    try {
        if (!impl->env) {
            impl->env = std::make_unique<Ort::Env>(ORT_LOGGING_LEVEL_WARNING, "hgr");
        }

        impl->so = Ort::SessionOptions{};
        impl->so.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
        impl->so.SetIntraOpNumThreads(1);
        impl->so.SetInterOpNumThreads(1);

        switch (_device) {
        case device::cpu:
            break;

        case device::cuda: {
            OrtCUDAProviderOptions options{};
            options.device_id = 0;
            options.cudnn_conv_algo_search = OrtCudnnConvAlgoSearchExhaustive;
            options.arena_extend_strategy = 0;
            impl->so.AppendExecutionProvider_CUDA(options);
            break;
        }
        default:
            throw std::runtime_error("Unsupported device");
        }

        std::wstring wpath(path.begin(), path.end());
        impl->session = std::make_unique<Ort::Session>(*impl->env, wpath.c_str(), impl->so);

        // Cache input/output names (so we don't allocate every Run)
        {
            Ort::AllocatorWithDefaultOptions allocator;
            auto in_name_alloc = impl->session->GetInputNameAllocated(0, allocator);
            auto out_name_alloc = impl->session->GetOutputNameAllocated(0, allocator);
            impl->input_name = in_name_alloc.get();
            impl->output_name = out_name_alloc.get();
        }

        // Parse model input shape: must be [1,3,T,H,W] all fixed
        {
            auto in_info = impl->session->GetInputTypeInfo(0).GetTensorTypeAndShapeInfo();
            auto shape = in_info.GetShape();

            if (shape.size() != 5) {
                throw std::runtime_error("Model input rank is not 5 (expected [1,3,T,H,W]).");
            }

            const int64_t N = shape[0];
            const int64_t C = shape[1];
            const int64_t T = shape[2];
            const int64_t H = shape[3];
            const int64_t W = shape[4];

            if (N != 1) throw std::runtime_error("Model input N != 1. This implementation assumes batch=1.");
            if (C != 3) throw std::runtime_error("Model input C != 3. Only 3-channel supported.");
            if (T <= 0 || H <= 0 || W <= 0) {
                throw std::runtime_error("Model input T/H/W is dynamic (<=0). Fixed input size required.");
            }

            impl->model_t = (int32_t)T;
            impl->model_h = (int32_t)H;
            impl->model_w = (int32_t)W;

            // Reset EMA state on setup
            impl->ema_inited = false;
            impl->ema_probs.clear();
        }

        impl->initialized = true;
    }
    catch (const Ort::Exception& e) {
        throw std::runtime_error(std::string("ONNX Runtime error: ") + e.what());
    }
}

void hgrapi::v1::hgr::setup(hgrapi::v1::dlType delType, hgrapi::v1::device _device) {
    std::scoped_lock lock(this->impl->mtx_run);

    if (impl->initialized) {
        throw std::runtime_error("hgr::setup() called twice. Call shutdown() first.");
    }

    try {
        if (!impl->env) {
            impl->env = std::make_unique<Ort::Env>(ORT_LOGGING_LEVEL_WARNING, "hgr");
        }

        impl->so = Ort::SessionOptions{};
        impl->so.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
        impl->so.SetIntraOpNumThreads(1);
        impl->so.SetInterOpNumThreads(1);

        switch (_device) {
        case device::cpu:
            break;

        case device::cuda: {
            OrtCUDAProviderOptions options{};
            options.device_id = 0;
            options.cudnn_conv_algo_search = OrtCudnnConvAlgoSearchExhaustive;
            options.arena_extend_strategy = 0;
            impl->so.AppendExecutionProvider_CUDA(options);
            break;
        }
        default:
            throw std::runtime_error("Unsupported device");
        }

        switch (delType) {
        case hgrapi::v1::dlType::ghost3d:
            // NOTE: your code uses `zerodce` here; keep as-is if that's your embedded model blob name.
            // Replace with your actual embedded bytes symbol for ghost3d.
            //impl->session = std::make_unique<Ort::Session>(*impl->env, zerodce, sizeof(zerodce), impl->so);
            break;
        default:
            throw std::runtime_error("Unsupported dlType");
        }

        // Cache input/output names
        {
            Ort::AllocatorWithDefaultOptions allocator;
            auto in_name_alloc = impl->session->GetInputNameAllocated(0, allocator);
            auto out_name_alloc = impl->session->GetOutputNameAllocated(0, allocator);
            impl->input_name = in_name_alloc.get();
            impl->output_name = out_name_alloc.get();
        }

        // Parse model input shape: must be [1,3,T,H,W] all fixed
        {
            auto in_info = impl->session->GetInputTypeInfo(0).GetTensorTypeAndShapeInfo();
            auto shape = in_info.GetShape();

            if (shape.size() != 5) {
                throw std::runtime_error("Model input rank is not 5 (expected [1,3,T,H,W]).");
            }

            const int64_t N = shape[0];
            const int64_t C = shape[1];
            const int64_t T = shape[2];
            const int64_t H = shape[3];
            const int64_t W = shape[4];

            if (N != 1) throw std::runtime_error("Model input N != 1. This implementation assumes batch=1.");
            if (C != 3) throw std::runtime_error("Model input C != 3. Only 3-channel supported.");
            if (T <= 0 || H <= 0 || W <= 0) {
                throw std::runtime_error("Model input T/H/W is dynamic (<=0). Fixed input size required.");
            }

            impl->model_t = (int32_t)T;
            impl->model_h = (int32_t)H;
            impl->model_w = (int32_t)W;

            // Reset EMA state on setup
            impl->ema_inited = false;
            impl->ema_probs.clear();
        }

        impl->initialized = true;
    }
    catch (const Ort::Exception& e) {
        throw std::runtime_error(std::string("ONNX Runtime error: ") + e.what());
    }
}

void hgrapi::v1::hgr::shutdown() {
    std::scoped_lock lock(this->impl->mtx_run);

    if (this->impl->initialized == false) {
        throw std::runtime_error("Its not initialized");
    }

    impl->session.reset();
    impl->initialized = false;
}

void hgrapi::v1::hgr::setEmaAlpha(float alpha) {
    std::scoped_lock lock(this->impl->mtx_run);
    this->impl->ema_alpha = std::clamp(alpha, 0.0f, 1.0f);
}


hgrapi::v1::result hgrapi::v1::hgr::predict(const std::vector<hgrapi::v1::image_ptr>& frames) {

    std::scoped_lock lock(this->impl->mtx_run);

    if (!impl || !impl->initialized || !impl->session)
        throw std::runtime_error("hgr::predict(frames): not initialized");

    if (frames.empty())
        throw std::runtime_error("hgr::predict(frames): frames is empty");

    const int32_t T = impl->model_t;
    const int32_t H = impl->model_h;
    const int32_t W = impl->model_w;

    if (T <= 0 || H <= 0 || W <= 0)
        throw std::runtime_error("hgr::predict(frames): model input size not set (setup failed?)");

    if ((int32_t)frames.size() != T) {
        throw std::runtime_error(
            "hgr::predict(frames): frame count mismatch. Expected T=" +
            std::to_string(T) + ", got " + std::to_string(frames.size())
        );
    }


    std::vector<int64_t> in_shape = { 1, 3, (int64_t)T, (int64_t)H, (int64_t)W };

    const size_t plane = (size_t)H * (size_t)W;   // H*W
    const size_t volume = (size_t)T * plane;       // T*H*W
    std::vector<float> in_data((size_t)3 * volume);

    float* c0 = in_data.data() + 0 * volume;
    float* c1 = in_data.data() + 1 * volume;
    float* c2 = in_data.data() + 2 * volume;

    // Resize each frame to (W,H) and pack as NCTHW
    for (int t = 0; t < T; ++t) {
        auto img = frames[(size_t)t];
        if (!img)
            throw std::runtime_error("hgr::predict(frames): null frame at t=" + std::to_string(t));
        if (img->channel() != 3)
            throw std::runtime_error("hgr::predict(frames): only 3-channel supported");

        auto rimg = hgrapi::v1::image::resize(img, (uint32_t)W, (uint32_t)H);

        const unsigned char* src = rimg->data();
        const uint32_t stride = rimg->stride();
        if (!src)
            throw std::runtime_error("hgr::predict(frames): resized frame data is null");

        const size_t t_off = (size_t)t * plane; // offset in T dimension

        for (int y = 0; y < H; ++y) {
            const unsigned char* row = src + (size_t)y * stride;
            for (int x = 0; x < W; ++x) {
                const unsigned char* px = row + (size_t)x * 3;
                const size_t hw = (size_t)y * (size_t)W + (size_t)x;
                const size_t idx = t_off + hw;


                c0[idx] = px[0] / 255.0f;
                c1[idx] = px[1] / 255.0f;
                c2[idx] = px[2] / 255.0f;
            }
        }
    }

    Ort::MemoryInfo mem = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
        mem, in_data.data(), in_data.size(), in_shape.data(), in_shape.size()
    );

    const char* input_names[] = { impl->input_name.c_str() };
    const char* output_names[] = { impl->output_name.c_str() };

    auto outputs = impl->session->Run(
        Ort::RunOptions{ nullptr },
        input_names, &input_tensor, 1,
        output_names, 1
    );

    if (outputs.empty() || !outputs[0].IsTensor())
        throw std::runtime_error("hgr::predict(frames): output is not a tensor");

    const float* logits = outputs[0].GetTensorData<float>();
    if (!logits)
        throw std::runtime_error("hgr::predict(frames): output tensor data is null");

    // Expect output shape [1, 27] (or [27])
    auto out_info = outputs[0].GetTensorTypeAndShapeInfo();
    auto out_shape = out_info.GetShape();

    size_t K = 0;
    if (out_shape.size() == 2 && out_shape[0] == 1 && out_shape[1] > 0) {
        K = (size_t)out_shape[1];
    }
    else if (out_shape.size() == 1 && out_shape[0] > 0) {
        K = (size_t)out_shape[0];
    }
    else {
        throw std::runtime_error("hgr::predict(frames): unsupported output shape");
    }

    if (K != (size_t)hgrapi::v1::kNumClasses) {
        throw std::runtime_error(
            "hgr::predict(frames): output class count mismatch. Expected 27, got " + std::to_string(K)
        );
    }


    std::vector<float> probs(K);
    for (size_t i = 0; i < K; ++i) {
        probs[i] = logits[i];
    }

    // EMA smoothing
    // 처음에는 나온 확률 그대로 사용
    if (!impl->ema_inited || impl->ema_probs.size() != probs.size()) {
        impl->ema_probs = probs;
        impl->ema_inited = true;
    }
    else {
        const float a = std::clamp(impl->ema_alpha, 0.0f, 1.0f);
        for (size_t i = 0; i < probs.size(); ++i) {
            impl->ema_probs[i] = a * probs[i] + (1.0f - a) * impl->ema_probs[i];
        }
    }

    // best result (EMA)
    const int best = hgrapi::v1::argmax_index(impl->ema_probs);
    float bestProb = 0.0f;
    std::string bestLabel = "Unknown";

    if (best >= 0 && best < hgrapi::v1::kNumClasses) {
        bestProb = impl->ema_probs[(size_t)best];
        bestLabel = hgrapi::v1::GESTURES[best];
    }

    return { best, bestProb, bestLabel, impl->ema_probs };
}

#pragma endregion


#pragma region Static Functions
hgrapi::v1::hgr_ptr hgrapi::v1::hgr::create(memoryPool_ptr pool) {
    return std::shared_ptr<hgrapi::v1::hgr>(new hgr(pool));
}

hgrapi::v1::hgr_ptr hgrapi::v1::hgr::create() {
    return std::shared_ptr<hgrapi::v1::hgr>(new hgr());
}
#pragma endregion
