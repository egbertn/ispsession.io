using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml.Serialization;
using System.IO;
using System.ComponentModel;
using System.Net;
using System.Net.Sockets;
using System.Net.NetworkInformation;
using iSOFT.Configurator.DAL.Interop;
using System.ServiceProcess;
using iSOFT.TaskSchedulerInterop;
using iSOFT.TaskScheduler;

namespace iSOFT.Configurator.DAL
{
    [XmlRoot("installable", Namespace = "http://iSOFT.nl/installables")]
    public sealed class Installables
    {
        /// <summary>
        /// just an internal
        /// </summary>
        private string ConfigFile { get; set; }
        /// <summary>
        /// Reads config xml file and returns the instance
        /// </summary>
        /// <param name="configFile"></param>
        /// <returns></returns>
        public static Installables Load(string configFile)
        {
            XmlSerializer ser = new XmlSerializer(typeof(Installables));
            using (var file = File.OpenRead(configFile))
            {
                var inst = (Installables)ser.Deserialize(file);
                inst.ConfigFile = configFile;
                return inst;
            }
        }
        /// <summary>
        /// saves this intance back to XML having all modifications
        /// </summary>
        public void Save(string configFile)
        {
            if (util.FileIsWritable(ConfigFile))
            {
                XmlSerializer ser = new XmlSerializer(typeof(Installables));
                using (var file = File.OpenWrite(this.ConfigFile))
                {
                    file.SetLength(0);
                    ser.Serialize(file, this);
                }
            }
        }

        private List<Database> m_Databases;
        private List<Service> m_Services;
        private List<Application> m_Applications;
        private ServiceAccount m_ServiceAccount;

        public Installables()
        {
            m_Databases = new List<Database>();
            m_Services = new List<Service>();
            m_Applications = new List<Application>();
        }
        [XmlArray("appDependencies")]
        public List<AppDependencies> AppDependency { get; set; }

        /// <summary>
        /// the name of the iSOFT Solution Suite e.g. XDS
        /// </summary>
        [XmlElement("suiteName")]
        public string SuiteName { get; set; }


        /// <summary>
        /// defines a service account shared by all windows services
        /// </summary>
        [XmlElement("serviceAccount")]
        public ServiceAccount ServiceAccount
        {
            get
            {
                if (m_ServiceAccount == null) m_ServiceAccount = new ServiceAccount();
                return m_ServiceAccount;
            }
            set
            {
                m_ServiceAccount = value;
            }
        }
        [XmlArray("applications")]
        public List<Application> Applications
        {
            get { return m_Applications; }
            set { m_Applications = value; }
        }

        [XmlArray("databases")]
        public List<Database> Databases
        {
            get { return m_Databases; }
            set { m_Databases = value; }
        }
        [XmlArray("services")]
        public List<Service> Services
        {
            get { return m_Services; }
            set { m_Services = value; }
        }
        [XmlElement("webServices")]
        public WebServices Webservices { get; set; }

        public void Save()
        {
            this.Save(ConfigFile);
        }
    }
    /// <summary>
    /// represents a find replace item. Some values represent a macro 
    /// e.g. {SERVER_NAME}, {DNS_NAME}, {HOST_ADDR6}, {HOST_ADDR4}, {ASK_VARIABLE[n]}
    /// </summary>
    [XmlType(TypeName = "findReplaceVar")]
    public sealed class FindReplaceVar
    {
        private string _default;
        public FindReplaceVar()
        {
            IsGlobalSetting = true;//default
        }

        [XmlIgnore()]
        public string ComponentName { get; set; }

        [XmlAttribute("find")]
        public string Find { get; set; }

        [XmlAttribute("replace")]
        public string Replace { get; set; }

        /// <summary>
        /// Specifies a default value for e.g. askVariable
        /// It can include macro values as in Replace
        /// </summary>
        [XmlAttribute("default"), Description("Specifies a default value for e.g. askVariable")]
        public string Default
        {
            get
            {
                return _default;
            }
            set
            {
                _default = ParseMacro(value);
            }
        }
        /// <summary>
        /// Needed in case of a 'askvariable' macro
        /// </summary>
        [XmlAttribute("description")]
        public string Description { get; set; }

        [XmlIgnore()]
        public bool IsAsk { get { return Replace.Contains("{ASK_VARIABLE"); } }

        [XmlIgnore()]
        public int ToOrdinal
        {
            get
            {
                int bl = Replace.IndexOf("{ASK_VARIABLE");
                int no = int.Parse(Replace.Substring(bl + 13, Replace.IndexOf('}', bl + 13) - (bl + 13)));
                return no;
            }
        }
        /// <summary>
        /// If true, the setting will be asked once for all services
        /// ie the setting is shared
        /// </summary>
        [XmlAttribute("isGlobalSetting"), DefaultValue(true)]
        public bool IsGlobalSetting { get; set; }

