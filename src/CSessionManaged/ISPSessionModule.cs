﻿using System;
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
    internal sealed class ISPHttpSessionStateContainer : HttpSessionStateContainer
    {
        internal ISPHttpSessionStateContainer(
            string id, string oldSessionId, ISessionStateItemCollection sessionItems,
            HttpStaticObjectsCollection staticObjects,
            int timeout,
            bool newSession,
            HttpCookieMode cookieMode,
            SessionStateMode mode,
            bool isReadonly) :
            base(id, sessionItems, staticObjects, timeout, newSession, cookieMode, mode, isReadonly)
        {
            this.oldSessionId = oldSessionId;
        }
        internal DateTime StartedAt = DateTime.UtcNow;
        public string oldSessionId { get; set; }
    }
    //since Module is loaded once, you can consider the member variables static
    // so do not use static stuff, it complicates more than it fixes
    //System.Web.SessionState.ISessionStateModule 4.6.2
    public sealed class ISPSessionModule : IHttpModule
    {
        private static bool _initialized;
        /// <summary>
        /// INCR can also be supported at Redis. TODO: think about this idea
        /// </summary>
        private static int _instanceCount;
        private static ISPSessionIDManager _sessionIDManager;
        private static readonly object locker = new object();

        private static SessionAppSettings _appSettings;

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
                    _appSettings = new SessionAppSettings();
                    // Create a SessionIDManager.
                    _sessionIDManager = new ISPSessionIDManager(_appSettings);
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
                Diagnostics.TraceError("ISP Sesssion cannot write to EventLog with the current security context {0}", Thread.CurrentPrincipal.Identity.Name);
            }

        }
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
            //TODO: remove license Debug.WriteLine muke!
            if (Interlocked.Increment(ref _instanceCount) > StreamManager.Maxinstances)
            {
                Thread.Sleep(500 * (_instanceCount - StreamManager.Maxinstances));
                Trace.TraceInformation("LICENSE ERROR max = {0} requested ={1} \r\n", StreamManager.Maxinstances, _instanceCount);
                ISPSessionModule.WriteToEventLog(new Exception(string.Format(LicenseSpace, _instanceCount, StreamManager.Maxinstances)), "instancing");
            }
            //if (DateTime.UtcNow.Second % 6 == 0)
            //{
            //    NativeMethods.OutputDebugStringW(string.Format("License {0}\r\n", _instanceCount));
            //}
            bool isNew = false;
            string sessionID;
            string oldSessionID = null;
            sessionID = _sessionIDManager.GetSessionID(context);

            ISPSessionStateItemCollection sessionItems = null;
            if (sessionID != null)
            {
                sessionItems = CSessionDL.SessionGet(_appSettings, sessionID);
            }

            if (_appSettings.Liquid || sessionID == null)
            {
                oldSessionID = sessionID;
                sessionID = _sessionIDManager.CreateSessionID(context);

                //if (redirected)
                // create empty SessionState but we will not support this.
                //    return;
            }

            _sessionIDManager.SaveSessionID(context, sessionID, out bool redirected, out bool cookieAdded);
            if (sessionItems == null)
            {
                // Identify the session as a new session state instance. Create a new SessionItem
                // and add it to the local Hashtable.

                isNew = true;
                sessionItems = new ISPSessionStateItemCollection()
                {
                    Items = new SessionStateItemCollection(),
                    Meta = new PersistMetaData(false)
                    {
                        Expires = _appSettings.SessionTimeout,
                        Liquid = _appSettings.Liquid,
                        ReEntrance = _appSettings.ReEntrance
                    }
                };
                if (sessionMode == PagesEnableSessionState.True)
                {
                    CSessionDL.SessionInsert(_appSettings, sessionID, sessionItems.Meta);
                }

            }
            else
            {
                Diagnostics.TraceInformation("CSessionDL.SessionGet expires({0}), reentrance({1}), liquid({2}), size({3})",
                    sessionItems.Meta.Expires,
                    sessionItems.Meta.ReEntrance,
                    sessionItems.Meta.Liquid,
                    sessionItems.Meta.ZLen);
            }
            SessionStateUtility.AddHttpSessionStateToContext(context,
                             new ISPHttpSessionStateContainer(sessionID,
                                                            oldSessionID,
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
            Diagnostics.TraceInformation(UninitString, _instanceCount);

            // Read the session state from the context
            var stateProvider = (ISPHttpSessionStateContainer)SessionStateUtility.GetHttpSessionStateFromContext(context);
            var oldSessionId = stateProvider.oldSessionId;
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
                CSessionDL.SessionSave(_appSettings, stateProvider, oldSessionId, ref meta);
                var spentTime = (DateTime.UtcNow - stateProvider.StartedAt).TotalMilliseconds;
                Diagnostics.TraceInformation("CSessionDL.SessionSave timeOut ({0}), reEntrance({1}), Liquid({2}), size({3}) time({4})",
                    meta.Expires, meta.ReEntrance, meta.Liquid, meta.ZLen, Math.Ceiling(spentTime));

            }
            SessionStateUtility.RemoveHttpSessionStateFromContext(context);
        }
    }
}