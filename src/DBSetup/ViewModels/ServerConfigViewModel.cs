using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using ispsession.Configurator.DAL;
using ispsession.Configurator.DAL.Interop;
using ispsession.io.setup.PRISM;
using ispsession.io.setup.Stuff;
using ispsession.io.setup.util;

namespace ispsession.io.setup.ViewModels
{
    public sealed class ServerConfigViewModel : NotificationObject
    {

        //public ICommand SelectionChangedCommand { get; private set; }
        private OtherConfig _settings;
        public ServerConfigViewModel()
        {
            Servers = new ObservableCollection<Server>();
         //   ServerEnum.OnNewServer += OnNewServerEvent;
            
            //Dispatcher.CurrentDispatcher.BeginInvoke(new System.Action<NetServerTypeEnum>(ServerEnum.EnumServers), NetServerTypeEnum.SV_TYPE_SQLSERVER);
            _logging = new Collection<string>
            {
                "Disabled",
                Environment.ExpandEnvironmentVariables("To %windir%\\Temp"),
                "To OutputDebugString",
                "Both"
            };
            _CurrentLoggingItem = 0;
            DefaultSessionTimeOut = 20;
            ApplyCommand = new RelayCommand<Button>(SaveSetting, CanClickApply);
            CmdConnect = new RelayCommand<object>(OnCmdConnect);
            //SelectionChangedCommand = new RelayCommand<ComboBox>(OnSelectionChanged);
             
        }
        public string Title
        {
            get { return "ISP Session COM Client settings"; }
        }
      

