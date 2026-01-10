//C++
#include <hgr/memoryToken.h>
#include <msclr/marshal_cppstd.h>


//C++/CLI
#include "MemoryToken.h"

//C#
#pragma region Constructor

HGRAPI::V1::MemoryToken::MemoryToken(hgrapi::v1::memoryToken_ptr memoryToke) {
	this->instance = new hgrapi::v1::memoryToken_ptr(memoryToke);
}
#pragma endregion


#pragma region Destructor
HGRAPI::V1::MemoryToken::~MemoryToken() {
	this->Cleanup(true);
}
#pragma endregion

#pragma region Finalizer
HGRAPI::V1::MemoryToken::!MemoryToken() {
	this->Cleanup(false);
}
#pragma endregion

#pragma region Private Functions
void HGRAPI::V1::MemoryToken::Cleanup(bool disposing) {
	if (this->disposed == true) return;

	//Managed Resource Cleanup
	if (disposing) {

	}

	//Native Resource Cleanup
	delete this->instance;
	this->instance = nullptr;

	this->disposed = true;
}
#pragma endregion


#pragma region Public Proeprty
std::size_t HGRAPI::V1::MemoryToken::ActualSize::get() {
	try {


		return this->instance->get()->size();
	}
	catch (std::exception ex) {
		throw gcnew System::Exception(msclr::interop::marshal_as<System::String^>(ex.what()));
	}
}

std::size_t HGRAPI::V1::MemoryToken::Size::get() {
	try {

		return this->instance->get()->size();
	}
	catch (std::exception ex) {
		throw gcnew System::Exception(msclr::interop::marshal_as<System::String^>(ex.what()));
	}
}

System::IntPtr HGRAPI::V1::MemoryToken::Data::get() {
	try {
		return System::IntPtr(this->instance->get()->data());
	}
	catch (std::exception ex) {
		throw gcnew System::Exception(msclr::interop::marshal_as<System::String^>(ex.what()));
	}
}
#pragma endregion


