<div align="center">

<img src="https://raw.githubusercontent.com/gellston/HGR/main/icon.png" alt="HGR Icon" width="140" />

  # HGR (Hand Gesture Recognition)

  AI-based Hand Gesture Recognition inference API for <b>Windows x64</b>.

  <!-- Native (Cu118) -->
  <a href="https://www.nuget.org/packages/HGR.Native.Cu118">
    <img src="https://img.shields.io/nuget/v/HGR.Native.Cu118.svg?style=for-the-badge&logo=nuget&label=NuGet%20Native%20Cu118" />
  </a>
  <img src="https://img.shields.io/badge/CUDA-11.8-76B900?style=for-the-badge&logo=nvidia" />

  <br/>

  <!-- Managed (Cuda118) -->
  <a href="https://www.nuget.org/packages/HGR.Managed.Cu118">
    <img src="https://img.shields.io/nuget/v/HGR.Managed.Cu118.svg?style=for-the-badge&logo=nuget&label=NuGet%20Managed%20Cuda118" />
  </a>
  <img src="https://img.shields.io/badge/CUDA-11.8-76B900?style=for-the-badge&logo=nvidia" />

  <br/>

  <img src="https://img.shields.io/badge/C%2B%2B-Used-00599C?style=for-the-badge&logo=c%2B%2B" />
  <img src="https://img.shields.io/badge/C%2B%2B%2FCLI-Used-512BD4?style=for-the-badge" />
  <img src="https://img.shields.io/badge/C%23-Used-512BD4?style=for-the-badge&logo=csharp" />
  <img src="https://img.shields.io/badge/Python-Model%20Training-3776AB?style=for-the-badge&logo=python" />

</div>


## Overview

**HGR** is a hand-gesture recognition library that provides an **inference API** for an AI model trained in **Python**.

