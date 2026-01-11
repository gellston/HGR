using HGRAPI.V1;
using OpenCvSharp;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Threading;

namespace HGRViewer.Services
{
    public sealed class HGRService : IHGRService, IDisposable
    {

        #region Private Property
        // ===== fixed defaults (WPF viewer defaults) =====
        private readonly int _cameraIndex = 0;
        private readonly int _resizeW = 128;
        private readonly int _resizeH = 64;
        private readonly int _maxFrames = 32;
        private readonly int _sampleFrames = 16;
        private readonly float _emaAlpha = 0.2f;

        // ===== runtime =====
        private readonly object _sync = new();
        private readonly Dispatcher _dispatcher;

        private MemoryPool? _memoryPool;
        private HGR? _hgr;
        private ClipSampler? _sampler;

        private Thread? _worker;
        private volatile bool _stopRequested;

        // 0 = stopped, 1 = playing (guard)
        private int _playGuard = 0;
        private int _setupGuard = 0;

        #endregion

        #region Event
        // ===== events =====
        public event Action? OnSetup;
        public event Action? OnShutdown;

        public event Action? OnPlaying;
        public event Action? OnStop;

        public event Action<Image, Result>? OnCapture;
        #endregion

        #region Constructor
        public HGRService()
        {
            // WPF 전용: UI Dispatcher를 내부에서 획득
            _dispatcher = Application.Current?.Dispatcher
                          ?? throw new InvalidOperationException("WPF Dispatcher not available. Ensure Application.Current exists.");
        }
        #endregion



        #region Private Functions

        // =========================
        // Helpers
        // =========================
        private static Device ParseDevice(string device)
        {
            if (string.IsNullOrWhiteSpace(device))
                return Device.Cpu;

            var d = device.Trim().ToLowerInvariant();
            if (d.Contains("CUDA"))
                return Device.Cuda;

            return Device.Cpu;
        }

        private void RaiseOnUI(Action action)
        {
            if (_dispatcher.CheckAccess())
            {
                action();
                return;
            }

            _dispatcher.Invoke(action);
        }
        #endregion


        #region Public Functions
        public void Setup(string device)
        {
            // 중복 Setup 방지
            if (Interlocked.CompareExchange(ref _setupGuard, 1, 0) != 0)
                return;

            try
            {
                lock (_sync)
                {
                    _memoryPool = MemoryPool.Create();
                    _hgr = HGR.Create();

                    var dev = ParseDevice(device);
                    _hgr.Setup(DLType.Ghost3D, dev);
                    _hgr.EmaAlpha = _emaAlpha;

                    _sampler = ClipSampler.Create();
                    _sampler.MaxFrames = _maxFrames;
                    _sampler.SampleFrames = _sampleFrames;
                }

                RaiseOnUI(() => OnSetup?.Invoke());
            }
            catch
            {
                Interlocked.Exchange(ref _setupGuard, 0);
                throw;
            }
        }

        public void StartPlay()
        {
            // 중복 StartPlay 방지
            if (Interlocked.CompareExchange(ref _playGuard, 1, 0) != 0)
                return;

            lock (_sync)
            {
                if (_hgr is null || _sampler is null || _memoryPool is null)
                {
                    Interlocked.Exchange(ref _playGuard, 0);
                    throw new InvalidOperationException("HGRService is not set up. Call Setup() first.");
                }

                _stopRequested = false;

                _worker = new Thread(WorkerLoop)
                {
                    IsBackground = true,
                    Name = "HGRService.PlayThread"
                };
                _worker.Start();
            }

            RaiseOnUI(() => OnPlaying?.Invoke());
        }

        public void StopPlay()
        {
            if (Interlocked.Exchange(ref _playGuard, 0) == 0)
                return;

            _stopRequested = true;

            Thread? t;
            lock (_sync) { t = _worker; }

            if (t is not null && t.IsAlive)
                t.Join(1500);

            RaiseOnUI(() => OnStop?.Invoke());
        }

        public void Shutdown()
        {
            StopPlay();

            lock (_sync)
            {
                _sampler?.Dispose();
                _sampler = null;

                _hgr?.Dispose();
                _hgr = null;

                _memoryPool?.Dispose();
                _memoryPool = null;

                _worker = null;
                _stopRequested = false;

                Interlocked.Exchange(ref _setupGuard, 0);
            }

            RaiseOnUI(() => OnShutdown?.Invoke());
        }

        public void Dispose()
        {
            Shutdown();
        }

        // =========================
        // Worker
        // =========================
        private void WorkerLoop()
        {
            try
            {
                MemoryPool memoryPool;
                HGR hgr;
                ClipSampler sampler;

                lock (_sync)
                {
                    memoryPool = _memoryPool!;
                    hgr = _hgr!;
                    sampler = _sampler!;
                }

                using var cap = new VideoCapture(_cameraIndex);
                if (!cap.IsOpened())
                    return;

                using var frame = new Mat();

                while (!_stopRequested)
                {
                    ///Thread.Sleep(1);
                    if (!cap.Read(frame) || frame.Empty())
                        continue;

                    try
                    {

                        using var dlImage = Image.Create((uint)frame.Cols, (uint)frame.Rows, 3, frame.Data, memoryPool);
                        using var resizeImage = Image.Resize(dlImage, (uint)_resizeW, (uint)_resizeH);

                        sampler.Append(resizeImage);

                        var samples = sampler.RequestSampling();
                        try
                        {
                            var result = hgr.Predict(samples);

                            // 이벤트는 UI 스레드에서 발생
                            RaiseOnUI(() => OnCapture?.Invoke(dlImage, result));
                        }
                        finally
                        {
                            ClipSampler.DisposeImages(samples);
                        }
                    }
                    finally
                    {

                    }
                }
            }
            catch
            {
                // 필요하면 로깅/OnError 이벤트 추가
            }
            finally
            {
                Interlocked.Exchange(ref _playGuard, 0);
                RaiseOnUI(() => OnStop?.Invoke());
            }
        }

        #endregion



    }
}
