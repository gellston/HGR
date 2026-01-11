using ConvMVVM2.Core.Attributes;
using ConvMVVM2.Core.MVVM;
using HGRViewer.Models;
using HGRViewer.Services;
using HGRViewer.Utils;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media.Imaging;

namespace HGRViewer.ViewModels
{
    public partial class MainViewModel : ViewModelBase
    {

        #region Private Property
        private readonly IHGRService hgrService;

        private readonly string[] Gestures =
        {
            "Doing other things","No gesture","Drumming Fingers","Pulling Hand In","Pulling Two Fingers In",
            "Pushing Hand Away","Pushing Two Fingers Away","Rolling Hand Backward","Rolling Hand Forward",
            "Shaking Hand","Sliding Two Fingers Down","Sliding Two Fingers Left","Sliding Two Fingers Right",
            "Sliding Two Fingers Up","Stop Sign","Swiping Down","Swiping Left","Swiping Right","Swiping Up",
            "Thumb Down","Thumb Up","Turning Hand Clockwise","Turning Hand Counterclockwise",
            "Zooming In With Full Hand","Zooming In With Two Fingers","Zooming Out With Full Hand",
            "Zooming Out With Two Fingers"
        };
        #endregion

        #region Constructor
        public MainViewModel(IHGRService hgrService)
        {


            foreach(var gesture in Gestures)
            {
                this.PredictionCollection.Add(new Prediction()
                {
                    Name = gesture,
                    Score = 0.0
                });
            }


            this.hgrService = hgrService;

            this.hgrService.OnShutdown += HgrService_OnShutdown;
            this.hgrService.OnSetup += HgrService_OnSetup;

            this.hgrService.OnCapture += HgrService_OnCapture;
            this.hgrService.OnPlaying += HgrService_OnPlaying;
            this.hgrService.OnStop += HgrService_OnStop;
        }



        #endregion




        #region Public Property
        [Property]
        private string _SelectedDevice = "CPU";

        [Property]
        private bool _IsInitialized = false;

        [Property]
        private WriteableBitmap _CurrentFrame = null;

        [Property]
        private bool _IsPlaying = false;

        [Property]
        private bool _IsZooming = false;
        #endregion

        #region Collection
        [Property]
        private ObservableCollection<string> _DeviceCollection = new ObservableCollection<string>()
        {
            "CPU", "CUDA"
        };

        [Property]
        private ObservableCollection<Prediction> _PredictionCollection = new ObservableCollection<Prediction>();
        #endregion

        #region Command

        [RelayCommand]
        private void Setup()
        {
            try
            {
                this.hgrService.Setup(this.SelectedDevice);

            }
            catch (Exception ex)
            {

            }
        }

        [RelayCommand]
        private void Shutdown()
        {
            try
            {

                this.hgrService.Shutdown();

            }
            catch (Exception ex)
            {

            }
        }

        [RelayCommand]
        private void Play()
        {
            try
            {

                this.hgrService.StartPlay();

            }
            catch (Exception ex)
            {

            }
        }


        [RelayCommand]
        private void Stop()
        {
            try
            {
                this.hgrService.StopPlay();

            }catch(Exception ex)
            {

            }
        }
        #endregion

        #region Event Handler
        private void HgrService_OnSetup()
        {

            this.IsInitialized = true;
        }

        private void HgrService_OnShutdown()
        {
            this.IsInitialized = false;
        }


        private void HgrService_OnStop()
        {

            this.IsPlaying = false;
        }

        private void HgrService_OnPlaying()
        {

            this.IsPlaying = true;
        }

        private void HgrService_OnCapture(HGRAPI.V1.Image image, HGRAPI.V1.Result result)
        {
            try
            {

                if(this.CurrentFrame == null || 
                   this.CurrentFrame.Width != image.Width ||
                   this.CurrentFrame.Height != image.Height || 
                   this.CurrentFrame.Width * 3 != image.Stride)
                {
                    this.CurrentFrame = ImageHelper.CreateFromRawPointer(image.Data, (int)image.Width, (int)image.Height, (int)image.Stride);
                }
                else
                {
                    ImageHelper.UpdatePixelsSameSize(this.CurrentFrame, image.Data, (int)image.Width, (int)image.Height, (int)image.Stride);
                }

                for (int index = 0; index < result.Probs.Count; index++)
                {
                    this.PredictionCollection[index].Score = result.Probs[index] * 100;
                }

                
                if(result.Name == "Sliding Two Fingers Left" || result.Name == "Swiping Left")
                {
                    this.IsZooming = false;
                }
                if (result.Name == "Sliding Two Fingers Right" || result.Name == "Swiping Right")
                {
                    this.IsZooming = true;
                }
                

               

            }
            catch (Exception ex)
            {


            }
            finally
            {
                image.Dispose();
            }

        }

        #endregion
    }
}
