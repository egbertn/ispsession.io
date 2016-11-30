using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using ispsession.Configurator.DAL.Interop;
using ispsession.io.setup.PRISM;
using ispsession.io.setup.Properties;
using ispsession.io.setup.Stuff;
using ispsession.io.setup.util;

namespace ispsession.io.setup.ViewModels
{
    public sealed class ConfigDemoViewModel: NotificationObject
    {
        public RelayCommand<Button> CommandCreate {get; private set;}
        Settings _settings;
        public ConfigDemoViewModel()
        {
            CommandCreate = new RelayCommand<Button>(OnCreate, CanCreate);
            Sites = new Collection<Site>();
            AppPools = new Collection<string>();
        }
        public void OnLoad()
        {

            _settings = new Settings();
            Sites =  new Collection<Site>(IIS.Getsites.ToArray());
            AppPools = new Collection<string>( IIS.getAppools);
  
        }
        protected override void AfterPropertyUpdate(string name)
        {
            _canCreate = true;
            CommandCreate.RaiseCanExecuteChanged();

        }
        public string Title
        {
            get { return "ISP Session Demo Creator"; }
        }
        public IList<string> DemoList
        {
            get
            {
                return new Collection<string>(
                    new [] { "CookieWeb" });
            }
        }

        private bool _canCreate = false;
        public bool CanCreate()
        {
            return _canCreate;
        }
        private ICollection<string> _appPools;
        public ICollection<string> AppPools
        {
            get
            {
                return _appPools;
            }

            private set
            {
                _appPools = value;
                RaisePropertyChanged(() => AppPools);
            }
        }
        private int _selectedAppPoolIndex;
        public int SelectedAppPoolIndex
        {
            get
            {
                return _selectedAppPoolIndex;
            }
            set
            {
                if (value != _selectedAppPoolIndex)
                {
                    _selectedAppPoolIndex = value;
                    RaisePropertyChanged(()=>SelectedAppPoolIndex);
                }
            }
        }

        private int _selectedSiteIndex;
        public int SelectedSiteIndex
        {
            get { return _selectedSiteIndex; }
            set
            {
                if (value != _selectedSiteIndex)
                {
                    _selectedSiteIndex = value;
                    RaisePropertyChanged(() => SelectedSiteIndex);
                }
            }
        }
        private string _appPoolText;
        public string AppPoolText
        {
            get { return _appPoolText; }
            set
            {
                if (value != _appPoolText)
                {
                    _appPoolText = value;
                    RaisePropertyChanged(() => AppPoolText);
                }
            }
        }
        private Collection<Site> _sites;
        public Collection<Site> Sites
        {
            get
            {
                return _sites;
            }
            set
            {
                if (value != _sites)
                {
                    _sites = value;
                    RaisePropertyChanged( () => Sites);
                }
            }
        }
        private void OnCreate(Button sender)
        {
            var lst = Sites[SelectedSiteIndex];
            string targetInstance = lst.Instance.ToString();
            string siteToDoAction = DemoList[SelectedDemoIndex];
            string target = Path.Combine(IIS.RootPath(targetInstance), siteToDoAction);
            if (!ButtonText.Contains("Remove"))
            {
                string source = Path.Combine(AppInfo.CurrentPath + "\\demo", siteToDoAction);
                bool result = Installer.CopyAll(source, target);
                var pDir = IIS.CreateIISVDir(targetInstance, siteToDoAction, AppPoolText, target, siteToDoAction);
                ButtonText = ButtonText.Replace("Create", "Remove");
            }
            else
            {
              //  Cursor.Current = Cursors.WaitCursor;

                try
                {
                    IIS.RemoveIISVdir(targetInstance, siteToDoAction);

                    try
                    {
                        Directory.Delete(target, true);
                    }
                    //ignore
                    catch
                    {
                    }
                    //_settings.WebInstalled = false;
                    AppPoolEnabled = true;

                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message, AppInfo.AssemblyTitle);
                }
                //Cursor.Current = Cursors.Default;
                ButtonText = ButtonText.Replace("Remove", "Create");
                MessageBox.Show(string.Format("{0} editor removed", siteToDoAction), AppInfo.AssemblyTitle, MessageBoxButton.OK);
                //btnConnectWeb.Enabled = false;
            }

        }
        private string _buttonText = "Create";
        public string ButtonText { get { return _buttonText; }
            set
            {
                _buttonText = value;
                RaisePropertyChanged(()=>ButtonText);
                
            }
        }

        private int _selectedDemoIndex;
        public int SelectedDemoIndex
        {
            get { return _selectedDemoIndex; }
            set
            {
                if (value != _selectedDemoIndex)
                {
                    _selectedDemoIndex = value;
                    CheckButtonText();
                    RaisePropertyChanged(()=>SelectedDemoIndex);
                }
            }
        }

        private void CheckButtonText()
        {
             var lst = Sites[SelectedSiteIndex];
            string targetInstance = lst.Instance.ToString();
            string siteToDoAction = DemoList[SelectedDemoIndex];

            ButtonText = IIS.IISVdirExists(targetInstance, siteToDoAction) ? "Remove" : "Create";
        }

        private bool _AppPoolEnabled;
        public bool AppPoolEnabled
        {
            get { return _AppPoolEnabled; }
            set
            {
                _AppPoolEnabled = value;
                RaisePropertyChanged(()=>AppPoolEnabled);
            }
        }
 
    }
}
