using System;
using System.Collections;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace ispsession.io
{
    public class ISPSession : IISPSession
    {
        private readonly SessionAppSettings _settings;
        private readonly ISPSessionStateItemCollection2 _sessionItems;
        private readonly string _sessionId;
        //TODO: parse this flag
        private readonly bool _isReadonly;
        private readonly bool _isNew;
        private bool _liquid;
        private bool _reEntrance;
        private bool _isAbandoned;
        //minutes
        private int _timeOut;
        internal ISPSession(string sessionKey, bool isNewSessionKey, SessionAppSettings settings)
        {
            ISPSessionStateItemCollection items;
            _settings = settings;
            _sessionId = sessionKey;
            _isNew = isNewSessionKey;
            _timeOut = _settings.SessionTimeout;
            _liquid = _settings.Liquid;
            _reEntrance = _settings.ReEntrance;
            if (!isNewSessionKey)
            {
                items = CSessionDL.SessionGet(_settings, sessionKey);             
            }
            else
            {
                items = new ISPSessionStateItemCollection();
                CSessionDL.SessionInsert(_settings, sessionKey, new PersistMetaData()
                {
                    Expires = _settings.CookieExpires,
                    LastUpdated = new DBTIMESTAMP(),
                    Liquid = _liquid ? (short)-1: (short)0,
                        ReEntrance = _reEntrance ? (short)-1: (short)0
                }
                );
                items.Items = new ISPSessionStateItemCollection2();
            }
            _sessionItems = items.Items;
        }
        public object this[string name]
        {
            get
            {
                return _sessionItems[name];
            }

            set
            {
                _sessionItems[name] = value;
            }
        }

        public object this[int index]
        {
            get
            {
                return _sessionItems[index];
            }

            set
            {
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
            _isAbandoned = true;
            _sessionItems.Clear();
            CSessionDL.SessionRemove(_settings, _sessionId);            
        }

        public void Add(string name, object value)
        {
            _sessionItems[name] = value;
        }

        public void Clear()
        {
            _sessionItems.Clear();
        }

        public Task CommitAsync()
        {
            return Task.Run(() =>
            {
                if (_isAbandoned)
                {
                    _sessionItems.Clear();//make sure
                   return;
                }


                CSessionDL.SessionSave(_settings, _sessionItems,
                    new PersistMetaData()
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