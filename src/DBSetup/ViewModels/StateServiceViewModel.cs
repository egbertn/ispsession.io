using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Security.Principal;
using System.ServiceProcess;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using ispsession.Configurator.Config;
using ispsession.Configurator.DAL;
using ispsession.io.setup.PRISM;
using ispsession.io.setup.Stuff;
using ispsession.io.setup.util;

namespace ispsession.io.setup.ViewModels
{
    public class StateServiceViewModel : NotificationObject
    {
        private OtherConfig _settings;
        private ServiceController _service;

        public StateServiceViewModel()
        {
            //StartupType = StartupType<System.ServiceProcess.ServiceStartMode>.StartupTypes;
            //NotifyPropertyChanged("StartupType");
            CmdStart = new RelayCommand<Button>(Start, CanClickStart);
            CmdStop = new RelayCommand<Button>(Stop, CanClickStop);
            //CmdBrowserFolder = new RelayCommand<object>(BrowserFolder);

            ApplyCommand = new RelayCommand<object>(OnApply, CanApply);
            //TODO:fix
            SelectedServiceStartMode = new StartupType<ServiceStartMode>();
            ;
            CmdConnect = new RelayCommand<object>(OnCmdConnect);
        }

        public string Title
        {
            get { return "ISP Session COM Garbage collection settings"; }
        }

        private bool _canApply;

        private bool CanApply()
        {
            return _canApply;
        }
        protected override void AfterPropertyUpdate(string name)
        {
            _canApply = true; 
            ApplyCommand.RaiseCanExecuteChanged();
            
        }
        private void OnApply(object param)
        {
            // _settings.Set("SessionStoragePath", StoragePath);
            _settings.Set("ASPSessionConnString", ConnectionString);
            _settings.Set("CollectTimer", "10000");
            _settings.Set("CollectTimer", (CollectionIntervalSeconds*1000).ToString());
            _settings.Set("disableStats", "false");
            _settings.Set("EnableLogging", "0");
            _settings.Set("KeepConnection", "true");

            _settings.Save();
            var sUser = UserAccount;
            var sPassword = param != null ? ((PasswordBox) param).Password : null;

            if (string.IsNullOrEmpty(sUser))
            {
                sUser = null;
            }
            if (string.IsNullOrEmpty(sPassword))
            {
                sPassword = null;
            }
            NTAccount acc = null;
            if (!IsNetworkServiceChecked)
            {
                try
                {
                    acc = new NTAccount(sUser);
                    acc.SetRunasServicePolicy(false);
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message, AppInfo.AssemblyTitle);
                }
            }
            else
            {
                acc = NtSecurityInterop.NetworkService;
                sPassword = null;
            }
            //TODO DESCRIPTION
            if (acc != null)
                try
                {
                    _service.SetServiceLogin(acc, sPassword, null);
                    _service.SetServiceStartupMode(IsManualChecked
                        ? ServiceStartMode.Manual
                        : ServiceStartMode.Automatic);
                }
                catch (Exception ex)
                {

                    MessageBox.Show(ex.Message, AppInfo.AssemblyTitle);
                }
            sPassword = null;

            _canApply = false;
            ApplyCommand.RaiseCanExecuteChanged();


        }

        private string _userPassword;

        public string UserPassword
        {
            get { return _userPassword; }
            set
            {

                if (value != UserAccount)
                {
                    _userPassword = value;
                    RaisePropertyChanged(() => UserPassword);
                }
            }
        }

        private string _userAccount;

        public string UserAccount
        {
            get { return _userAccount; }
            set
            {

                if (value != UserAccount)
                {
                    _userAccount = value;
                    RaisePropertyChanged(() => UserAccount);
                }
            }
        }

        private void OnServiceChange(IntPtr data)
        {
            if (_service.Status == ServiceControllerStatus.Running)
            {
                _CanClickStart = false;
                _CanClickStop = true;
            }
            else
            {
                _CanClickStart = true;
                _CanClickStop = false;
            }
        }

