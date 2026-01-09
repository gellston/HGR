#pragma once

#ifndef HGR_INFERENCE
#define HGR_INFERENCE

#include <memory>
#include <string>

#include "hgr_api.h"
#include "image.h"

namespace hgrapi {
	namespace v1 {

		enum dlType {
			ghost3d
		};

		enum device {
			cpu,
			cuda
		};

		class memoryPool;
		using memoryPool_ptr = std::shared_ptr<memoryPool>;

		class hgr;
		using hgr_ptr = std::shared_ptr<hgr>;
		class impl_hgr;
		class hgr {
		private:
#pragma region Private Property
			std::unique_ptr<impl_hgr> impl;
#pragma endregion

#pragma region Constructor
			/// <summary>
			/// Constructor
			/// </summary>
			/// <returns></returns>
			HGR_NATIVE_API hgr(memoryPool_ptr pool);

			HGR_NATIVE_API hgr();
#pragma endregion
		public:



#pragma region Destructor
			/// <summary>
			/// Destructor
			/// </summary>
			/// <returns></returns>
			HGR_NATIVE_API ~hgr();
#pragma endregion

#pragma region Public Functions
			/// <summary>
			/// Setup internal modoel and instances
			/// </summary>
			/// <returns></returns>
			HGR_NATIVE_API void setup(const std::string& path, device _device);


			HGR_NATIVE_API void setup(dlType delType, device _device);

			/// <summary>
			/// Cleaup internal model and instances
			/// </summary>
			/// <returns></returns>
			HGR_NATIVE_API void shutdown();



			/// <summary>
			/// Predict from image
			/// </summary>
			/// <param name="path"></param>
			/// <returns></returns>
			HGR_NATIVE_API std::tuple<int, float, std::string, std::vector<float>> predict(const std::vector<hgrapi::v1::image_ptr>& frames);

			
			/// <summary>
			/// setter for Ema Alpha
			/// </summary>
			/// <param name="alpha"></param>
			/// <returns></returns>
			HGR_NATIVE_API void setEmaAlpha(float alpha);
#pragma endregion

#pragma region Static Functions
			/// <summary>
			/// Create HGR class
			/// </summary>
			/// <returns></returns>
			HGR_NATIVE_API static hgr_ptr create();


			/// <summary>
			/// Create HGR class with memory pool
			/// </summary>
			/// <param name="pool"></param>
			/// <returns></returns>
			HGR_NATIVE_API static hgr_ptr create(memoryPool_ptr pool);
#pragma endregion



		};
	}
}


#endif