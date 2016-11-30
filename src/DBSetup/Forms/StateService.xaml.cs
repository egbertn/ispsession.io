using ADCCure.ISPSession.setup.ViewModels;

namespace ADCCure.ISPSession.setup.Forms
{
    /// <summary>
    /// Interaction logic for StateService.xaml
    /// </summary>
    public partial class StateService 
    {
        
        public StateService()
        {
            InitializeComponent();
            Loaded += (o, e) => ((StateServiceViewModel) LayoutRoot.DataContext).OnLoad();
        }
    }
}
