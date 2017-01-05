using System.Web;
using System.Web.Configuration;
namespace ispsession.io
{
    /// <summary>
    /// Session Default Application settings
    /// NOTE: DO not confuse with Session Settings which might differ per session (such as timeout)
    /// </summary>
    public class SessionAppSettings
    {
        private const string ispsession_io_pref = "ispsession_io:";
        public SessionAppSettings()
        {
            var cfg = WebConfigurationManager.AppSettings; 
            AppKey = cfg.GetAppValue<string>(ispsession_io_pref + "APP_KEY");
            if (string.IsNullOrEmpty(AppKey))
            {
                throw new HttpException("APP_KEY must be specified for ISP Session");
            }
            Path = cfg.GetAppValue(ispsession_io_pref + "AD_PATH", default(string));
            Domain = cfg.GetAppValue(ispsession_io_pref + "AD_DOMAIN", default(string));
            SnifQueryStringFirst = cfg.GetAppValue(ispsession_io_pref + "SnifQueryStringFirst", false);
            CookieNoSSL = cfg.GetAppValue(ispsession_io_pref + "CookieNoSSL",false);
            CookieExpires = cfg.GetAppValue(ispsession_io_pref + "CookieExpires", 0);
            CookieName = cfg.GetAppValue(ispsession_io_pref + "CookieName", "GUID");
            SessionTimeout = cfg.GetAppValue(ispsession_io_pref + "SessionTimeout", 30);
            Liquid = cfg.GetAppValue(ispsession_io_pref + "LiquidSession", false);
            ReEntrance = cfg.GetAppValue(ispsession_io_pref + "ReEntrance", false);
            Compress = cfg.GetAppValue(ispsession_io_pref + "Compress", false);
            DataBase = cfg.GetAppValue(ispsession_io_pref + "DataBase", -1);
            // do not try to parse apx/mvc pages to find the EnableSessionState Meta Tag
            //SkipMetaTagParsing = cfg.GetAppValue("SkipMetaTagParsing", false);
            if (SessionTimeout == 0)
            {
                SessionTimeout = 30;
            }
            Lic = cfg.GetAppValue<string>(ispsession_io_pref + "Csession.LIC");
            if (string.IsNullOrEmpty(Lic))
            {
                throw new HttpException("Ccession.LIC appSetting missing");
            }
            LicKey = cfg.GetAppValue<string>(ispsession_io_pref + "License");
#if !DEMO
            if (string.IsNullOrEmpty(LicKey))
            {
                throw new HttpException("License appSetting missing");
            }
#endif
            DatabaseConnection = cfg.GetAppValue(ispsession_io_pref + "DataSource", "localhost:6379");
            StreamManager.TraceInformation(@"Cookie ({0}), AppKey({1}), Path({2}), 
                                    Domain({3}), SnifQ({4}), CookieNoSSL({5}), 
                                    CookieExpires({6}), SessionTimeout({7}), Liquid({8}), 
                                    ReEntrance({9}), Compress({10}) , Db {11}
                                    Connection({12})", this.CookieName, this.AppKey, this.Path,
                                                       this.Domain, this.SnifQueryStringFirst, this.CookieNoSSL,
                                                        this.CookieExpires, this.SessionTimeout, this.Liquid,
                                                        this.ReEntrance, this.Compress, this.DataBase,
                                                        this.DatabaseConnection);
        }
        //defaults to GUID
        public string CookieName;
        //  [Description("APP_KEY")]
        public string AppKey;
        //cookie path
        // [Description("AD_PATH")]
        public string Path;
        //cokie domain path
        // [Description("AD_DOMAIN")]
        public string Domain;
        //  [Description("SnifQueryStringFirst")]
        public bool SnifQueryStringFirst ;
        /// <summary>
        /// allows both https and http to be mixed
        /// </summary>
        //  [Description("CookieNoSSL")]
        public bool CookieNoSSL;
        //  [Description("CookieExpires")]
        public int CookieExpires;
        
        //defaults to 30 minutes
        public int SessionTimeout;

        public bool Liquid;
        /// <summary>
        /// Difference with ISP Sesssion classic 
        /// Because this setting with .NET is RegenerateExpiredSessionId(true/false)
        /// </summary>
        public bool ReEntrance;
        public bool Compress;
     //   public bool SkipMetaTagParsing { get; private set; }
        //    [Description("Csession.LIC")]
        public string Lic;
        // [Description("License")]
        public string LicKey;
        //redis datasource
        //   [Description("DataSource")]
        /// <summary>
        /// Redis, mostly, such as "localhost:6379"
        /// </summary>
        public string DatabaseConnection;
        /// <summary>
        /// default -1 which Redis database number to use
        /// </summary>
        public int DataBase;

        /// <summary>
        /// creates a copy of Settings. So, we can dabble with the settings, without disturbing globals
        /// Note: Should be struct, but for performance reason, kept class.
        /// </summary>
        public SessionAppSettings Clone()
        {
            //should be struct, I know
            return new SessionAppSettings()
            {
                AppKey = this.AppKey,
                Compress = this.Compress,
                CookieExpires = this.CookieExpires,
                CookieName = this.CookieName,
                CookieNoSSL = this.CookieNoSSL,
                DataBase = this.DataBase,
                DatabaseConnection = this.DatabaseConnection,
                Domain = this.Domain,
                Lic = this.Lic,
                LicKey = this.LicKey,
                Liquid = this.Liquid,
                Path = this.Path,
                ReEntrance = this.ReEntrance,
                SessionTimeout = this.SessionTimeout,
                SnifQueryStringFirst = this.SnifQueryStringFirst
            };
        }
    }
}
