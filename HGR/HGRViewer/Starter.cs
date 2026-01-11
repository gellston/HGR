using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace HGRViewer
{
    public class Starter
    {

        #region Static Functions

        [STAThread]
        public static void Main(string[] args)
        {

            var host = ConvMVVM2.WPF.Host.ConvMVVM2Host.CreateHost<BootStraper, Application>(args, "App");
            host.ShutdownMode(ShutdownMode.OnMainWindowClose)
                .Build()
                .RunApp("MainWindow");

        }
        #endregion
    }
}
