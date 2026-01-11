using ConvMVVM2.Core.MVVM;
using HGRViewer.Services;
using HGRViewer.ViewModels;
using HGRViewer.Views;
using HGRViewer.Windows;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HGRViewer
{
    internal class BootStraper : ConvMVVM2.Core.MVVM.AppBootstrapper
    {
        protected override void OnStartUp(IServiceContainer container)
        {

        }

        protected override void RegionMapping(IRegionManager layerManager)
        {

            layerManager.Mapping<MainView>("MainView");
        }

        protected override void RegisterModules()
        {

        }

        protected override void RegisterServices(IServiceCollection serviceCollection)
        {

            // Windows
            serviceCollection.AddSingleton<MainWindow>();

            //Views
            serviceCollection.AddSingleton<MainView>();

            //ViewModels
            serviceCollection.AddSingleton<MainViewModel>();


            //Services
            serviceCollection.AddSingleton<IHGRService, HGRService>();

        }

        protected override void ViewModelMapping(IViewModelMapper viewModelMapper)
        {

        }
    }
}
