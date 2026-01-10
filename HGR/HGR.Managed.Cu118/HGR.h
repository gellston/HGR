#pragma once


//C++
#include "hgr/hgr.h"

//C++/CLI
#include "MemoryPool.h"
#include "Image.h"

//C#


namespace HGRAPI {
	namespace V1 {

		public value struct Result {
			int Index;
			float Prob;
			System::String^ Name;
			System::Collections::Generic::List<float>^ Probs;
		};

		public enum class DLType {
			Ghost3D
		};

		public enum class Device {
			Cpu,
			Cuda
		};

		public ref class HGR {
		private:

#pragma region Private Property
			bool disposed = false;
			hgrapi::v1::hgr_ptr* instance = nullptr;
#pragma endregion

#pragma region Constructor
			HGR(hgrapi::v1::hgr_ptr hgr);
#pragma endregion

#pragma region Private Functions
			void Cleanup(bool disposing);
#pragma endregion


		public:

#pragma region Destructor
			~HGR();
#pragma endregion

#pragma region Finalizer
			!HGR();
#pragma endregion

#pragma region Public Property
			property float EmaAlpha {
				void set(float value);
			}
#pragma endregion

#pragma region Public Functions
			void Setup(System::String^ path, HGRAPI::V1::Device device);

			void Setup(DLType dlType, HGRAPI::V1::Device device);

			void Shutdown();

			Result Predict(System::Collections::Generic::List<HGRAPI::V1::Image^>^ images);
#pragma endregion
			
#pragma region Static Functions
			static HGR^ Create();
			static HGR^ Create(HGRAPI::V1::MemoryPool^ pool);
#pragma endregion

		};
	}
}