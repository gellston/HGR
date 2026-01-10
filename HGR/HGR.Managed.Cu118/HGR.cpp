

//C++

#include <msclr/marshal_cppstd.h>


//C++/CLI
#include "HGR.h"




#pragma region Constructor
HGRAPI::V1::HGR::HGR(hgrapi::v1::hgr_ptr hgr) {
	this->instance = new hgrapi::v1::hgr_ptr(hgr);
}
#pragma endregion


#pragma region Destructor
HGRAPI::V1::HGR::~HGR() {
	this->Cleanup(true);
}
#pragma endregion


#pragma region Finalizer
HGRAPI::V1::HGR::!HGR() {
	this->Cleanup(false);
}
#pragma endregion


#pragma region Private Functions
void HGRAPI::V1::HGR::Cleanup(bool disposing) {

	if (this->disposed == true) return;


	if (disposing) {
		//Managed Resource Clean
	}


	//Native Resource Clean
	if (this->instance != nullptr) {
		delete this->instance;
		this->instance = nullptr;
	}


	this->disposed = true;

}
#pragma endregion



#pragma region Public Property
void HGRAPI::V1::HGR::EmaAlpha::set(float value) {
	this->instance->get()->setEmaAlpha(value);
}
#pragma endregion


#pragma region Public Functions
void HGRAPI::V1::HGR::Setup(HGRAPI::V1::DLType dltype, HGRAPI::V1::Device device) {
	try {

		auto nativeDlType = safe_cast<hgrapi::v1::dlType>(dltype);
		auto nativeDevice = safe_cast<hgrapi::v1::device>(device);

		this->instance->get()->setup(nativeDlType, nativeDevice);

	}
	catch (std::exception ex) {
		throw gcnew System::Exception(gcnew System::String(ex.what()));
	}
}

void HGRAPI::V1::HGR::Setup(System::String^ path, HGRAPI::V1::Device device) {
	try {

		auto nativePath = msclr::interop::marshal_as<std::string>(path);
		auto nativeDevice = safe_cast<hgrapi::v1::device>(device);

		this->instance->get()->setup(nativePath, nativeDevice);

	}
	catch (std::exception ex) {
		throw gcnew System::Exception(gcnew System::String(ex.what()));
	}
}

void HGRAPI::V1::HGR::Shutdown() {
	try {

		this->instance->get()->shutdown();

	}
	catch (std::exception ex) {
		throw gcnew System::Exception(gcnew System::String(ex.what()));
	}
}

HGRAPI::V1::Result HGRAPI::V1::HGR::Predict(System::Collections::Generic::List<HGRAPI::V1::Image^>^ images) {
	try {

		std::vector<hgrapi::v1::image_ptr> nativeImages;

		for (int index = 0; index < images->Count; index++) {
			auto nativeImagePtr = (hgrapi::v1::image_ptr *)images[index]->Handle.ToPointer();
			nativeImages.push_back(*nativeImagePtr);
		}

		auto nativeResult = this->instance->get()->predict(nativeImages);

		HGRAPI::V1::Result managedResult;
		managedResult.Name = msclr::interop::marshal_as<System::String^>(nativeResult.name);
		managedResult.Index = nativeResult.index;
		managedResult.Prob = nativeResult.prob;
		managedResult.Probs = gcnew System::Collections::Generic::List<float>();

		for (auto& prob : nativeResult.probs) {
			managedResult.Probs->Add(prob);
		}

		return managedResult;
	}
	catch (std::exception ex) {
		throw gcnew System::Exception(gcnew System::String(ex.what()));
	}
}
#pragma endregion


#pragma region Static Functions
HGRAPI::V1::HGR^ HGRAPI::V1::HGR::Create() {
	try {

		return gcnew HGRAPI::V1::HGR(hgrapi::v1::hgr::create());
	}
	catch (std::exception ex) {
		throw gcnew System::Exception(gcnew System::String(ex.what()));
	}
}

HGRAPI::V1::HGR^ HGRAPI::V1::HGR::Create(HGRAPI::V1::MemoryPool^ pool) {
	try {
		auto nativePool = (hgrapi::v1::memoryPool_ptr *)pool->Handle.ToPointer();
		return gcnew HGRAPI::V1::HGR(hgrapi::v1::hgr::create(*nativePool));
	}
	catch (std::exception ex) {
		throw gcnew System::Exception(gcnew System::String(ex.what()));
	}
}
#pragma endregion
