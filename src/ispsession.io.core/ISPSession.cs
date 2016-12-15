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
        private readonly string _sessionId;
        //TODO: parse this flag
        private readonly bool _isReadonly;
        private readonly bool _isNew;
        private bool _liquid;
        private bool _reEntrance;
        private bool _isAbandoned;
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
            _sessionId = sessionKey;
            _isNew = isNewSessionKey;
            _timeOut = _settings.SessionTimeout;
            _liquid = _settings.Liquid;
            _reEntrance = _settings.ReEntrance;
         
        }

        internal void InitItems(ISPSessionStateItemCollection items)
        {
            this._sessionItems = items.Items;
            this._sessionItems.IsNew = _isNew;
            this._sessionItems.SessionID = _sessionId;
            this._sessionItems.Timeout = _timeOut;
            this._sessionItems.IsReadOnly = _isReadonly;            
        }

        public object this[string name]
        {
            get
            {
                if (!_tryEstablish(this))
                {
                    throw new InvalidOperationException(CannotInitiateSession);
                }
                return _sessionItems[name];
            }

            set
            {
                if (!_tryEstablish(this))
                {
                    throw new InvalidOperationException(CannotInitiateSession);
                }
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
                return _sessionItems[index];
            }

            set
            {
                if (!_tryEstablish(this))
                {
                    throw new InvalidOperationException(CannotInitiateSession);
                }
                _sessionItems[index] = value;
            }
        }

        public int Count
        {
            get
            {
                return _sessionItems.Count;
            }
        }

        public string Id
        {
            get
            {
                return _sessionId;
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
                return _sessionId;
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
            CSessionDL.SessionRemove(_settings, _sessionId);            
        }

        public void Add(string name, object value)
        {
            if (!_tryEstablish(this))
            {
                throw new InvalidOperationException(CannotInitiateSession);
            }
            _sessionItems[name] = value;
        }

        public void Clear()
        {
            if (!_tryEstablish(this))
            {
                throw new InvalidOperationException(CannotInitiateSession);
            }
            _sessionItems.Clear();
        }

        public Task CommitAsync()
        {
            return Task.Run(() =>
            {
                if (_sessionItems == null)
                {
                    return;
                }
                if (_isAbandoned)
                {
                    _sessionItems.Clear();//make sure
                    //TODO: Expire?
                   return;
                }

                
                CSessionDL.SessionSave(_settings, _sessionItems,
                    new PersistMetaData(false)
                    {
                        Expires = _timeOut,
                        Liquid = _liquid ? (short)-1 : (short)0,
                        ReEntrance = _reEntrance ? (short)-1 : (short)0,
                        LastUpdated = new DBTIMESTAMP()
                    });
            });

        }

        public IEnumerator GetEnumerator()
        {
            return _sessionItems.GetEnumerator();
        }

        public Task LoadAsync()
        {
            throw new NotImplementedException();
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