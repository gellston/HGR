using OpenCvSharp;

namespace ManagedTest
{
    internal class Program
    {
        static void Main(string[] args)
        {
            try
            {
                var memoryPool = HGRAPI.V1.MemoryPool.Create();
                var hgr = HGRAPI.V1.HGR.Create();

                hgr.Setup(HGRAPI.V1.DLType.Ghost3D, HGRAPI.V1.Device.Cuda);
                hgr.EmaAlpha = 0.2f;

                var sampler = HGRAPI.V1.ClipSampler.Create();
                sampler.MaxFrames = 40;
                sampler.SampleFrames = 16;

                using var cap = new VideoCapture(0);
                if (!cap.IsOpened())
                {
                    Console.WriteLine("Failed to open VideoCapture (camera 0).");
                    return;
                }

                using var frame = new Mat();

                while (true)
                {
                    if (!cap.Read(frame) || frame.Empty())
                    {
                        Console.WriteLine("Failed to read frame or empty frame.");
                        break;
                    }


                    using var dlImage = HGRAPI.V1.Image.Create((uint)frame.Cols, (uint)frame.Rows, 3, frame.Data, memoryPool);
                    using var resizeImage = HGRAPI.V1.Image.Resize(dlImage, 128, 64);
                   
                    sampler.Append(resizeImage);

                    var samples = sampler.RequestSampling();
                    var result = hgr.Predict(samples);

                    Cv2.ImShow("capture", frame);

                    int key = Cv2.WaitKey(1);
                    if (key == 27)
                        break;

                    System.Console.WriteLine("name : {0} prob : {1}", result.Name, result.Prob);

                    HGRAPI.V1.ClipSampler.DisposeImages(samples);
                }

                cap.Release();
                Cv2.DestroyAllWindows();
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Exception: {ex}");
            }
        }
    }
}
