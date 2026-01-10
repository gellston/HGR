#include "ClipSampler.h"

#pragma region Constructor
HGRAPI::V1::ClipSampler::ClipSampler(hgrapi::v1::clipSampler_ptr sampler) {
	this->instance = new hgrapi::v1::clipSampler_ptr(sampler);
}
#pragma endregion


#pragma region Destructor
HGRAPI::V1::ClipSampler::~ClipSampler() {

	this->Cleanup(true);
}
#pragma endregion


#pragma region Finalizer
HGRAPI::V1::ClipSampler::!ClipSampler() {

	this->Cleanup(false);

}
#pragma endregion

#pragma region Private Functions
void HGRAPI::V1::ClipSampler::Cleanup(bool disposing) {

	if (this->disposed == true) return;


	if (disposing) {
		//Managed Code Cleanup
	}

	if (this->instance != nullptr) {
		delete this->instance;
		this->instance = nullptr;
	}


	this->disposed = true;
}
#pragma endregion

#pragma region Public Property

int HGRAPI::V1::ClipSampler::MaxFrames::get() {
	return this->instance->get()->getMaxFrames();
}

void HGRAPI::V1::ClipSampler::MaxFrames::set(int value) {
	this->instance->get()->setMaxFrames(value);
}


int HGRAPI::V1::ClipSampler::SampleFrames::get() {
	return this->instance->get()->getSampleFrames();
}

void HGRAPI::V1::ClipSampler::SampleFrames::set(int value) {
	this->instance->get()->setSampleFrames(value);
}
#pragma endregion


#pragma region Public Functions

void HGRAPI::V1::ClipSampler::Append(HGRAPI::V1::Image^ image) {
	try {

		auto nativeImage = (hgrapi::v1::image_ptr *)image->Handle.ToPointer();
		this->instance->get()->append(*nativeImage);

	}
	catch (std::exception ex) {
		throw gcnew System::Exception(gcnew System::String(ex.what()));
	}
}

System::Collections::Generic::List<HGRAPI::V1::Image^>^ HGRAPI::V1::ClipSampler::RequestSampling() {
	try {

		System::Collections::Generic::List<HGRAPI::V1::Image^>^ images = gcnew System::Collections::Generic::List<HGRAPI::V1::Image^>();

		auto nativeImages = this->instance->get()->requestSampling();
		for (auto& nativeImage : nativeImages) {
			images->Add(gcnew HGRAPI::V1::Image(nativeImage));
		}

		return images;
	}
	catch (std::exception ex) {
		throw gcnew System::Exception(gcnew System::String(ex.what()));
	}
}
#pragma endregion


#pragma region Static Functions
HGRAPI::V1::ClipSampler^ HGRAPI::V1::ClipSampler::Create() {
	try {
		auto nativeSampler = hgrapi::v1::clipSampler::create();
		auto managedSampler = gcnew HGRAPI::V1::ClipSampler(nativeSampler);
		return managedSampler;
	}
	catch (std::exception ex) {
		throw gcnew System::Exception(gcnew System::String(ex.what()));
	}
}

void HGRAPI::V1::ClipSampler::DisposeImages(System::Collections::Generic::List<HGRAPI::V1::Image^>^ images) {
	try {

		for (int index = 0; index < images->Count; index++) {
			auto managedImage = images[index];
			delete managedImage;
		}

		images->Clear();
	}
	catch (...) {
		throw;
	}
}
#pragma endregion
