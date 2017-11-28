﻿using ispsession.io.Interfaces;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Runtime.InteropServices;

namespace ispsession.io
{
    public class ApplicationCache : IApplicationCache, IKeyManager
    {
        private readonly IDictionary<string, ElementModel> _dictionary;
        private readonly HashSet<string> _removed;

        public ApplicationCache()
        {
            _removed = new HashSet<string>();
            _dictionary = new Dictionary<string, ElementModel>();
        }
        public object this[string key]
        {
            get
            {
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


        /// <summary>
        /// the number of items in the application
        /// </summary>
        public int Count => _dictionary.Count;

        int IKeyManager.KeyCount => _dictionary.Count;

        /// <summary>
        /// Sets an existing key to expire given in milliseconds
        /// </summary>
        /// <param name="key"></param>
        /// <param name="at">Milliseconds.</param>
        /// <exception cref="KeyNotFoundException"/>
        public void ExpireKeyAt(string key, int at)
        {
            if (!_dictionary.ContainsKey(key))
            {
                throw new KeyNotFoundException($"requested key {key} does not exist");
            }
            _dictionary[key].ExpireAt = at;
        }

        public bool KeyExists(string key)
        {
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
                throw new InvalidOperationException("Expected byte[] packed object value");
            }
            return null;
        }
        public bool LockKey(string key)
        {
            return false;
        }
        /// <summary>
        /// Removes all keys from the application
        /// </summary>
        public void RemoveAll()
        {
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
        }
        /// <summary>
        /// returns a collection of keys
        /// </summary>
        /// <returns></returns>
        public IEnumerator GetEnumerator()
        {
            return _dictionary.Keys.GetEnumerator();
        }
        /// <summary>
        /// returns a collection of keys
        /// </summary>
        /// <returns></returns>
        IEnumerator<string> IEnumerable<string>.GetEnumerator()
        {
            return _dictionary.Keys.GetEnumerator();
        }

        void IKeyManager.KeyStates(
            out IList<string> changedKeys, 
            out IList<string> newKeys, 
            out IList<string> otherKeys, 
            out IList<Tuple<string, int>> expireKeys, 
            out IList<string> removedKeys)
        {
            changedKeys = new Collection<string>();
            newKeys = new Collection<string>();
            otherKeys = new Collection<string>();
            expireKeys = new Collection<Tuple<string, int>>();
            var removedAry = new string[_removed.Count];
            _removed.CopyTo(removedAry);
            removedKeys = new Collection<string>(removedAry);
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

        byte[] IKeyManager.SerializeKey(string key)
        {          
            var util = new PersistUtil();
            var element = _dictionary[key];
            util.WriteString(key);
            var vT = StreamManager.ConvertTypeToVtEnum(element.Value);
            util.WriteValue(element.Value, vT);
            return util.GetBytes();
        }

        void IKeyManager.DeserializeKey(byte[] binaryString)
        {
            var util = new PersistUtil(new MemoryStream(binaryString));
            var key = util.ReadString();
            var vt = (VarEnum)util.ReadInt16();
            var m = new ElementModel() {  Value = util.ReadValue(vt) };
            _dictionary[key] = m;
        }
    }
}