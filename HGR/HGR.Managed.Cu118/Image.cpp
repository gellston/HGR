
#include <hgr/image.h>
#include <msclr/marshal_cppstd.h>


#include "Image.h"



#pragma region Constructor
HGRAPI::V1::Image::Image(hgrapi::v1::image_ptr image) {
	this->instance = new hgrapi::v1::image_ptr(image);
}
#pragma endregion


#pragma region Destructor 
HGRAPI::V1::Image::~Image() {
	this->Cleanup(true);
}
#pragma endregion


#pragma region Finalizer
HGRAPI::V1::Image::!Image() {
	this->Cleanup(false);
}
#pragma endregion

#pragma region Private Functions
void HGRAPI::V1::Image::Cleanup(bool disposing) {
	if (this->disposed == true) return;

	//Managed Cleanup
	if (disposing) {

	}

	delete this->instance;
	this->instance = nullptr;

	this->disposed = false;
}
#pragma endregion


#pragma region Public Property
uint32_t HGRAPI::V1::Image::Width::get() {
	try {

		return this->instance->get()->width();
	}
	catch (std::exception ex) {
		throw gcnew System::Exception(msclr::interop::marshal_as<System::String^>(ex.what()));
	}
}


uint32_t HGRAPI::V1::Image::Height::get() {
	try {

		return this->instance->get()->height();
	}
	catch (std::exception ex) {
		throw gcnew System::Exception(msclr::interop::marshal_as<System::String^>(ex.what()));
	}
}

uint32_t HGRAPI::V1::Image::Channel::get() {
	try {

		return this->instance->get()->channel();
	}
	catch (std::exception ex) {
		throw gcnew System::Exception(msclr::interop::marshal_as<System::String^>(ex.what()));
	}
}


std::size_t HGRAPI::V1::Image::Size::get() {
	try {

		return this->instance->get()->size();
	}
	catch (std::exception ex) {
		throw gcnew System::Exception(msclr::interop::marshal_as<System::String^>(ex.what()));
	}
}

uint32_t HGRAPI::V1::Image::Stride::get() {
	try {

		return this->instance->get()->stride();
	}
	catch (std::exception ex) {
		throw gcnew System::Exception(msclr::interop::marshal_as<System::String^>(ex.what()));
	}
}

System::IntPtr HGRAPI::V1::Image::Data::get() {
	try {

		return System::IntPtr(this->instance->get()->data());
	}
	catch (std::exception ex) {
		throw gcnew System::Exception(msclr::interop::marshal_as<System::String^>(ex.what()));
	}
}


System::IntPtr HGRAPI::V1::Image::Handle::get() {
	try {

		return System::IntPtr(this->instance);
	}
	catch (std::exception ex) {
		throw gcnew System::Exception(msclr::interop::marshal_as<System::String^>(ex.what()));
	}
}
#pragma endregion



#pragma region Static Functions
HGRAPI::V1::Image^ HGRAPI::V1::Image::Create(uint32_t width, uint32_t height, uint32_t channel) {
	try {

		auto nativeImage = hgrapi::v1::image::create(width, height, channel);
		auto managedImage = gcnew HGRAPI::V1::Image(nativeImage);

		return managedImage;
	}
	catch (std::exception ex) {
		throw gcnew System::Exception(msclr::interop::marshal_as<System::String^>(ex.what()));
	}
}

HGRAPI::V1::Image^ HGRAPI::V1::Image::Create(uint32_t width, uint32_t height, uint32_t channel, MemoryPool^ pool) {
	try {

		auto nativeMemoryPool = (hgrapi::v1::memoryPool_ptr*)pool->Handle.ToPointer();
		auto nativeImage = hgrapi::v1::image::create(width, height, channel, *nativeMemoryPool);
		auto managedImage = gcnew HGRAPI::V1::Image(nativeImage);
		return managedImage;
	}
	catch (std::exception ex) {
		throw gcnew System::Exception(msclr::interop::marshal_as<System::String^>(ex.what()));
	}
}


HGRAPI::V1::Image^ HGRAPI::V1::Image::Create(uint32_t width, uint32_t height, uint32_t channel, System::IntPtr buffer, MemoryPool^ pool) {
	try {

		auto nativeMemoryPool = (hgrapi::v1::memoryPool_ptr*)pool->Handle.ToPointer();
		auto nativePointer = buffer.ToPointer();
		auto nativeImage = hgrapi::v1::image::create(width, height, channel, nativePointer, *nativeMemoryPool);
		auto managedImage = gcnew HGRAPI::V1::Image(nativeImage);
		return managedImage;
	}
	catch (std::exception ex) {
		throw gcnew System::Exception(msclr::interop::marshal_as<System::String^>(ex.what()));
	}
}

HGRAPI::V1::Image^ HGRAPI::V1::Image::Resize(Image^ image, uint32_t width, uint32_t height) {
	try {

		auto nativeInput = (hgrapi::v1::image_ptr*)image->Handle.ToPointer();
		auto nativeOutput = hgrapi::v1::image::resize(*nativeInput, width, height);

		return gcnew HGRAPI::V1::Image(nativeOutput);
	}
	catch (std::exception ex) {
		throw gcnew System::Exception(msclr::interop::marshal_as<System::String^>(ex.what()));
	}
}

HGRAPI::V1::Image^ HGRAPI::V1::Image::Imread(System::String^ path, ColorType colorType) {
	try {
		auto nativePath = msclr::interop::marshal_as<std::string>(path);
		auto nativeColorType = safe_cast<hgrapi::v1::colorType>(colorType);
		auto nativeImage = hgrapi::v1::image::imread(nativePath, nativeColorType);

		return gcnew HGRAPI::V1::Image(nativeImage);
	}
	catch (std::exception ex) {
		throw gcnew System::Exception(msclr::interop::marshal_as<System::String^>(ex.what()));
	}
}

HGRAPI::V1::Image^ HGRAPI::V1::Image::Imread(System::String^ path, ColorType colorType, MemoryPool^ pool) {
	try {
		auto nativePath = msclr::interop::marshal_as<std::string>(path);
		auto nativePool = (hgrapi::v1::memoryPool_ptr*)pool->Handle.ToPointer();
		auto nativeColorType = safe_cast<hgrapi::v1::colorType>(colorType);
		auto nativeImage = hgrapi::v1::image::imread(nativePath, nativeColorType, *nativePool);

		return gcnew HGRAPI::V1::Image(nativeImage);
	}
	catch (std::exception ex) {
		throw gcnew System::Exception(msclr::interop::marshal_as<System::String^>(ex.what()));
	}
}

void HGRAPI::V1::Image::Imwrite(System::String^ path, Image^ image) {
	try {
		auto nativePath = msclr::interop::marshal_as<std::string>(path);
		auto nativeInputImage = (hgrapi::v1::image_ptr*)image->Handle.ToPointer();
		hgrapi::v1::image::imwrite(nativePath, *nativeInputImage);
	}
	catch (std::exception ex) {
		throw gcnew System::Exception(msclr::interop::marshal_as<System::String^>(ex.what()));
	}
}
#pragma endregion

