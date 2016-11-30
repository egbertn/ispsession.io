using System.Windows;
using System.Diagnostics;
using ispsession.io.setup.Forms;

namespace ispsession.io.setup
{

    public partial class App
    {
        protected override void OnStartup(StartupEventArgs e)
        {
            if (e.Args.Length > 0)
            {
                //var frm = new FormDbCreate();

                //var result = frm.ShowDialog();
                Trace.TraceInformation("Application Startup With Params {0}", string.Join(";", e.Args));
                // the old window
                //if (result == true)
                    Installer.Install(e.Args[0], e.Args.Length > 1 ? int.Parse(e.Args[1]) : (int) 0);
              
            }
            else
            {
                var m = new MainWindow();
                m.ShowDialog();
            }
            Shutdown();
        }
    }
}