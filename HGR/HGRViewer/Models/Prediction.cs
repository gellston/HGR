using ConvMVVM2.Core.Attributes;
using ConvMVVM2.Core.MVVM;

namespace HGRViewer.Models
{
    public partial class Prediction : ViewModelBase
    {
        #region Constructor
        public Prediction()
        {

        }
        #endregion

        #region Public Property
        [Property]
        private string _Name = "";

        [Property]
        private double _Score = 0.0;
        #endregion


    }
}