        public void OnLoad()
        {
            //default
            IsNetworkServiceChecked = true;
            try
            {
                if (!AppInfo.IsInDesignMode)
                {
                    _settings = new OtherConfig("Service\\ASPSession.exe", "appSettings");
                    ConnectionString = _settings.Get("ASPSessionConnString",
                        @"Provider=SQLOLEDB;User id=;password=***;data source=(local)\SQLEXPRESS");
                    _service = new ServiceController(Installer.ISPSESSIONSERVICE);
                    var login = _service.GetServiceLogin();
                    IsNetworkServiceChecked = login == NtSecurityInterop.NetworkService;
                    var startupMode = _service.GetServiceStartupMode();
                    IsManualChecked = startupMode == ServiceStartMode.Manual;
                    IsNotManualChecked = startupMode == ServiceStartMode.Automatic;
                    UserAccount = login.Value;
                    CollectionIntervalSeconds = _settings.Get("CollectTimer", 60000)/1000;
                    _canApply = false;
                    ApplyCommand.RaiseCanExecuteChanged();

                }
            }
            catch // no service installed
            {

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

        private string _dbName;

        public string DbName
        {
            get { return _dbName; }
            set
            {
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

        private bool _isNotManualChecked;

        public bool IsNotManualChecked
        {
            get { return _isNotManualChecked; }
            set
            {
                if (value != _isNotManualChecked)
                {
                    _isNotManualChecked = value;
                    RaisePropertyChanged(() => IsNotManualChecked);
                }
            }
        }

        private bool _isManualChecked;

        public bool IsManualChecked
        {
            get { return _isManualChecked; }
            set
            {
                if (value != _isManualChecked)
                {
                    _isManualChecked = value;
                    RaisePropertyChanged(() => IsManualChecked);
                }
            }
        }

        public Collection<StartupType<ServiceStartMode>> StartupType { get; private set; }
        private StartupType<ServiceStartMode> _serviceStartMode;

        public StartupType<ServiceStartMode> SelectedServiceStartMode
        {
            get { return _serviceStartMode; }
            set
            {
                if (_serviceStartMode != value)
                {
                    _serviceStartMode = value;
                    RaisePropertyChanged(() => SelectedServiceStartMode);
                }
            }
        }

        public RelayCommand<RadioButton> CmdIsNetworkServiceChecked { get; private set; }
        public RelayCommand<RadioButton> CmdIsUserAccountChecked { get; private set; }

        public RelayCommand<object> CmdBrowserFolder { get; private set; }
        public RelayCommand<object> ApplyCommand { get; private set; }
        public RelayCommand<Button> CmdStart { get; private set; }
        public RelayCommand<Button> CmdStop { get; private set; }
        private bool _CanClickStop;
        private bool _CanClickStart;

        private bool CanClickStart()
        {
            return _CanClickStart;
        }

        private bool CanClickStop()
        {
            return _CanClickStop;
        }

        private void Start(Button e)
        {
            try
            {
                _service.Start();
                _service.WaitForStatus(ServiceControllerStatus.Running, TimeSpan.FromSeconds(10));
                _CanClickStop = true;
                _CanClickStart = false;
                CmdStart.RaiseCanExecuteChanged();
                CmdStop.RaiseCanExecuteChanged();
            }
            catch
            {
            }

        }

        private ServiceControllerStatus _serviceStatus;

        public ServiceControllerStatus ServiceStatus
        {
            get { return _serviceStatus; }
            set
            {
                if (value != _serviceStatus)
                {
                    _serviceStatus = value;
                    RaisePropertyChanged(() => ServiceStatus);
                }
            }
        }

        private void Stop(Button e)
        {
            _service.Stop();
            _service.WaitForStatus(ServiceControllerStatus.Stopped, TimeSpan.FromSeconds(10));
            _CanClickStop = false;
            _CanClickStart = true;

        }


        private bool _notIsNetworkServiceChecked;

        public bool NotIsNetworkServiceChecked
        {
            get { return _notIsNetworkServiceChecked; }
            set
            {
                _notIsNetworkServiceChecked = value;
                RaisePropertyChanged(() => NotIsNetworkServiceChecked);
            }
        }

        private bool _isNetworkServiceChecked;

        public bool IsNetworkServiceChecked
        {
            get { return _isNetworkServiceChecked; }
            set
            {
                if (value != _isNetworkServiceChecked)
                {
                    _isNetworkServiceChecked = value;
                    NotIsNetworkServiceChecked = !value;
                    RaisePropertyChanged(() => IsNetworkServiceChecked);
                }
            }
        }


        private int _collectionIntervalSeconds;

        public int CollectionIntervalSeconds
        {
            get { return _collectionIntervalSeconds; }
            set
            {
                if (_collectionIntervalSeconds != value)
                {
                    _collectionIntervalSeconds = value;
                    RaisePropertyChanged(() => CollectionIntervalSeconds);
                }
            }
        }
    }
}