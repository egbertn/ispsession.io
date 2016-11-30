using System;
using System.Diagnostics;
using System.Security;
using System.Threading;
using System.Web;
using System.Web.Configuration;
using System.Web.SessionState;

namespace ispsession.io
{
    //// not quite nice but it does the job
    internal sealed class ISPSessionStateItemCollection
    {
        internal PersistMetaData Meta;
        internal SessionStateItemCollection Items;
    }
    internal sealed class ISPHttpSessionStateContainer: HttpSessionStateContainer
    {
        internal ISPHttpSessionStateContainer(
            string id, ISessionStateItemCollection sessionItems, 
            HttpStaticObjectsCollection staticObjects, 
            int timeout, 
            bool newSession, 
            HttpCookieMode cookieMode, 
            SessionStateMode mode, 
            bool isReadonly) :
            base(id, sessionItems, staticObjects, timeout, newSession, cookieMode, mode, isReadonly)
        {

        }
        internal DateTime StartedAt = DateTime.UtcNow;
    }
    //since Module is loaded once, you can consider the member variables static
    // so do not use static stuff, it complicates more than it fixes
    //System.Web.SessionState.ISessionStateModule 4.6.2
    public sealed class ISPSessionModule : IHttpModule
    {
        private bool _initialized;
        /// <summary>
        /// INCR can also be supported at Redis. TODO: think about this idea
        /// </summary>
        private int _instanceCount;
        private ISPSessionIDManager _sessionIDManager;
        private readonly object locker = new object();

        private SessionAppSettings _appSettings;

        private const string UninitString = "Uninit CSessionManaged {0}";
        private const string InitString = "Init CSessionManaged {0}";
        internal const string EventSource = "ISPSession";
        private const string LicenseSpace =
    "ISP Session now has insufficient license space for the required number of instances {0} while this {1} is the maximum. You can contact information@adccure.nl to extent the license! Thank you!";


        void IHttpModule.Init(HttpApplication app)
        {
            app.AcquireRequestState += this.OnAcquireRequestState;
            app.ReleaseRequestState += this.OnReleaseRequestState;

            if (!_initialized)
            {
                lock (locker)
                {
                    _initialized = true;
                    this._appSettings = new SessionAppSettings();
                    // Create a SessionIDManager.
                    _sessionIDManager = new ISPSessionIDManager(_appSettings);
#if !Demo
            string license = _appSettings.Lic;
            var lic = _appSettings.LicKey;
            if (string.IsNullOrEmpty(license))
            {
                throw new Exception("Ccession.LIC appsetting is missing in web.config");
            }
            if (string.IsNullOrEmpty(lic))
            {
                throw new Exception(
                    "ISP Session requires a Appsetting such as <add key=\"License\" value=\"0245456556560418A91B161F23534007\" />");
            }
                    Checked = Helpers.LicentieCheck(Helpers.HexToBytes(lic), license);
#else
                    Checked = true;
#endif
                }
            }
        }

        void IHttpModule.Dispose()
        {
            //really, nothing to dispose here.
        }
        /// <summary>
        /// Will mostly deal with Redis exceptions
        /// </summary>        
        internal static void WriteToEventLog(Exception e, string action)
        {
            try
            {
                //HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\Eventlog\Application\ISPSession does not exist
                System.Diagnostics.EventLog.WriteEntry(
                     EventSource,
                     string.Format("An exception occurred communicating with the data source.\n\nAction: {0}\n\nException: {1}", action, e),
                     EventLogEntryType.Error, 1, 1);
            }
            catch (SecurityException)
            {
                Helpers.TraceError("ISP Sesssion cannot write to EventLog with the current security context {0}", Thread.CurrentPrincipal.Identity.Name);
            }

        }
        private bool Checked;
        //
        // Event handler for HttpApplication.AcquireRequestState
        //

