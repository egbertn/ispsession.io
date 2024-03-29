﻿using ispsession.io.core;
using ispsession.io.core.Interfaces;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;
using System.Runtime.Serialization.Formatters.Binary;
using System.Threading.Tasks;

namespace ispsession.io
{
    public class ApplicationCache : IApplicationCache, IKeySerializer
    {
        private const string CannotInitiateSession = "Cannot Initiate Cache now";
        private IDictionary<string, ElementModel> _dictionary;
        private readonly HashSet<string> _removed;
        private readonly CacheAppSettings _settings;
        //becomes true if application is not new but requested, loaded as late as possible!
        private bool _wasLoaded;
        private readonly object locker = new object();
        private DateTimeOffset _initStart;
        public ApplicationCache(CacheAppSettings settings)
        {
            _removed = new HashSet<string>();
            _dictionary = new Dictionary<string, ElementModel>();
            if ((settings.EnableLogging & 3) == 3)
            {
                StreamManager.TraceInfo.TraceInfo = true;
                StreamManager.TraceInfo.TraceError = true;
            }
            this._settings = settings ?? throw new ArgumentNullException(nameof(settings));
        }

        private async Task CheckLoad()
        {
            if (!_wasLoaded)
            {
                if (!await LoadAsync())
                {
                    throw new InvalidOperationException(CannotInitiateSession);
                }

                lock (locker)
                {
                    _wasLoaded = true;

                }
               

            }
        }

        public object this[string key]
        {
            get
            {
                CheckLoad().Wait();
                if (!_dictionary.ContainsKey(key))
                {
                    return null;
                }
                var element = _dictionary[key];
                var isSerialized = element.IsSerialized;
                if (isSerialized)
                {
                    element.Value = UnpackObject(element.Value);
                    element.IsSerialized = false;
                }
                return element.Value;
            }
            set
            {
                CheckLoad().Wait();
                var element = new ElementModel() { Value = value };
                if (!_dictionary.ContainsKey(key))
                {
                    element.IsNew = true;
                }
                else
                {
                    element.IsDirty = true;
                }
                _dictionary[key] = element;
            }
        }

        internal async Task<bool> LoadAsync()
        {
            if (_wasLoaded == false)
            {
                _wasLoaded = true;
                _initStart = DateTimeOffset.Now;
                await CSessionDL.ApplicationGet(_settings, this);
                return true;
            }
            return false;
        }

        public async Task CommitAsync()
        {
            await CSessionDL.ApplicationSave(_settings, this, DateTimeOffset.Now - _initStart);
        }

       

        /// <summary>
        /// the number of items in the application
        /// </summary>
        public int Count { get { CheckLoad().Wait(); return _dictionary.Count; } }

        /// <summary>
        /// Sets an existing key to expire given in milliseconds
        /// </summary>
        /// <param name="key"></param>
        /// <param name="at">Milliseconds.</param>
        /// <exception cref="KeyNotFoundException"/>
        public void ExpireKeyAt(string key, int at)
        {
            CheckLoad().Wait();
            if (!_dictionary.ContainsKey(key))
            {
                throw new KeyNotFoundException($"requested key {key} does not exist");
            }
            _dictionary[key].ExpireAt = at;
        }

