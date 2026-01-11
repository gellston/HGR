#include <iostream>
#include <opencv2/opencv.hpp>


#include <hgr/hgr.h>
#include <hgr/clipSampler.h>

int main()
{

    try {

        auto memoryPool = hgrapi::v1::memoryPool::create();

        auto hgr = hgrapi::v1::hgr::create();
        hgr->setup(hgrapi::v1::dlType::ghost3d, hgrapi::v1::device::cuda);
        hgr->setEmaAlpha(0.2f);

        auto sampler = hgrapi::v1::clipSampler::create();
        sampler->setMaxFrames(40);
        sampler->setSampleFrames(16);

        cv::VideoCapture cap;
        cap.open(0);

        if (!cap.isOpened()) {
            std::cerr << "Failed to open VideoCapture.\n";
            return 1;
        }

        cv::Mat frame;

        while (true) {

            if (!cap.read(frame) || frame.empty()) {
                std::cerr << "End of stream or failed to read frame.\n";
                break;
            }

            auto dlImage = hgrapi::v1::image::create(frame.cols, frame.rows, 3, memoryPool);
            std::memcpy(dlImage->data(), frame.data, dlImage->size());
            auto resizeImage = hgrapi::v1::image::resize(dlImage, 128, 64);
            sampler->append(resizeImage);


            auto samples = sampler->requestSampling();
            auto result = hgr->predict(samples);


            std::cout << "name : " << result.name << " prob : " << result.prob << std::endl;

            cv::imshow("capture", frame);
            cv::waitKey(1);
        }

    }
    catch (std::exception ex) {
        std::cout << ex.what() << std::endl;
    }

    return 0;

}
