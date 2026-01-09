
#include "hgr/clipSampler.h"

#include <mutex>

#pragma region Impl
namespace hgrapi {
	namespace v1 {
		class impl_clipSampler {
		public:
			int maxFrames = 34;

			int sampleFrames = 17;

			std::mutex mtx_sample;


			std::vector<hgrapi::v1::image_ptr> samples;
		};
	}
}
#pragma endregion



#pragma region Constructor
hgrapi::v1::clipSampler::clipSampler() : impl(new hgrapi::v1::impl_clipSampler()) {

}
#pragma endregion


#pragma region Destructor
hgrapi::v1::clipSampler::~clipSampler() {

}
#pragma endregion


#pragma region Public Functions

int hgrapi::v1::clipSampler::getMaxFrames() {

	std::scoped_lock lock(this->impl->mtx_sample);

	return this->impl->maxFrames;
}

void hgrapi::v1::clipSampler::setMaxFrames(int count) {

	std::scoped_lock lock(this->impl->mtx_sample);

	this->impl->maxFrames = count;
}



void hgrapi::v1::clipSampler::setSampleFrames(int count) {

	std::scoped_lock lock(this->impl->mtx_sample);


	this->impl->sampleFrames = count;
}


int hgrapi::v1::clipSampler::getSampleFrames() {

	std::scoped_lock lock(this->impl->mtx_sample);


	return this->impl->sampleFrames;

}



void hgrapi::v1::clipSampler::append(image_ptr image) {

	std::scoped_lock lock(this->impl->mtx_sample);
	this->impl->samples.push_back(image);
	if (this->impl->maxFrames > 0) {
		while (static_cast<int>(this->impl->samples.size()) > this->impl->maxFrames) {
			this->impl->samples.erase(this->impl->samples.begin());
		}
	}

}


std::vector<hgrapi::v1::image_ptr> hgrapi::v1::clipSampler::requestSampling() {


	std::scoped_lock lock(this->impl->mtx_sample);

	const int k = this->impl->sampleFrames;
	const size_t n = this->impl->samples.size();

	if (k <= 0 || n == 0) {
		throw std::exception("Invalid sampling count");
	}

	std::vector<hgrapi::v1::image_ptr> out;
	out.reserve(static_cast<size_t>(k));

	if (n == 1) {
		for (int i = 0; i < k; ++i) 
			out.push_back(this->impl->samples[0]);
		return out;
	}

	if (k == 1) {
		out.push_back(this->impl->samples[n - 1]);
		return out;
	}

	for (int i = 0; i < k; ++i) {
		const double t = static_cast<double>(i) / static_cast<double>(k - 1);
		size_t idx = static_cast<size_t>(std::llround(t * static_cast<double>(n - 1)));

		if (idx >= n) idx = n - 1;
		out.push_back(this->impl->samples[idx]);
	}

	return out;
}
#pragma endregion


#pragma region Static Functions
hgrapi::v1::clipSampler_ptr hgrapi::v1::clipSampler::create() {
	return std::shared_ptr<hgrapi::v1::clipSampler>(new hgrapi::v1::clipSampler());
}
#pragma endregion

