using System;
using System.Configuration;
using System.Web;
using System.Web.Configuration;

namespace ispsession.io
{
    public class ISPApplicationModule : IHttpModule
    {
        private static readonly object locker = new object();
        private const string ItemContextKey = "_ISPApplicationCache";
        private static SessionAppSettings _appSettings;
        private DateTimeOffset _startTime;
        private static void EnsureAppSettings()
        {

            if (_appSettings == null)
            {
                lock (locker)
                {
                    var cfg = WebConfigurationManager.AppSettings;
                    var AppKey = cfg.GetAppValue<string>(SessionAppSettings.ispsession_io_pref + "APP_KEY");

                    _appSettings = new SessionAppSettings()
                    {
                        AppKey = AppKey,
                        DatabaseConnection = cfg.GetAppValue<string>(SessionAppSettings.ispsession_io_pref + "DataSource")
                    };

                }
            }
        }
        void IHttpModule.Dispose()
        {
        }

        void IHttpModule.Init(HttpApplication context)
        {
            context.AcquireRequestState += this.OnAcquireRequestState;
            context.ReleaseRequestState += this.OnReleaseRequestState;

        }
        private void OnAcquireRequestState(object source, EventArgs args)
        {
            var context = ((HttpApplication)source).Context;
            if (!context.Items.Contains(ItemContextKey))
            {
                EnsureAppSettings();
                if (!ISPSessionIDManager.Validate2(_appSettings.AppKey))
                {
                    throw new ConfigurationErrorsException($"invalid key {SessionAppSettings.ispsession_io_pref}:APP_KEY {_appSettings.AppKey}");
                }
                var appInstance = new ApplicationCache();
                _startTime = DateTimeOffset.UtcNow;
                var db = CSessionDL.SafeConn.GetDatabase(_appSettings.DataBase);
                CSessionDL.ApplicationGet(db, _appSettings.AppKey, appInstance);

                context.Items[ItemContextKey] = appInstance;

            }
        }
        private void OnReleaseRequestState(object source, EventArgs args)
        {
            var context = ((HttpApplication)source).Context;
            var appInstance = (ApplicationCache)context.Items[ItemContextKey];
            var db = CSessionDL.SafeConn.GetDatabase(_appSettings.DataBase);
            CSessionDL.ApplicationSave(db, _appSettings.AppKey, appInstance, DateTimeOffset.UtcNow - _startTime);
        }
    }
}