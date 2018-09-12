namespace ispsession.io
{
    public class CacheAppSettings
    {
        public string AppKey { get; set; }
        //domains and such
        public string Lic { get; set; }

        public string LicKeyCore { get; set; }
       
        //redis datasource

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

    }
    /// <summary>
    /// Session Default Application settings
    /// NOTE: DO not confuse with Session Settings which might differ per session (such as timeout)
    /// </summary>
    public class SessionAppSettings
    {
        //defaults to GUID
        public string CookieName { get; set; }
 
        public string AppKey { get; set; }
        //cookie path
     
        public string Path { get; set; }
        //cokie domain path
     
        public string Domain { get; set; }
       
        public bool SnifQueryStringFirst { get; set; }
        /// <summary>
        /// allows both https and http to be mixed
        /// </summary>
     
        public bool CookieNoSSL { get; set; }
        public bool HttpOnly { get; set; }
       
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
        
        
        //domains and such
        public string Lic { get; set; }
       
        public string LicKeyCore { get; set; }
       
        //redis datasource
       
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
                LicKeyCore = this.LicKeyCore,
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
