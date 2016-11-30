namespace ispsession.io
{
    /// <summary>
    /// Session Default Application settings
    /// NOTE: DO not confuse with Session Settings which might differ per session (such as timeout)
    /// </summary>
    public class SessionAppSettings
    {
        //defaults to GUID
        public string CookieName { get; set; }
        //  [Description("APP_KEY")]
        public string AppKey { get; set; }
        //cookie path
        // [Description("AD_PATH")]
        public string Path { get; set; }
        //cokie domain path
        // [Description("AD_DOMAIN")]
        public string Domain { get; set; }
        //  [Description("SnifQueryStringFirst")]
        public bool SnifQueryStringFirst { get; set; }
        /// <summary>
        /// allows both https and http to be mixed
        /// </summary>
        //  [Description("CookieNoSSL")]
        public bool CookieNoSSL { get; set; }
        //  [Description("CookieExpires")]
        public int CookieExpires { get; set; }

        //defaults to 30 minutes
        public int SessionTimeout { get; set; }

        public bool Liquid { get; set; }
        /// <summary>
        /// Difference with ISP Sesssion classic 
        /// Because this setting with .NET is RegenerateExpiredSessionId(true/false)
        /// </summary>
        public bool ReEntrance { get; set; }
        public bool Compress { get; set; }
        
        //    [Description("Csession.LIC"]
        public string Lic { get; set; }
        // [Description("License")]
        public string LicKey { get; set; }
        //redis datasource
        //   [Description("DataSource")]
        /// <summary>
        /// Redis, mostly, such as "localhost:6379"
        /// </summary>
        public string DatabaseConnection { get; set; }
        /// <summary>
        /// default -1 which Redis database number to use
        /// </summary>
        public int DataBase { get; set; }
        /// <summary>
        /// 0 = disable, 2=write to windows\temp 3 =outputdebug
        /// </summary>
        public int EnableLogging { get; set; }
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
                SnifQueryStringFirst = this.SnifQueryStringFirst,
                EnableLogging = this.EnableLogging
            };
        }
    }
}
