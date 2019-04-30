using System;
using System.Collections;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using ispsession.io.core.Interfaces;
using Microsoft.AspNetCore.Http;

namespace ispsession.io.core
{
    public sealed class ISPSession : IISPSession
    {
        private readonly SessionAppSettings _settings;
        private const string CannotInitiateSession = "Cannot Initiate Session now";
        private ISPSessionStateItemCollection2 _sessionItems;
        private readonly Func<ISPSession, Task<bool>> _tryEstablish;
        //TODO: parse this flag
        private readonly bool _isReadonly;
        private readonly object locker = new object();
        private bool _liquid;
        private bool _reEntrance;
        private bool _isAbandoned;
        //becomes true if session is not new but requested, loaded as late as possible!
        private bool _wasLoaded;
        //minutes
        private int _timeOut;
        internal ISPSession(string sessionKey,  Func<ISPSession, Task< bool>> tryEstablish, bool isNewSessionKey, SessionAppSettings settings)
        {
            if (settings == null)
            {
                throw new ArgumentNullException(nameof(settings), "SessionAppSettings must be provided");
            }
            if (string.IsNullOrEmpty(settings.DatabaseConnection))
            {
                throw new InvalidOperationException("SessionAppSettings.DataBaseConnection not given");

            }
            if ((settings.EnableLogging & 3) == 3)
            {
                StreamManager.TraceInfo.TraceInfo = true;
                StreamManager.TraceInfo.TraceError = true;
            }
            _tryEstablish = tryEstablish ?? throw new ArgumentNullException(nameof(tryEstablish));
            _settings = settings;
            IsNewSession = isNewSessionKey;
            if (_settings.SessionTimeout != null)
            {
                _timeOut = _settings.SessionTimeout.Value;
            }
            else
            {
                _timeOut = 30;//minutes
            }
            if (_settings.Liquid != null)
            {
                _liquid = _settings.Liquid.Value;
            }
            if (_settings.ReEntrance != null)
            {
                _reEntrance = _settings.ReEntrance.Value;
            }
            this.SessionID = sessionKey;
           
            _sessionItems = new ISPSessionStateItemCollection2() ;
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
                    IsNewSession = true;

                    return;
                }
                _wasLoaded = true;
                this._sessionItems = items.Items;
                this._sessionItems.IsReadOnly = _isReadonly;
                this._sessionItems.Dirty = false;
                this.Liquid = items.Meta.Liquid == -1;
                this.Timeout = items.Meta.Expires ;

                this.ReEntrance = items.Meta.ReEntrance == -1;
               
            }
        }
        private async Task CheckLoad()
        {
            if (!_wasLoaded)
            {
                if (!await _tryEstablish(this))
                {
                    throw new InvalidOperationException(CannotInitiateSession);
                }
                if (IsNewSession == false)
                {
                    lock (locker)
                    {
                        _wasLoaded = true;
                       
                    }
                   // await LoadAsync();
                }
            }
        }
        public object this[string name]
        {
            get
            {
                CheckLoad().Wait();
                return _sessionItems[name];
            }

            set
            {
                CheckLoad().Wait();
                _sessionItems[name] = value;
            }
        }

        public object this[int index]
        {
            get
            {
                CheckLoad().Wait();
                return _sessionItems[index];
            }

            set
            {
                CheckLoad().Wait();
                _sessionItems[index] = value;
            }
        }

        public int Count
        {
            get
            {
                CheckLoad().Wait();
                return _sessionItems.Count;
            }
        }

        public string Id
        {
            get
            {
                return SessionID;
            }
        }

        public bool IsAvailable
        {
            get
            {
                return true;
            }
        }

        public bool IsNewSession { get; set; }
      

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
                CheckLoad().Wait();
                return _sessionItems.Keys;
            }
        }

        public string SessionID { get; set; }
       
        public string OldSessionID { get; set; }

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
            CheckLoad().Wait();
            _isAbandoned = true;
            _isAbandoned = false;
            _sessionItems.Clear();

         
        }

        public void Add(string name, object value)
        {
            CheckLoad().Wait();
            _sessionItems[name] = value;
        }

        public void Clear()
        {
            CheckLoad().Wait();
            _sessionItems.Clear();
        }

        public async Task CommitAsync()
        {
            //return Task.Run(() =>
            //{
            if (_sessionItems == null)
            {
                return;
            }
            if (_isAbandoned)
            {
                await CSessionDL.SessionRemoveAsync(_settings, SessionID);
                return ;
            }


            await CSessionDL.SessionSave(_settings, _sessionItems,
                new PersistMetaData(false)
                {
                    Expires = _timeOut,
                    Liquid = _liquid ? (short)-1 : (short)0,
                    ReEntrance = _reEntrance ? (short)-1 : (short)0,
                    LastUpdated = new DBTIMESTAMP()
                }, SessionID, OldSessionID);
            //});

        }

        public IEnumerator GetEnumerator()
        {
            CheckLoad().Wait();
            return _sessionItems.GetEnumerator();
        }

        public async Task LoadAsync()
        {
            if (_wasLoaded == false)
            {
                this.InitItems(await CSessionDL.SessionGetAsync(_settings, SessionID));
                if (this.Liquid)
                {
                    await _tryEstablish(this);
                }
                _wasLoaded = true;
            }
           
        }

        public void Remove(string key)
        {
            CheckLoad().Wait();
            _sessionItems.Remove(key);
        }

        public void RemoveAll()
        {
            CheckLoad().Wait();
            _sessionItems.Clear();
        }

        public void RemoveAt(int index)
        {
            CheckLoad().Wait();
            _sessionItems.RemoveAt(index);
        }

        public void Set(string key, byte[] value)
        {
            CheckLoad().Wait();
            _sessionItems[key] = value;
        }

        public bool TryGetValue(string key, out byte[] value)
        {
            CheckLoad().Wait();

            value = null;
            if (_sessionItems[key] != null && _sessionItems[key].GetType() == typeof(byte[]))
            {
                value = (byte[])_sessionItems[key];
                return true;
            }
            return false;
        }

        IEnumerator IISPSession.GetEnumerator()
        {
            CheckLoad().Wait();
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
            get { return _reEntrance; }
            set { _reEntrance = value; }
        }
        //
        // Summary:
        //     Store the session in the data store. This may throw if the data store is unavailable.
        public  Task CommitAsync(CancellationToken cancellationToken = default(CancellationToken))
        {
            return CommitAsync();
        }
        //
        // Summary:
        //     Load the session from the data store. This may throw if the data store is unavailable.
        public  Task LoadAsync(CancellationToken cancellationToken = default(CancellationToken))
        {
            return LoadAsync();
        }
    }
}