        public void OnLoad()
        {
            //string file = Path.Combine(Path.Combine(AppInfo.CurrentPath, "Service"), "ASPSession.exe");
            //MessageBox.Show(file);
            try
            {
                if (!AppInfo.IsInDesignMode)
                {
                    _settings = new OtherConfig("CSession.Dll", "appSettings");

                    License = _settings.Get<string>("License");
                    DefaultSessionTimeOut = _settings.Get("SessionTimeout", 20);
                    CurrentLoggingItem = _settings.Get("EnableLogging", 0);
                    DisableStats = _settings.Get("disableStats", false);
                                        SyncNativeSession = _settings.Get("SyncNativeSession", false);
                    ConnectionString = _settings.Get<string>("ASPSessionConnStringUsr", "localhost:6376");
                    CSessionLIC = _settings.Get<string>("Csession.LIC");
                    if (!string.IsNullOrEmpty(CSessionLIC))
                    {
                        CSessionLIC= CSessionLIC.Replace(Environment.NewLine,  " ");
                    }
                    _canApply = false;
                    ApplyCommand.RaiseCanExecuteChanged();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Settings in CSession.DLL.Config  seem to be invalid. Please review. (message was (" + ex.Message + ")");
                
            }

        }
        private void SaveSetting(Button e)
        {
          
            _settings.Set("ASPSessionConnStringUsr", ConnectionString);
            _settings.Set("SessionTimeout", DefaultSessionTimeOut.ToString());            
            _settings.Set("License", License);
            _settings.Set("disableStats", DisableStats? "true" : "false");
            _settings.Set("Csession.LIC", CSessionLIC.Replace(" ", "\r\n"));
            _settings.Set("EnableLogging", CurrentLoggingItem.ToString());
            if (!ValidateInput())
            {
            }
            try
            {
                _settings.Save();
                _canApply = false;
                ApplyCommand.RaiseCanExecuteChanged();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, AppInfo.AssemblyTitle);
            }
        }
        
        
        public ICommand CmdConnect { get; private set; }
        private void OnCmdConnect(object e)
        {
            var connect = new DbConnect();
            connect.ConnectionSuccessEvent += (connection, dbName, save) =>
                {
                    if (save)
                    {
                        DbName = dbName;
                        ConnectionString = connection;
                        
                    }
                };
            connect.Database = DbName;
            connect.ConnectionString = ConnectionString;
           
            connect.Show();
        }
        private bool _canApply = false; //TEMP
        private bool CanClickApply()
        {
            return _canApply;
        }
        /// <summary>
        /// validates the values to be valid ones 
        /// </summary>
        /// <returns>true if valid</returns>
        private bool ValidateInput()
        {
            // valid GUID
            // valid state server
            // note about logging (message box)
            return true;
        }

        private void OnNewServerEvent(ServerArgs e)
        {
            if (e.Servers != null)
                Servers.Clear();
                foreach (var s in e.Servers)
                {
                    Servers.Add(s);
                }
        }

        public ObservableCollection<Server> Servers { get; private set; }
        

        //private void UpdateServer(string newServer)
        //{
        //    var props = DbCreator.ConnectionStringToProps(_connectionString);
        //    props["Data Source"] = newServer;
        //    ConnectionString = DbCreator.ConnectionStringFromProps(props, true);
        //}
        
        private string _dbName;
        public string DbName
        {
            get { return _dbName; }
            set {
                if (value != _dbName)
                {
                    _dbName = value;
                    RaisePropertyChanged(() => DbName);
                }
            }
        }
        private string _connectionString;
        public string ConnectionString
        {
            get { return _connectionString; }
            set
            {
                if (_connectionString != value)
                {
                    _connectionString = value;
                    RaisePropertyChanged(() => ConnectionString);
                }
            }
        }

        private bool _syncNativeSession;

        public bool SyncNativeSession
        {
            get { return _syncNativeSession; }
            set
            {
                if (value != _syncNativeSession)
                {
                    _syncNativeSession = value;
                    RaisePropertyChanged(()=> SyncNativeSession);
                }
            }
        }
        private string _license;
        public string License
        {
            get
            {
                return _license;
            }
            set
            {
                if (_license != value)
                {
                    _license = value;
                    RaisePropertyChanged(() => License);
                }
            }
        }

        readonly IList<string> _logging;
        public IList<string> Logging
        {
            get
            {
                return _logging;
            }
        }

        private bool? _disableStats;
        public bool DisableStats
        {
            get { return _disableStats == true; }
            set
            {
                if (_disableStats != value)
                {
                    _disableStats = value;
                    RaisePropertyChanged(()=> DisableStats);
                }
            }
        }

        private string _cSessionLIC;
        public string CSessionLIC
        {
            get { return _cSessionLIC; }
            set
            {
                if (value != _cSessionLIC)
                {
                    _cSessionLIC = value;
                    RaisePropertyChanged(()=>CSessionLIC);
                }
            }
        }

        private int? _CurrentLoggingItem;
        public int CurrentLoggingItem
        {
            get
            {
                return _CurrentLoggingItem.GetValueOrDefault(0);
            }
            set
            {
                if (_CurrentLoggingItem != null && _CurrentLoggingItem != value)
                {
                    _CurrentLoggingItem = value;
                    RaisePropertyChanged(()=> CurrentLoggingItem);
                }
            }
        }

        private bool _numericSession;
        public bool NumericSession
        {
            get { return _numericSession; }
            set
            {
                if (value != _numericSession)
                {
                    _numericSession = value;
                    RaisePropertyChanged(()=>NumericSession);
                }
            }
        }
        
        private int _defaultSessionTimeOut;
        public int DefaultSessionTimeOut
        {
            get
            {
                return _defaultSessionTimeOut;
            }
            set
            {
                if (value != _defaultSessionTimeOut)
                {
                    _defaultSessionTimeOut = value;
                    RaisePropertyChanged(()=>DefaultSessionTimeOut);
                }
            }
        }
        public RelayCommand<Button> ApplyCommand
        {
            get;
            private set;
        }

        protected override void AfterPropertyUpdate(string name)
        {
            _canApply = true;
            ApplyCommand.RaiseCanExecuteChanged();
        }
    }
}