- Name: **HGR (Hand Gesture Recognition)**
- Author / Maintainer: **gellston**
- Examples:
  * [`CppInference`](https://github.com/gellston/HGR/tree/main/HGR/CppInference)
  * [`CSharpInference`](https://github.com/gellston/HGR/tree/main/HGR/CSharpInference)
  * [`FSharpInference`](https://github.com/gellston/HGR/tree/main/HGR/FSharpInference)
  * [`VBInference`](https://github.com/gellston/HGR/tree/main/HGR/VBInference)

### NuGet Packages (Native vs Managed)

- **`HGR.Native.Cu118`** *(C++ / native)*  
  Native runtime + C++ API for Windows x64. Use this if you want to call HGR directly from C++.

- **`HGR.Managed.Cuda118`** *(C# / .NET)*  
  A managed wrapper (C++/CLI) around the native runtime for a smoother .NET experience on Windows x64.

> Both packages target **Windows x64**. GPU inference requires a compatible NVIDIA GPU environment (see below).

## Demonstration
https://www.youtube.com/watch?v=CjE8BRevUUY

### Training Scripts
- Training scripts used for model development are available here:
  * https://github.com/gellston/HGR/tree/main/python

### Dataset
- This project used the **Jester-Dataset** :
  * https://www.qualcomm.com/developer/software/jester-dataset

---

## Platform

- ✅ **Windows x64 only**
  - Even if you use **C#** or **C++**, this library only works on **Windows x64**.

---

## Runtime (CPU / CUDA)

### CPU
- **CPU inference**: no special runtime constraints (beyond standard Windows x64 requirements).

### CUDA (GPU)
- **CUDA inference** requires an NVIDIA GPU + driver.
- **You must install CUDA 11.8** on the target machine.
- **You must install cuDNN 8.5.0.96** (CUDA 11.x build) on the target machine.
- This package **does not bundle** the NVIDIA CUDA / cuDNN redistributable DLLs.
  - Make sure CUDA/cuDNN DLLs are discoverable at runtime (e.g., in `PATH` or alongside your app).

> If CUDA inference fails to load (e.g., DLL not found / entry point not found), the most common causes are:
> - NVIDIA driver is outdated/incompatible
> - CUDA/cuDNN versions do not match (**CUDA 11.8 + cuDNN 8.5.0.96**)
> - CUDA/cuDNN DLLs are not on `PATH` (or not deployed next to the executable)

### CUDA / cuDNN Installation Links

> Note: NVIDIA downloads may require an NVIDIA Developer account login.

- **CUDA Toolkit 11.8 (Windows x86_64)**  
  https://developer.nvidia.com/cuda-11-8-0-download-archive?target_os=Windows&target_arch=x86_64

- **cuDNN v8.5.0.96 (CUDA 11.x, Windows x86_64 zip)**  
  Option A (recommended entry): cuDNN Archive (pick **v8.5.0 for CUDA 11.x** → **Windows x86_64 (Zip)**)  
  https://developer.nvidia.com/rdp/cudnn-archive

  Option B (direct file; will redirect to NVIDIA login):  
  https://developer.nvidia.com/compute/cudnn/secure/8.5.0/local_installers/11.7/cudnn-windows-x86_64-8.5.0.96_cuda11-archive.zip

### CPU + CUDA Mixed Usage (Important)
- `HGR.Native.Cu118` and `HGR.Managed.Cu118`  can be used in a **mixed mode**:
  - You can run **CPU inference** regardless of CUDA availability.
  - To run **CUDA inference**, you must have a compatible NVIDIA driver + **CUDA 11.8** + **cuDNN 8.5.0.96** installed/configured.
  - This enables **CPU fallback** or choosing **CPU/CUDA per workload**.

---

## Development Environment

- **Visual Studio 2026**

---

## Runtime Dependency (Required)

This library requires a separate **redistribution package** to run (native runtime DLLs, etc.).
Download and install the redistribution package before using HGR.

- Microsoft Visual C++ Redistributable (Latest Supported):
  https://learn.microsoft.com/ko-kr/cpp/windows/latest-supported-vc-redist?view=msvc-170

---

## NuGet Packages

HGR is not a “single one-off release”. The NuGet packages can be **updated over time**
(bug fixes, performance improvements, new runtime variants, model upgrades).

Current / planned package list:

- **`HGR.Native.Cu118`** (Windows x64, native runtime, CPU, requires **CUDA 11.8 + cuDNN 8.5.0.96** for GPU)  
  https://www.nuget.org/packages/HGR.Native.Cu118
- **`HGR.Managed.Cuda118`** (Windows x64, managed wrapper for .NET / C#; uses the native runtime under the hood)  
  https://www.nuget.org/packages/HGR.Managed.Cuda118

> The list may expand (e.g., different CUDA versions) and existing packages may receive updates.

---

## Installation

### C++ (native)
#### Package Manager
```powershell
Install-Package HGR.Native.Cu118
```

#### .NET CLI
```bash
dotnet add package HGR.Native.Cu118
```

### .NET / C# (managed wrapper)
#### Package Manager
```powershell
Install-Package HGR.Managed.Cuda118
```

#### .NET CLI
```bash
dotnet add package HGR.Managed.Cuda118
```

---

## Usage in C++

```cpp
#include <iostream>
#include <opencv2/opencv.hpp>


#include <hgr/hgr.h>
#include <hgr/clipSampler.h>

int main()
{

	try {

        auto memoryPool = hgrapi::v1::memoryPool::create();

		auto hgr = hgrapi::v1::hgr::create();
		hgr->setup("C://github//HGR//python//results//model.onnx", hgrapi::v1::device::cuda);
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

```

## Usage in C#

```csharp
//it will update soon
```

---

## Roadmap

- [x] Provide a managed NuGet wrapper for **.NET / C#** (**`HGR.Managed.Cuda118`**)
- [ ] Improve .NET API ergonomics (more idiomatic C# surface)
- [ ] Add additional runtime variants (e.g., different CUDA versions)
- [ ] Improve low-light enhancement model quality and provide more model options/variants

---

## Research References / Acknowledgements

This project uses ideas and/or model architectures from academic research.
If you use **HGR** in research, demos, or publications, please consider **citing the original papers**.

We sincerely thank the authors and contributors of these works for advancing low-light enhancement research:

- **Zero-DCE (CVPR 2020)**  
  Chunle Guo, Chongyi Li, Jichang Guo, Chen Change Loy, Junhui Hou, Sam Kwong, Runmin Cong  
  *Zero-Reference Deep Curve Estimation for Low-Light Image Enhancement*  
  Paper (CVF Open Access): https://openaccess.thecvf.com/content_CVPR_2020/html/Guo_Zero-Reference_Deep_Curve_Estimation_for_Low-Light_Image_Enhancement_CVPR_2020_paper.html  
  Project page: https://li-chongyi.github.io/Proj_Zero-DCE.html

- **Zero-DCE++ (TPAMI 2021/2022)** *(used by this API)*  
  Chongyi Li, Chunle Guo, Chen Change Loy  
  *Learning to Enhance Low-Light Image via Zero-Reference Deep Curve Estimation*  
  arXiv: https://arxiv.org/abs/2103.00860  
  Project page: https://li-chongyi.github.io/Proj_Zero-DCE%2B%2B.html  
  DOI: https://doi.org/10.1109/TPAMI.2021.3063604

> Note: Please also comply with the licenses/terms of any upstream code, weights, and third-party libraries you use or redistribute.

---

## License

This project is licensed under the **MIT License** (for the HGR source code).

### Third-party notices (important)
This distribution may include third-party components and/or binaries.  
Those components are **NOT** covered by the MIT License and remain subject to their respective licenses/terms.

Included third-party license texts are provided under the `licenses/` folder:

- `CUDA-EULA.txt` — NVIDIA CUDA runtime components (redistributables)
- `cudnn-LICENSE.txt` — NVIDIA cuDNN runtime components
- `onnxruntime-LICENSE.txt` — ONNX Runtime license
- `onnxruntime-ThirdPartyNotices.txt` — ONNX Runtime third-party notices
- `opencv-LICENSE.txt` — OpenCV license

By using this package, you agree to comply with all applicable third-party license terms in addition to the MIT License.

---

MIT License

Copyright (c) 2025–present gellston

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