        public bool KeyExists(string key)
        {
            CheckLoad().Wait();
            return _dictionary.ContainsKey(key);
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="key"></param>
        /// <exception cref="KeyNotFoundException"/>
        /// <exception cref="InvalidOperationException"/>
        public Type KeyType(string key)
        {
            CheckLoad().Wait();
            if (!_dictionary.ContainsKey(key))
            {
                throw new KeyNotFoundException($"requested key {key} does not exist");
            }
            var element = _dictionary[key];
            var isSerialized = element.IsSerialized;
            if (isSerialized)
            {
                element.Value = UnpackObject(element.Value);
                element.IsSerialized = false;
            }
            return element.Value.GetType();
        }
      
        private static object UnpackObject(object value)
        {
            if (!(value is byte[]))
            {
                throw new ArgumentException("Expected byte[] packed object value", nameof(value));
            }
            var memoryBuff = (byte[])value;
            bool isNetObject = memoryBuff[0] == 99 && memoryBuff[1] == 55;
            var bytesInStream = memoryBuff.Length;
            if (!isNetObject)
            {

                //   TraceInformation("Deser Com Object");
                var hglob = NativeMethods.GlobalAlloc(NativeMethods.AllocFlags.GMEM_MOVABLE, new IntPtr(bytesInStream));
                Marshal.Copy(memoryBuff, 0, NativeMethods.GlobalLock(hglob), bytesInStream);
                var size = NativeMethods.GlobalSize(hglob);
                NativeMethods.GlobalUnlock(hglob);
                var hr = NativeMethods.CreateStreamOnHGlobal(hglob, true, out IStream pstr);
                if (hr < 0) Marshal.ThrowExceptionForHR(hr);

                var uknown = new Guid("00000000-0000-0000-C000-000000000046");

                hr = NativeMethods.OleLoadFromStream(pstr, ref uknown, out object data);
                if (hr < 0) Marshal.ThrowExceptionForHR(hr);
                if (data == null)
                {
                    pstr.Seek(0, 0, IntPtr.Zero);//Position = 0
                    //TraceInformation("IPersistStream failed, trying IPersistStreamInit");
                    data = StreamManager.OleLoadFromStream2(pstr, uknown);
                }
                return data;
            }
            else // it is a .NET serializable object
            {
                //  TraceInformation("deser .NET");
                var bFormatter = new BinaryFormatter
                {
                    AssemblyFormat = System.Runtime.Serialization.Formatters.FormatterAssemblyStyle.Simple,
                    TypeFormat = System.Runtime.Serialization.Formatters.FormatterTypeStyle.TypesWhenNeeded
                };
                //correct 99 and 55 with -2 
                using (var mem = new MemoryStream(memoryBuff, 2, bytesInStream - 2))
                {
                   return bFormatter.Deserialize(mem);
                } 
       
                //  TraceInformation("DeSerializing {0}", TraceInfo.TraceInfo ? data.GetType().Name : null);

            }          
            

        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="key"></param>
        /// <exception cref="NotImplementedException"/>
        public bool LockKey(string key)
        {

            throw new NotImplementedException("Redlock is not yet implemented in this version");
            //return false;
        }
        /// <summary>
        /// Removes all keys from the application
        /// </summary>
        public void RemoveAll()
        {
            CheckLoad().Wait();

            foreach (var k in _dictionary.Keys)
            {
                if (!_removed.Contains(k))
                {
                    _removed.Add(k);
                }
            }
            _dictionary.Clear();
        }
        /// <summary>
        /// Removes specified key from cache
        /// </summary>
        /// <param name="key"></param>
        /// <exception cref="KeyNotFoundException"/>
        public void RemoveKey(string key)
        {
            CheckLoad().Wait();

            if (!_dictionary.ContainsKey(key))
            {
                throw new KeyNotFoundException($"requested key {key} does not exist");
            }
            var element = _dictionary[key];
            _dictionary.Remove(key);
            if (!_removed.Contains(key))
            {
                _removed.Add(key);
            }
        }

        public void UnlockKey(string Key)
        {
            throw new NotImplementedException("Redlock is not yet implemented in this version");

        }
        /// <summary>
        /// returns a collection of keys
        /// </summary>
        /// <returns></returns>
        public IEnumerator GetEnumerator()
        {
            CheckLoad().Wait();

            return _dictionary.Keys.GetEnumerator();
        }
        /// <summary>
        /// returns a collection of keys
        /// </summary>
        /// <returns></returns>
        IEnumerator<string> IEnumerable<string>.GetEnumerator()
        {
            CheckLoad().Wait();

            return _dictionary.Keys.GetEnumerator();
        }
        #region IKeySerializer
        void IKeySerializer.KeyStates(
            out IList<string> changedKeys, 
            out IList<string> newKeys, 
            out IList<string> otherKeys, 
            out IList<Tuple<string, int>> expireKeys, 
            out IList<string> removedKeys)
        {
            changedKeys = new List<string>();
            newKeys = new List<string>();
            otherKeys = new List<string>();
            expireKeys = new List<Tuple<string, int>>();
            var removedAry = new string[_removed.Count];
            _removed.CopyTo(removedAry);
            removedKeys = new List<string>(removedAry);
            removedAry = null;
            foreach (var k in _dictionary)
            {
                if (k.Value.IsNew)
                {
                    newKeys.Add(k.Key);
                }
                else if (k.Value.IsDirty)
                {
                    changedKeys.Add(k.Key);
                }
                else
                {
                    otherKeys.Add(k.Key);
                }
                if (k.Value.ExpireAt > 0)
                {
                    expireKeys.Add(new Tuple<string, int>(k.Key, k.Value.ExpireAt));
                }
            }

        }

        byte[] IKeySerializer.SerializeKey(string key)
        {          
            var util = new PersistUtil();
            var element = _dictionary[key];
            util.WriteString(key);
            var vT = StreamManager.ConvertTypeToVtEnum(element.Value);
            util.WriteInt16((short)vT);
            //if we get here, it never is a Serialized object (array) because the key got dirty
            util.WriteValue(element.Value, vT);
            return util.GetBytes();
        }

        void IKeySerializer.DeserializeKey(byte[] binaryString)
        {
            var util = new StreamManager(new MemoryStream(binaryString)) { LateObjectActivation = true };
            var key = util.ReadString();
            var vt = (VarEnum)util.ReadInt16();
            var m = new ElementModel() {  Value = util.ReadValue(vt) };
            if (vt == VarEnum.VT_DISPATCH || vt == VarEnum.VT_UNKNOWN || (vt == VarEnum.VT_VARIANT && m.Value is byte[]))
            {
                m.IsSerialized = true;
            }
            _dictionary[key] = m;
        }
        #endregion

       
    }
}