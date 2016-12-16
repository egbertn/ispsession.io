using System;
using System.Collections;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace ispsession.io
{
    public class ISPSession : IISPSession
    {
        private readonly SessionAppSettings _settings;
        private const string CannotInitiateSession = "Cannot Initiate Session now";
        private ISPSessionStateItemCollection2 _sessionItems;
        private readonly Func<ISPSession, bool> _tryEstablish;        
        //TODO: parse this flag
        private readonly bool _isReadonly;
        private readonly object locker = new object();
        private bool _isNew;
        private bool _liquid;
        private bool _reEntrance;
        private bool _isAbandoned;
        //becomes true if session is not new but requested, loaded as late as possible!
        private bool _wasLoaded;
        //minutes
        private int _timeOut;
        internal ISPSession(string sessionKey, Func<ISPSession, bool> tryEstablish,bool isNewSessionKey, SessionAppSettings settings)
        {
            if(settings == null)
            {
                throw new ArgumentNullException(nameof(settings),"SessionAppSettings must be provided");
            }
            if (string.IsNullOrEmpty(settings.DatabaseConnection))
            {
                throw new InvalidOperationException("SessionAppSettings.DataBaseConnection not given");

            }
            if ((settings.EnableLogging & 3)== 3)
            {
                Helpers.TraceInfo.TraceInfo = true;
                Helpers.TraceInfo.TraceError = true;
            }
            if (tryEstablish == null)
            {
                throw new ArgumentNullException(nameof(tryEstablish));
            }
            _tryEstablish = tryEstablish;
            _settings = settings;           
            _isNew = isNewSessionKey;
            _timeOut = _settings.SessionTimeout;
            _liquid = _settings.Liquid;
            _reEntrance = _settings.ReEntrance;
            _sessionItems =  new ISPSessionStateItemCollection2() { SessionID = sessionKey };            
        }

        private void InitItems(ISPSessionStateItemCollection items)
        {
            lock (locker)
            {
                //we must avoid a second initialisation, which occurs outside our MVC View (e.g.) or API controlller
                // when e.g. a javascript is loaded. ASP.NET core runs all middleware thus, the session as well
                
                // when the session is abandoned, we must deal with it as if new
                if (items == null)
                {
                    _isNew = true;

                    return;
                }
                this._sessionItems = items.Items;
                this._sessionItems.IsNew = _isNew;                
                this._sessionItems.Timeout = _timeOut;
                this._sessionItems.IsReadOnly = _isReadonly;
                
            }
        }
        private void CheckLoad()
        {
            if (!_wasLoaded && IsNewSession == false)
            {
                lock (locker)
                {
                    _wasLoaded = true;
                    LoadAsync().Wait();
                }
            }
        }
        public object this[string name]
        {
            get
            {
                if (!_tryEstablish(this))
                {
                    throw new InvalidOperationException(CannotInitiateSession);
                }
                CheckLoad();
                return _sessionItems[name];
            }

            set
            {
                if (!_tryEstablish(this))
                {
                    throw new InvalidOperationException(CannotInitiateSession);
                }
                CheckLoad();
                _sessionItems[name] = value;
            }
        }

        public object this[int index]
        {
            get
            {
                if (!_tryEstablish(this))
                {
                    throw new InvalidOperationException(CannotInitiateSession);
                }
                CheckLoad();
                return _sessionItems[index];
            }

            set
            {
                if (!_tryEstablish(this))
                {
                    throw new InvalidOperationException(CannotInitiateSession);
                }
                CheckLoad();
                _sessionItems[index] = value;
            }
        }

        public int Count
        {
            get
            {
                CheckLoad();
                return _sessionItems.Count;
            }
        }

        public string Id
        {
            get
            {
                return _sessionItems.SessionID;
            }
        }

        public bool IsAvailable
        {
            get
            {
                return true;
            }
        }

        public bool IsNewSession
        {
            get
            {
                return _isNew;
            }
        }

        public bool IsReadOnly
        {
            get
            {
                return _isReadonly;
            }
        }

        public IEnumerable<string> Keys
        {
            get
            {
                CheckLoad();
                return _sessionItems.Keys;                
            }
        }

        public int LCID
        {
            get
            {
                throw new NotImplementedException();
            }

            set
            {
                throw new NotImplementedException();
            }
        }

        public string SessionID
        {
            get
            {
                return _sessionItems.SessionID;
            }
        }

        public int Timeout
        {
            get
            {
                return _timeOut;
            }

            set
            {
                _timeOut = value;
            }
        }

        public void Abandon()
        {
            if (!_tryEstablish(this))
            {
                throw new InvalidOperationException(CannotInitiateSession);
            }            
            _isAbandoned = true;
            _sessionItems.Clear();
            
            CSessionDL.SessionRemove(_settings, SessionID);            
        }

        public void Add(string name, object value)
        {
            if (!_tryEstablish(this))
            {
                throw new InvalidOperationException(CannotInitiateSession);
            }
            CheckLoad();
            _sessionItems[name] = value;
        }

        public void Clear()
        {
            if (!_tryEstablish(this))
            {
                throw new InvalidOperationException(CannotInitiateSession);
            }
            CheckLoad();
            _sessionItems.Clear();
        }

        public Task CommitAsync()
        {
            //return Task.Run(() =>
            //{
            if (_sessionItems == null)
            {
                return Task.FromResult(0);
            }
            if (_isAbandoned)
            {
                _sessionItems.Clear();//make sure
                                      //TODO: Expire?
                return Task.FromResult(0);
            }


            CSessionDL.SessionSave(_settings, _sessionItems,
                new PersistMetaData(false)
                {
                    Expires = _timeOut,
                    Liquid = _liquid ? (short)-1 : (short)0,
                    ReEntrance = _reEntrance ? (short)-1 : (short)0,
                    LastUpdated = new DBTIMESTAMP()
                });
            return Task.FromResult(0);
            //});

        }

        public IEnumerator GetEnumerator()
        {
            return _sessionItems.GetEnumerator();
        }

        public Task LoadAsync()
        {
            this.InitItems( CSessionDL.SessionGet(_settings, this.SessionID));
            return Task.FromResult(0);
        }

        public void Remove(string key)
        {
            _sessionItems.Remove(key);
        }

        public void RemoveAll()
        {
            _sessionItems.Clear();
        }

        public void RemoveAt(int index)
        {
            _sessionItems.RemoveAt(index);
        }

        public void Set(string key, byte[] value)
        {
            _sessionItems[key] = value;
        }

        public bool TryGetValue(string key, out byte[] value)
        {

            value = null;            
            if (_sessionItems[key]!= null && _sessionItems[key].GetType() == typeof(byte[]))
            {
                value = (byte[])_sessionItems[key];
                return true;
            }
            return false;
        }

        IEnumerator IISPSession.GetEnumerator()
        {
            return _sessionItems.GetEnumerator();
        }
        public bool Liquid
        {
            get { return _liquid; }
            set { _liquid = value; }
        }
        /// <summary>
        /// allows you to pickup a session id from URL, instead of a cookie
        /// Use with care, since the HTTP GET + url, normally gets logged through firewalls and such
        /// </summary>
        public bool ReEntrance
        {
            get { return _liquid; }
            set { _liquid = value; }
        }

    }
}