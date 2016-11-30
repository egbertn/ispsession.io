/*
 *    <!--xmlns:ei="http://schemas.microsoft.com/expression/2010/interactions"
             xmlns:i="http://schemas.microsoft.com/expression/2010/interactivity" -->
 * */
using ispsession.io.setup.ViewModels;

namespace ispsession.io.setup.Forms
{
    /// <summary>
    /// Interaction logic for ServerConfig.xaml
    /// </summary>
    public partial class ServerConfig 
    {
        
        public ServerConfig()
        {
            InitializeComponent();
            this.Loaded += (s, e) => ((ServerConfigViewModel)LayoutRoot.DataContext).OnLoad();
        }
    }
}