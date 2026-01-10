#pragma once


//C++
#include <hgr/clipSampler.h>

//C++/CLI
#include "Image.h"


//C#



namespace HGRAPI {
	namespace V1 {


		public ref class ClipSampler {
		private:

#pragma region Private Property
			hgrapi::v1::clipSampler_ptr* instance = nullptr;
			bool disposed = false;
#pragma endregion


#pragma region Constructor
			ClipSampler(hgrapi::v1::clipSampler_ptr sampler);
#pragma endregion

#pragma region Private Functions
			void Cleanup(bool disposing);
#pragma endregion


		public:

#pragma region Destructor
			~ClipSampler();
#pragma endregion

#pragma region Finalizer
			!ClipSampler();
#pragma endregion



#pragma region Public Property
			property int MaxFrames {
				int get();
				void set(int value);
			}

			property int SampleFrames {
				int get();
				void set(int vale);
			}
#pragma endregion


#pragma region Public Functions
			void Append(HGRAPI::V1::Image^ image);
			System::Collections::Generic::List<HGRAPI::V1::Image^>^ RequestSampling();
#pragma endregion


#pragma region Static Functions
			static ClipSampler^ Create();

			static void DisposeImages(System::Collections::Generic::List<HGRAPI::V1::Image^>^ images);
#pragma endregion

		};
	}
}