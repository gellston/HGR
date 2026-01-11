using HGRAPI.V1;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HGRViewer.Services
{
    public interface IHGRService
    {
        #region Public Functions
        public void StartPlay();
        public void StopPlay();
        public void Setup(string device);
        public void Shutdown();
        #endregion

        #region Event 
        public event Action OnSetup;
        public event Action OnShutdown;

        public event Action OnPlaying;
        public event Action OnStop;

        public event Action<Image, Result> OnCapture;
        #endregion
    }
}
