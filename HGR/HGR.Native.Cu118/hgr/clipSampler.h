#pragma once


#ifndef HGR_CLIP_SAMPLER
#define HGR_ClIP_SAMPLER

#include <memory>

#include "hgr_api.h"
#include "image.h"
#include "memoryPool.h"

namespace hgrapi {
	namespace v1 {
		
		class impl_clipSampler;
		class clipSampler;
		using clipSampler_ptr = std::shared_ptr<clipSampler>;
		class clipSampler {

		private:
#pragma region Private Property
			std::unique_ptr<impl_clipSampler> impl;
#pragma endregion

#pragma region Public Constructor
			HGR_NATIVE_API clipSampler();
#pragma endregion

		public:
#pragma region Destructor
			HGR_NATIVE_API ~clipSampler();
#pragma endregion
			

#pragma region Public Functions
			HGR_NATIVE_API void setMaxFrames(int count);
			HGR_NATIVE_API int getMaxFrames();

			HGR_NATIVE_API void setSampleFrames(int count);
			HGR_NATIVE_API int getSampleFrames();
	
			HGR_NATIVE_API void append(image_ptr image);
			HGR_NATIVE_API std::vector<image_ptr> requestSampling();
#pragma endregion


#pragma region Static Functions
			HGR_NATIVE_API static clipSampler_ptr create();
#pragma endregion


		};


	}
}



#endif