

open OpenCvSharp

try
    let memoryPool = HGRAPI.V1.MemoryPool.Create()
    let hgr = HGRAPI.V1.HGR.Create()

    hgr.Setup(HGRAPI.V1.DLType.Ghost3D, HGRAPI.V1.Device.Cuda)
    hgr.EmaAlpha <- 0.2f

    let sampler = HGRAPI.V1.ClipSampler.Create()
    sampler.MaxFrames <- 40
    sampler.SampleFrames <- 16

    use cap = new VideoCapture(0)
    if not (cap.IsOpened()) then
        printfn "Failed to open VideoCapture (camera 0)."
        0
    else
        use frame = new Mat()

        let mutable running = true
        while running do
            if not (cap.Read(frame)) || frame.Empty() then
                printfn "Failed to read frame or empty frame."
                running <- false
            else
                // Note: frame.Data is IntPtr
                use dlImage =
                    HGRAPI.V1.Image.Create(
                        uint32 frame.Cols,
                        uint32 frame.Rows,
                        uint32 3,
                        frame.Data,
                        memoryPool
                    )

                use resizeImage = HGRAPI.V1.Image.Resize(dlImage, uint32 128, uint32 64)

                sampler.Append(resizeImage)

                let samples = sampler.RequestSampling()
                let result = hgr.Predict(samples)

                Cv2.ImShow("capture", frame)

                let key = Cv2.WaitKey(1)
                if key = 27 then
                    running <- false

                printfn "name : %s prob : %f" result.Name result.Prob

                HGRAPI.V1.ClipSampler.DisposeImages(samples)

        cap.Release()
        Cv2.DestroyAllWindows()
        0

    with ex ->
    printfn "Exception: %O" ex
    1