        /// <summary>
        /// if globalsetting = false, a localsettingname must be provided
        /// </summary>
        [XmlAttribute("localSettingName")]
        public string LocalSettingName { get; set; }

        [XmlIgnore()]
        public AskVariables AskVariable { get; set; }
        /// <summary>
        /// processes the replace string evaluating any macro's or just returns the Replace value
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            if (IsAsk)
            {
                return Replace.Replace(string.Format("{{ASK_VARIABLE{0}}}", ToOrdinal), AskVariable.UserInput);
            }
            else
            {
                return ParseMacro(Replace);
            }
        }
        private static string ParseMacro(string repl)
        {
            bool isConnected = NetworkInterface.GetIsNetworkAvailable();

            // will convert NETBIOSname to NETBIOS.domain.local
            if (repl.IndexOf("{SERVER_NAME}") >= 0)
            {
                repl = repl.Replace("{SERVER_NAME}", Environment.MachineName);
            }
            if (repl.IndexOf("{ISO_DATE}") >= 0)
            {
                repl = repl.Replace("{ISO_DATE}", DateTime.Today.ToString("yyyy-MM-dd"));
            }
            if (repl.IndexOf("{DATE}") >= 0)
            {
                repl = repl.Replace("{DATE}", DateTime.Today.ToShortDateString());
            }
            if (repl.IndexOf("{DNS_NAME}") >= 0)
            {
                if (isConnected)
                {
                    repl = repl.Replace("{DNS_NAME}", Dns.GetHostEntry(Dns.GetHostName()).HostName);
                }
            }
            if (repl.IndexOf("{HOST_ADDR6}") >= 0)
            {
                if (isConnected)
                {
                    repl = repl.Replace("{HOST_ADDR6}", Dns.GetHostEntry(Dns.GetHostName()).AddressList.FirstOrDefault(f => f.AddressFamily == AddressFamily.InterNetworkV6).ToString());
                }
            }
            if (repl.IndexOf("{HOST_ADDR4}") >= 0)
            {
                if (isConnected)
                {
                    repl = repl.Replace("{HOST_ADDR4}", Dns.GetHostEntry(Dns.GetHostName()).AddressList.FirstOrDefault(f => f.AddressFamily == AddressFamily.InterNetwork).ToString());
                }
            }
            return repl;
        }
    }
    public class ServiceAccount
    {
        public ServiceAccount()
        {
        }
        [XmlAttribute("accountName")]
        public string AccountName { get; set; }

        /// <summary>
        /// if true, a local system service account will be created
        /// </summary>
        [XmlAttribute("windowsIntegratedSecurity"), DefaultValue(false)]
        public bool WindowsIntegratedSecurity { get; set; }

        /// <summary>
        ///  not yet implemented
        /// </summary>
        [XmlAttribute("generateRandomPassword"), DefaultValue(false)]
        public bool GenerateRandomPassword { get; set; }

        /// <summary>
        /// if true, becomes an admin
        /// if false, becomes a user
        /// </summary>
        [XmlAttribute("administrativerights"), DefaultValue(false)]
        public bool AdministrativeRights { get; set; }
    }

    public enum EnumDbTypes
    {
        Unknown = 0,
        SQLServer = 1,
        Informix = 2,
        Oracle = 3
    }

    [Flags]
    public enum EnumDependencies
    {
        Windows2008 = 1,
        Windows2008R2 = 2,
        Windows2003Sp2 = 4,
        WindowsVistaSp2 = 256,
        SQLServer2008 = 8,
        SQLServer2005 = 16,
        IIS6 = 32,
        IIS7 = 64,
        //Message Queue
        MQ = 128,
        //64-bit OS
        x64 = 512,
        WindowsServerOnly = 1024
    }

    [XmlType("appDependency")]
    public sealed class AppDependencies
    {

        public AppDependencies()
        {
        }
        [XmlAttribute("appName")]
        public EnumDependencies AppName { get; set; }
        /// <summary>
        /// returns true if te specific feature is avaialble
        /// </summary>
        [XmlIgnore()]
        public bool IsInstalled
        {
            get
            {
                switch (AppName)
                {
                    case EnumDependencies.MQ:
                        return util.IsMessageQueueueInstalled;
                    case EnumDependencies.Windows2008:
                    //fall through
                    case EnumDependencies.WindowsVistaSp2:
                        return Environment.OSVersion.Version >= new System.Version(6, 0, 6002);
                    case EnumDependencies.WindowsServerOnly:
                        //return Environment.
                        return true;
                    case EnumDependencies.Windows2003Sp2:
                        return Environment.OSVersion.Version >= new System.Version(5, 2, 3790);
                    case EnumDependencies.x64:
                        return IntPtr.Size == sizeof(long);
                    default:
                        return false;
                }
            }
        }
    }
    public sealed class DbInstallType
    {
        public DbInstallType()
        {
            this.ProviderName = "System.Data.SqlClient";
        }

        [XmlAttribute("dbType"), DefaultValue(EnumDbTypes.Unknown)]
        public EnumDbTypes DbType;
        [XmlAttribute("dbScript")]
        public string DbScript { get; set; }

        /// <summary>
        /// description if true, the provider will be prepended in the connection string e.g. System.Data.SqlClient|Initial Catalog=.. etc
        /// </summary>
        [XmlAttribute("iSOFTincludeProviderPrefix"), DefaultValue(false)]
        public bool iSOFTincludeProviderPrefix { get; set; }
        [XmlAttribute("connectionString")]
        public string ConnectionString { get; set; }

        public override string ToString()
        {
            return iSOFTincludeProviderPrefix ? ProviderName + "|" + ConnectionString : ConnectionString;
        }

        [XmlAttribute("providerName"), DefaultValue("System.Data.SqlClient")]
        public string ProviderName { get; set; }
    }

    [XmlType(TypeName = "database")]
    public sealed class Database
    {
       
        private List<FindReplaceVar> _findReplaceVars;

        [XmlArray("findReplaceVars")]
        public List<FindReplaceVar> ReplaceVariables
        {
            get { return _findReplaceVars; }
            set { _findReplaceVars = value; }

        }

        private bool? m_CanConnect;
        /// <summary>
        /// one DB can have several 'types' eg.. Oracle, SQL Server, Informix
        /// </summary>
        private List<DbInstallType> m_DbTypes;
        public Database()
        {
            m_DbTypes = new List<DbInstallType>();
            //_findReplaceVars = new List<FindReplaceVar>();

        }
        /// <summary>
        /// the name of the database
        /// </summary>
        [XmlAttribute("name")]
        public string Name { get; set; }
        [XmlElement("dbType")]
        public List<DbInstallType> DbTypes
        {
            get { return m_DbTypes; }
            set { }
        }

        /// <summary>
        /// used for internal UI mapping not for DB
        /// </summary>
        [XmlIgnore()]
        public int Ordinal { get; set; }
        [XmlIgnore()]
        public bool IsInstalled { get; set; }
        /// <summary>
        /// if true, the connection string just has been tested
        /// </summary>
        [XmlIgnore()]
        public bool CanConnect
        {
            get
            {
                if (m_CanConnect == null)
                {
                    // just for now take the first dbtype instance (mostly SQL server)
                    if (this.DbTypes != null && this.DbTypes.Count > 0)
                    {
                        m_CanConnect = DbCreator.CanConnect(DbTypes[0], true);
                    }
                    else
                    {
                        m_CanConnect = false;
                    }
                }
                return m_CanConnect.Value;
            }

            set
            {
                m_CanConnect = value;
            }
        }

        /// <summary>
        /// to be misused by binding to buttons in grids
        /// </summary>
        [XmlIgnore()]
        public string Action { get; set; }

        /// <summary>
        /// creates a db user account
        /// </summary>
        [XmlAttribute("ensuredDBUserAccount")]
        public string EnsuredDBUserAccount { get; set; }
        /// <summary>
        /// if set true, it will create a windows account if false, a db account
        /// </summary>
        [XmlAttribute("preferIntegratedSecurity")]
        public bool PreferIntegratedSecurity { get; set; }
        /// <summary>
        /// used for runtime creation password
        /// </summary>
        [XmlIgnore()]
        public string Password { get; set; }

        /// <summary>
        /// generates a random secure password
        /// </summary>
        [XmlAttribute("generateRandomPass"), DefaultValue(false)]
        public bool GenerateRandomPass { get; set; }

        //TODO: what about it generate random password and store it somewhere?or ask SETUP people to supply one?
        //public string EnsuredDBUserPassword

        [XmlAttribute("ensuredDBRole")]
        public string EnsuredDBRole { get; set; }

    }
    public enum ActionType
    {
        None = 0,
        /// <summary>
        /// this action causes a startup-action in windows task scheduler to be set
        /// </summary>
        StartupTask,
        DesktopIcon,
        RunWindows,
        CreateMenu
    }
    /// <summary>
    /// Defines specific actions to be done during or after setup
    /// </summary>
    [XmlType("action")]
    public sealed class Action
    {
        public Action()
        {
        }
        [XmlIgnore()]
        public object Parent;

        [XmlAttribute("name")]
        public string Name { get; set; }

        [XmlAttribute("actionType"), DefaultValue(ActionType.None)]
        public ActionType ActionType { get; set; }

        [XmlAttribute("executable")]
        public string Executable { get; set; }

        [XmlAttribute("description")]
        public string Description { get; set; }

        /// <summary>
        /// startup arguments
        /// </summary>
        [XmlAttribute("arguments")]
        public string Arguments { get; set; }

        /// <summary>
        /// under which credentials will this action be executed
        /// </summary>
        [XmlElement("serviceAccount")]
        public ServiceAccount ServiceAccount { get; set; }

        /// <summary>
        /// executes the designated action
        /// </summary>
        public void Execute()
        {
            switch (ActionType)
            {
                case ActionType.StartupTask:
                    if (ServiceAccount != null)
                    {
                        ScheduledTasks st = new ScheduledTasks();
                        Task tsk;
                        if (st.GetTaskNames().Exists(t => t.StartsWith(this.Name, StringComparison.InvariantCultureIgnoreCase)))
                        {
                            tsk = st.OpenTask(this.Name);
                        }
                        else
                        {
                            tsk = st.CreateTask(this.Name);
                        }
                        if (!string.IsNullOrEmpty(Description))
                        {
                            tsk.Comment = this.Description;
                        }
                        if (this.ServiceAccount != null && !string.IsNullOrEmpty(this.ServiceAccount.AccountName))
                        {

                            tsk.SetAccountInformation(NtSecurityInterop.UserAccountFormat(this.ServiceAccount.AccountName), DbCreator.GenerateRandomPass(true));
                        }
                        tsk.Triggers.Add(new OnLogonTrigger());

                        if (Parent != null && Parent is Application)
                        {
                            // this is done because Action also will be applied in 
                            // future for the service
                            Application app = (Application)Parent;
                            tsk.ApplicationName = '"' + Path.Combine(app.ProductLocation, Executable) + '"';
                        }
                        else
                        {
                            tsk.ApplicationName = Executable;
                        }

                        if (!string.IsNullOrEmpty(this.Arguments))
                        {
                            tsk.Parameters = this.Arguments;
                        }
                        tsk.Save();
                        tsk.Close();
                        st.Dispose();
                    }
                    break;
            }
        }

        public void Execute(bool remove)
        {
            if (remove)
            {
                ScheduledTasks st = new ScheduledTasks();
                Task t = st.OpenTask(this.Name);
                if (t != null)
                {
                    if (t.Status == TaskStatus.Running)
                    {
                        t.Terminate();
                    }
                    t.Dispose();

                    util.EnsureProcessKilled(this.Executable);
                    st.DeleteTask(this.Name);
                }



                st.Dispose();
            }
            else
            {
                Execute();
            }
        }
    }
    [XmlType("application")]
    public sealed class Application
    {
        public Application()
        {
        }
        private string _sourcePath;
        private string m_ProductLocation;
        private bool? _isInstalled;
        [XmlArray("appDependencies")]
        public List<AppDependencies> AppDependency { get; set; }

        /// <summary>
        /// what type of action will be performed with this appliction
        /// </summary>
        [XmlElement("action")]
        public Action Action { get; set; }

        [XmlAttribute("name")]
        public string Name { get; set; }

        /// <summary>
        /// returns true if the MSI on disk, is newer than the one installed
        /// </summary>
        [XmlIgnore()]
        public bool CanBeUpgraded
        {
            get
            {
                System.Version versionInstalled = MsiInterop.GetProductVersion(MsiInterop.GetLastProductCodeFromUpgradeCode(UpgradeCode));
                System.Version versionOnDisk = MsiInterop.GetProductVersion(Path.Combine(SourcePath, MsiFile));
                return versionOnDisk > versionInstalled;
            }
        }
        /// <summary>
        /// True if installed at IIS
        /// </summary>
        [XmlIgnore()]
        public bool IsInstalled
        {
            get
            {
                if (_isInstalled == null)
                {
                    _isInstalled = !string.IsNullOrEmpty(ProductLocation);
                }
                return _isInstalled.Value;
            }
            set
            {
                _isInstalled = value;
            }
        }

        /// <summary>
        /// basepath and all subpaths will be considered part of the service
        /// Applies to 'after' setup
        /// </summary>
        [XmlAttribute("sourcePath")]
        public string SourcePath
        {
            get
            {
                return string.IsNullOrEmpty(_sourcePath) ? util.AppPath :
                    Path.IsPathRooted(_sourcePath) ? _sourcePath : Path.GetFullPath(_sourcePath);
            }
            set
            {
                _sourcePath = value;
            }
        }
        /// <summary>
        /// relates to ProductCode in MSI file
        /// </summary>
        [XmlAttribute("productCode")]
        public Guid ProductCode { get; set; }

        /// <summary>
        /// relates to UpgradeCode in MSI file
        /// </summary>
        [XmlAttribute("upgradeCode")]
        public Guid UpgradeCode { get; set; }

        /// <summary>
        /// tries to return the product location based on the .exe name
        /// since this is a service
        /// </summary>
        [XmlIgnore()]
        public string ProductLocation
        {
            get
            {
                //NOTE: This code can be slow albeit, not that slow as would be using WindowsInstaller.Installer (COM)
                //not every MSI installation has this property INSTALLPROPERTY_INSTALLLOCATION "InstallLocation"
                // it is only there, if the original MSI had a specific property called 
                if (string.IsNullOrEmpty(m_ProductLocation) && MsiInterop.IsUpgradeCodeInstalled(UpgradeCode))
                {
                    m_ProductLocation = MsiInterop.GetProductInstallLocation(MsiInterop.GetLastProductCodeFromUpgradeCode(UpgradeCode), ".exe", ".sample", ".config");
                }
                return m_ProductLocation;
            }
        }

        /// <summary>
        /// the real deal containing the setup
        /// </summary>
        [XmlAttribute("msiFile")]
        public string MsiFile { get; set; }

        public void RemoveProduct()
        {
            int error = MsiInterop.MsiConfigureProductEx(MsiInterop.GetLastProductCodeFromUpgradeCode(this.UpgradeCode).ToString("b"),
                        MsiInterop.tagINSTALLLEVEL.INSTALLLEVEL_DEFAULT,
                        MsiInterop.tagINSTALLSTATE.INSTALLSTATE_DEFAULT, "REMOVE=ALL");
            if (error != 0)
            {
                throw new Win32Exception(error);
            }
        }
        public void InstallProduct()
        {
            string file = Path.Combine(SourcePath, MsiFile);
            string msiAction = "ACTION=INSTALL";

            if (!File.Exists(file))
            {
                throw new FileNotFoundException(string.Format("msi {0} does not exist! Make sure it does.", file));

            }
            int error = MsiInterop.MsiInstallProduct(file, msiAction);
            if (error != 0)
            {
                throw new Win32Exception(error);
            }
        }
    }
    /*
     *    <application name="XDSSystemTray" sourcePath="Tray" msiFile="iSOFT.XDSSystemTray.1.0.msi" upgradeCode="{D842F21A-8A7C-4341-9576-9A2928693DC9}">
      <action name="startupTask" executable="iSOFT.Configurator.exe" arguments="delay">
        <serviceAccount accountName="XDSAdmin" administrativerights="true" generateRandomPassword="true"/>        
      </action>
     * */
    [XmlType("service")]
    public sealed class Service
    {
        private string m_ProductLocation;
        private bool? _isInstalled;
        private string _sourcePath;
        private System.Version _versionInstalled;
        private System.Version _versionOnMsi;
        [XmlArray("appDependencies")]
        public List<AppDependencies> AppDependency { get; set; }
        public Service()
        {
            //_findReplaceVars = new List<FindReplaceVariable>();
        }
        private List<FindReplaceVar> _findReplaceVars;

        [XmlAttribute("name")]
        public string Name { get; set; }

        [XmlElement("application")]
        public List<Application> Application;

        /// <summary>
        /// basepath and all subpaths will be considered part of the service
        /// Applies to 'after' setup
        /// </summary>
        [XmlAttribute("sourcePath")]
        public string SourcePath
        {
            get
            {
                return string.IsNullOrEmpty(_sourcePath) ? util.AppPath :
                    Path.IsPathRooted(_sourcePath) ? _sourcePath : Path.GetFullPath(_sourcePath);
            }
            set
            {
                _sourcePath = value;
            }
        }
        /// <summary>
        /// the real deal containing the setup
        /// </summary>
        [XmlAttribute("msiFile")]
        public string MsiFile { get; set; }


        ///// <summary>
        ///// must be equal to the path set in the MSI file
        ///// You must set this to e.g. XDSInboundHL7V2Service see
        ///// DefaultLocation on 'File System' 
        ///// </summary>
        //[XmlAttribute("targetDir")]
        //public string TargetDir { get; set; }


        /// <summary>
        /// Sets/gets the physical file which must be configured
        /// </summary>
        [XmlAttribute("configFile")]
        public string ConfigFile { get; set; }

        /// <summary>
        /// denotes if a service must be running and installed (core)
        /// </summary>
        [XmlAttribute("vital"), DefaultValue(false)]
        public bool Vital { get; set; }

        /// <summary>
        /// If the service is up and running
        /// </summary>
        [XmlIgnore()]
        public bool IsRunning
        {
            get
            {
                if (!string.IsNullOrEmpty(ControllerName))
                {
                    try
                    {
                        ServiceController svc = new ServiceController(ControllerName);
                        return svc.Status == ServiceControllerStatus.Running;
                    }
                    // it does not exist
                    catch
                    {
                        return false;
                    }
                }
                return false;
            }

        }

        /// <summary>
        /// true if the service is configured
        /// </summary>
        [XmlIgnore()]
        public bool IsConfigured
        {
            get
            {

                return string.IsNullOrEmpty(ProductionLocation) ? false : File.Exists(Path.Combine(ProductionLocation, ConfigFile));
            }
        }
        public System.Version VersionMSI
        {
            get
            {
                return _versionOnMsi;
            }
        }
        public System.Version VersionInstalled
        {
            get
            {
                return _versionInstalled;
            }
        }

        /// <summary>
        /// returns true if the MSI on disk, is newer than the one installed
        /// </summary>     
        public bool CanBeUpgraded
        {
            get
            {
                _versionInstalled = MsiInterop.GetProductVersion(MsiInterop.GetLastProductCodeFromUpgradeCode(UpgradeCode));
                _versionOnMsi = MsiInterop.GetProductVersion(Path.Combine(SourcePath, MsiFile));
                return _versionInstalled > _versionOnMsi;
            }
        }
        /// <summary>
        /// based on ProductCode returns true/false if installed on Windows
        /// </summary>
        [XmlIgnore()]
        public bool IsInstalled
        {
            get
            {
                if (UpgradeCode == Guid.Empty)
                {
                    throw new Exception("UpgradeCode must be supplied. It must be copied from the MSI project file");
                }
                if (_isInstalled == null)
                {
                    _isInstalled = MsiInterop.IsMsiInstalled(MsiInterop.GetLastProductCodeFromUpgradeCode(UpgradeCode));
                }
                return _isInstalled.Value;
            }
            set
            {
                _isInstalled = value;
            }
        }
        public void UpgradeProduct()
        {
            if (IsInstalled)
            { RemoveProduct(); }

            InstallProduct();
        }
        public void InstallProduct()
        {
            string file = Path.Combine(SourcePath, MsiFile);
            string msiAction = "ACTION=INSTALL";

            if (!File.Exists(file))
            {
                throw new FileNotFoundException(string.Format("msi {0} does not exist! Make sure it does.", file));

            }
            int error = MsiInterop.MsiInstallProduct(file, msiAction);
            if (error != 0)
            {
                throw new Win32Exception(error);
            }

        }

        public void RemoveProduct()
        {
            int error = MsiInterop.MsiConfigureProductEx(MsiInterop.GetLastProductCodeFromUpgradeCode(this.UpgradeCode).ToString("b"),
                MsiInterop.tagINSTALLLEVEL.INSTALLLEVEL_DEFAULT,
                MsiInterop.tagINSTALLSTATE.INSTALLSTATE_DEFAULT, "REMOVE=ALL");
            if (error != 0)
            {
                throw new Win32Exception(error);
            }
        }


        /// <summary>
        /// the windows service controller name eg. 'iSOFT XDS Consumer Bridge'
        /// </summary>
        [XmlAttribute("controllerName")]
        public string ControllerName { get; set; }
        ///// <summary>
        ///// future license file which is required to run the service
        ///// </summary>
        //[XmlAttribute("licenseFile")]
        //public string LicenseFile { get; set; }


        [XmlArray("findReplaceVars")]
        public List<FindReplaceVar> ReplaceVariables
        {
            get { return _findReplaceVars; }
            set { _findReplaceVars = value; }

        }
        /// <summary>
        /// optional describes the input requested
        /// </summary>
        [XmlAttribute("description")]
        public string Description { get; set; }

        /// <summary>
        /// relates to ProductCode in MSI file
        /// </summary>
        [XmlAttribute("productCode")]
        public Guid ProductCode { get; set; }

        /// <summary>
        /// relates to UpgradeCode in MSI file
        /// </summary>
        [XmlAttribute("upgradeCode")]
        public Guid UpgradeCode { get; set; }

        /// <summary>
        /// tries to return the product location based on the .exe name
        /// since this is a service
        /// </summary>
        [XmlIgnore()]
        public string ProductionLocation
        {
            get
            {
                //NOTE: This code can be slow albeit, not that slow as would be using WindowsInstaller.Installer (COM)
                //not every MSI installation has this property INSTALLPROPERTY_INSTALLLOCATION "InstallLocation"
                // it is only there, if the original MSI had a specific property called 
                if (string.IsNullOrEmpty(m_ProductLocation) && MsiInterop.IsUpgradeCodeInstalled(UpgradeCode))
                {
                    m_ProductLocation = MsiInterop.GetProductInstallLocation(MsiInterop.GetLastProductCodeFromUpgradeCode(UpgradeCode), ".exe", ".sample", ".config");
                }
                return m_ProductLocation;
            }
        }

        /// <summary>
        /// lists dependend databases
        /// </summary>
        [XmlElement("database")]
        public List<Database> Databases { get; set; }

    }
    /// <summary>
    /// AppPool name to be shared by all IIS applications
    /// This is required for custom security e.g. for a username & PW to be applied
    /// </summary>
    [XmlType("appPool")]
    public sealed class AppPool
    {
        public AppPool()
        {
            TargetFramework = 4.0F;
        }
        [XmlAttribute("name")]
        public string Name { get; set; }
        
        //todo: apr 18 2012
        /// <summary>
        /// must be 2.0/4.0/4.5
        /// </summary>
        [DefaultValue(4.0F)]
        [XmlAttribute("targetFramework")]
        public float TargetFramework { get; set; }
    }
    [XmlType("webServices")]
    public sealed class WebServices
    {
        private List<Webservice> m_Webservices;
        public WebServices()
        {
            m_Webservices = new List<Webservice>();
        }
        [XmlElement("appPool")]
        public AppPool AppPool { get; set; }
        [XmlElement("Webservice")]
        public List<Webservice> WebService
        {
            get { return m_Webservices; }
            set { m_Webservices = value; }
        }
    }
    [XmlType("Webservice")]
    public sealed class Webservice
    {
        /// <summary>
        /// product location will be cached
        /// </summary>
        private string m_ProductLocation;
        private string _sourcePath;
        private bool? _isInstalled;
        private int? _instance;
        private static List<Site> sites;
        [XmlArray("appDependencies")]
        public List<AppDependencies> AppDependency { get; set; }
        public Webservice()
        {
            //_findReplaceVars = new List<FindReplaceVar>();
            AppPoolNetVersion = 2.0F;
            if (sites == null)
            {
                sites = IIS.Getsites;
            }
        }
        private List<FindReplaceVar> _findReplaceVars;
        [XmlAttribute("name")]
        public string Name { get; set; }
        [XmlElement("application")]
        public List<Application> Application;
        /// <summary>
        /// basepath and all subpaths will be considered part of the service
        /// If sourcepath is specified, you cannot use MsiFile
        /// </summary>
        [XmlAttribute("sourcePath")]
        public string SourcePath
        {
            get
            {
                return string.IsNullOrEmpty(_sourcePath) ? util.AppPath :
                    Path.IsPathRooted(_sourcePath) ? _sourcePath : Path.GetFullPath(_sourcePath);
            }
            set
            {
                _sourcePath = value;
            }
        }
        /// <summary>
        /// returns true if the MSI on disk, is newer than the one installed
        /// </summary>
        [XmlIgnore()]
        public bool CanBeUpgraded
        {
            get
            {
                System.Version versionInstalled = MsiInterop.GetProductVersion(MsiInterop.GetLastProductCodeFromUpgradeCode(UpgradeCode));
                System.Version versionOnDisk = MsiInterop.GetProductVersion(Path.Combine(SourcePath, MsiFile));
                return versionOnDisk > versionInstalled;
            }
        }
        [XmlIgnore()]
        public int IISInstance
        {
            get
            {
                if (_instance == null)
                {
                    if (!IsInstalled)
                    {
                        _instance = 0;
                    }
                }
                return _instance.Value;
            }
            set
            { _instance = value; }
        }
        /// <summary>
        /// denotes if a service must be running and installed (core)
        /// </summary>
        [XmlAttribute("vital"), DefaultValue(false)]
        public bool Vital { get; set; }

        /// <summary>
        /// True if installed at IIS
        /// </summary>
        [XmlIgnore()]
        public bool IsInstalled
        {
            get
            {
                if (_isInstalled == null)
                {
                    if (string.IsNullOrEmpty(ProductLocation))
                    {
                        _isInstalled = false;
                    }
                }
                return _isInstalled.Value;
            }
            set
            {
                _isInstalled = value;
            }
        }

        /// <summary>
        /// if msiFile is specified, you cannot use SourcePath
        /// </summary>
        [XmlAttribute("msiFile")]
        public string MsiFile { get; set; }
        /// <summary>
        /// Defines the IIS site name, e.g. "Default Web Site"
        /// </summary>
        [XmlAttribute("site")]
        public string Site { get; set; }

        /// <summary>
        /// product location will be cached
        /// </summary>

        [XmlIgnore()]
        public string ProductLocation
        {
            get
            {
                //NOTE: This code can be slow albeit, not that slow as would be using WindowsInstaller.Installer (COM)
                //not every MSI installation has this property INSTALLPROPERTY_INSTALLLOCATION "InstallLocation"
                // it is only there, if the original MSI had a specific property called 
                // we have installed from an MSI or not?
                if (string.IsNullOrEmpty(m_ProductLocation))
                {
                    if (UpgradeCode == Guid.Empty)
                    {
                        // not from an MSI
                        int instance;
                        if (IIS.VDirExists(sites, VirtualDir, out instance))
                        {
                            IISInstance = instance;
                            IsInstalled = true;
                            m_ProductLocation = IIS.RootPath(instance.ToString());
                        }
                    }
                    else if (MsiInterop.IsUpgradeCodeInstalled(UpgradeCode))
                    {
                        m_ProductLocation = MsiInterop.GetProductInstallLocation(MsiInterop.GetLastProductCodeFromUpgradeCode(UpgradeCode), ".exe", ".sample", ".config");
                        IsInstalled = true;
                        if (_instance == null)
                        {
                            int instance;
                            if (IIS.VDirExists(sites, VirtualDir, out instance))
                            {
                                _instance = instance;
                            }
                        }
                    }
                }
                return m_ProductLocation;
            }

        }

        /// <summary>
        /// an IIS Virtual Directory 
        /// </summary>
        [XmlAttribute("virtualDir")]
        public string VirtualDir { get; set; }

        /// <summary>
        /// DefaultAppPool or custom one
        /// </summary>
        [XmlAttribute("appPoolname")]
        public string AppPoolName { get; set; }

        /// <summary>
        /// e.g. 2.0, 3.5 , 4.0
        /// </summary>
        [XmlAttribute("appPoolNetVersion"), DefaultValue(2.0F)]
        public float AppPoolNetVersion { get; set; }

        /// <summary>
        /// specifies which variables will be replaced in the config files
        /// </summary>
        [XmlArray("findReplaceVars")]
        public List<FindReplaceVar> ReplaceVariables
        {
            get { return _findReplaceVars; }
            set { _findReplaceVars = value; }

        }
        /// <summary>
        /// future license file which is required to run the service
        /// </summary>
        [XmlAttribute("licenseFile")]
        public string LicenseFile { get; set; }

        /// <summary>
        /// relates to ProductCode in MSI file
        /// </summary>
        [XmlAttribute("productCode")]
        public Guid ProductCode { get; set; }
        /// <summary>
        /// relates to UpgradeCode in MSI file
        /// </summary>
        [XmlAttribute("upgradeCode")]
        public Guid UpgradeCode { get; set; }

        /// <summary>
        /// lists dependend databases
        /// </summary>
        [XmlElement("database")]
        public List<Database> Databases { get; set; }

        public void InstallProduct()
        {
            string file = Path.Combine(SourcePath, MsiFile);
            string msiAction = string.Format("ACTION=INSTALL TARGETSITE=\"/LM/W3SVC/{0}\" TARGETVDIR=\"{1}\" TARGETAPPPOOL=\"{2}\"", IISInstance, VirtualDir, AppPoolName);
            int error = MsiInterop.MsiInstallProduct(file, msiAction);
            if (error != 0)
            {
                throw new Win32Exception(error);
            }
        }

        public void RemoveProduct()
        {
            int error = MsiInterop.MsiConfigureProductEx(MsiInterop.GetLastProductCodeFromUpgradeCode(this.UpgradeCode).ToString("b"),
                MsiInterop.tagINSTALLLEVEL.INSTALLLEVEL_DEFAULT,
                MsiInterop.tagINSTALLSTATE.INSTALLSTATE_DEFAULT, "REMOVE=ALL");
            if (error != 0)
            {
                throw new Win32Exception(error);
            }
        }
    }
    /// <summary>
    /// functions as a holder record for user variables to be requested
    /// </summary>
    public sealed class AskVariables
    {
        public AskVariables()
        {
            //Parent = new FindReplaceVar();
        }
        public int Ordinal { get; set; }
        //public string Description { get; set; }

        [XmlIgnore()]
        public FindReplaceVar Parent { get; set; }


        public string ComponentName
        {
            get { return Parent != null ? Parent.ComponentName : null; }
        }
        /// <summary>
        /// If true, the setting will be asked once for all services
        /// ie the setting is shared
        /// </summary>
        public bool IsGlobalSetting
        {
            get
            {
                return Parent != null ? Parent.IsGlobalSetting : true;
            }
        }
        public string UserInput { get; set; }
        /// <summary>
        ///  we override this to get union working
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        public override bool Equals(object obj)
        {
            AskVariables comp = (AskVariables)obj;
            if (Parent.IsGlobalSetting)
            {
                return comp.Ordinal == Ordinal;
            }
            else
            {
                return comp.Parent.LocalSettingName == Parent.LocalSettingName && comp.Ordinal == Ordinal;
            }
        }
        public string Description
        {
            get
            {
                return Parent != null ? Parent.Description : null;
            }
        }
        /// <summary>
        /// if globalsetting = false, a localsettingname must be provided
        /// </summary>
        public string LocalSettingName
        {
            get
            {
                return Parent != null ? Parent.LocalSettingName : null;
            }
        }

        public override int GetHashCode()
        {
            if (Parent.IsGlobalSetting)
            {
                return Ordinal.GetHashCode();
            }
            else
            {
                return (Parent.LocalSettingName + Ordinal.ToString()).GetHashCode();
            }
        }
    }
}