        private void OnAcquireRequestState(object source, EventArgs args)
        {
            var context = ((HttpApplication)source).Context;
            var sessionMode = SessionMode(context);
            if (sessionMode == PagesEnableSessionState.False)
            {
                return;//no business here
            }
            if (Interlocked.Increment(ref _instanceCount) > Helpers.Maxinstances)
            {
                Thread.Sleep(500 * (_instanceCount - Helpers.Maxinstances));
                ISPSessionModule.WriteToEventLog(new Exception(string.Format(LicenseSpace, _instanceCount, Helpers.Maxinstances)), "instancing");
            }

            bool isNew = false;
            string sessionID;

            sessionID = _sessionIDManager.GetSessionID(context);

            ISPSessionStateItemCollection sessionItems = null;
            if (sessionID != null)
            {
                sessionItems = CSessionDL.SessionGet(_appSettings, sessionID);
            }
#if !Demo
            if (!Checked)
            {
                HttpContext.Current.Response.Write("The ispsession.io Module should be licensed. Please contact ADC Cure for an updated license at information@adccure.nl");
            }
#else

            var exp = double.Parse(Helpers.GetMetaData("at"));
            if (DateTime.Today > DateTime.FromOADate(exp))
            {
                context.Response.Write("The ispsession.io Module expired! We would welcome your order at <a href=\"http://ispsession.io/?page=order\">order here</a>");
            }

#endif
            if (_appSettings.Liquid || sessionID == null)
            {
                bool redirected, cookieAdded;
                sessionID = this._sessionIDManager.CreateSessionID(context);
                _sessionIDManager.SaveSessionID(context, sessionID, out redirected, out cookieAdded);

                //if (redirected)
                // create empty SessionState but we will not support this.
                //    return;
            }

            if (sessionItems == null)
            {
                // Identify the session as a new session state instance. Create a new SessionItem
                // and add it to the local Hashtable.

                isNew = true;
                sessionItems = new ISPSessionStateItemCollection()
                {
                    Items = new SessionStateItemCollection(),
                    Meta =  new PersistMetaData(false)
                    {
                        Expires = this._appSettings.SessionTimeout,
                        Liquid = this._appSettings.Liquid,
                        ReEntrance = this._appSettings.ReEntrance
                    } 
                };
                if (sessionMode == PagesEnableSessionState.True)
                {
                    CSessionDL.SessionInsert(this._appSettings, sessionID, sessionItems.Meta);
                }

            }
            else
            {
                Helpers.TraceInformation("CSessionDL.SessionGet expires({0}), reentrance({1}), liquid({2}), size({3})", 
				    sessionItems.Meta.Expires,
				    sessionItems.Meta.ReEntrance,
				    sessionItems.Meta.Liquid,
				    sessionItems.Meta.ZLen);
            }
            SessionStateUtility.AddHttpSessionStateToContext(context,
                             new ISPHttpSessionStateContainer(sessionID,
                                                          sessionItems.Items,
                                                          SessionStateUtility.GetSessionStaticObjects(context),
                                                          sessionItems.Meta.Expires,
                                                          isNew,
                                                          HttpCookieMode.UseCookies,
                                                          SessionStateMode.Custom,
                                                          sessionMode == PagesEnableSessionState.ReadOnly));

            // Execute the Session_OnStart event for a new session.
            if (isNew && Start != null)
            {
                Start(this, EventArgs.Empty);
            }
        }

        /// <summary>
        /// determines EnableSessionState Page Tag directive.
        /// </summary>    
        private static PagesEnableSessionState SessionMode(HttpContext context)
        {
            if (context.Handler is IRequiresSessionState)
            {
                return PagesEnableSessionState.True;
            }
            else if (context.Handler is IReadOnlySessionState)
            {
                return PagesEnableSessionState.ReadOnly;
            }
            return PagesEnableSessionState.False;
        }
        //
        // Event for Session_OnStart event in the Global.asax file.
        //
        public event EventHandler Start;

        private void OnReleaseRequestState(object source, EventArgs args)
        {
            var context = ((HttpApplication)source).Context;

            if (SessionMode(context) == PagesEnableSessionState.False)
            {
                return;//no business here
            }
            Interlocked.Decrement(ref _instanceCount);
            Helpers.TraceInformation(UninitString, _instanceCount);

            // Read the session state from the context
            var stateProvider = (ISPHttpSessionStateContainer)SessionStateUtility.GetHttpSessionStateFromContext(context);
            
            var sessionID = stateProvider.SessionID;

            if (stateProvider.IsAbandoned)
            {
                _sessionIDManager.RemoveSessionID(context);
                // Redis cleans up sessions, it's not really useful doing this
                //SessionStateUtility.RaiseSessionEnd(stateProvider, this, EventArgs.Empty);
                CSessionDL.SessionRemove(_appSettings, sessionID);
            }
            else
            {
                var meta = new PersistMetaData(false)
                {
                    Expires = stateProvider.Timeout,
                    LastUpdated = new DBTIMESTAMP(),
                    ReEntrance = _appSettings.ReEntrance,
                    Liquid = _appSettings.Liquid
                };
                CSessionDL.SessionSave(_appSettings, stateProvider, ref meta);
                var spentTime = (DateTime.UtcNow - stateProvider.StartedAt).TotalMilliseconds;
                Helpers.TraceInformation("CSessionDL.SessionSave timeOut ({0}), reEntrance({1}), Liquid({2}), size({3}) time({4})",
                    meta.Expires, meta.ReEntrance, meta.Liquid, meta.ZLen, Math.Ceiling(spentTime));

            }
            SessionStateUtility.RemoveHttpSessionStateFromContext(context);
        }
    }
}