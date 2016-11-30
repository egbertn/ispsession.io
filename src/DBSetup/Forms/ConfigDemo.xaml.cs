using System.Windows;
using ispsession.io.setup.ViewModels;

namespace ispsession.io.setup.Forms
{
    /// <summary>
    /// Interaction logic for ConfigDemo.xaml
    /// </summary>
    public partial class ConfigDemo 
    {
        public ConfigDemo()
        {
            InitializeComponent();
            Loaded += (sender, args) => ((ConfigDemoViewModel)LayoutRoot.DataContext).OnLoad();
        }
    }
}
