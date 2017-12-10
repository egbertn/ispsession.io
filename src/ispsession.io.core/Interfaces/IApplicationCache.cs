using System;
using System.Collections.Generic;

namespace ispsession.io.core.Interfaces
{
    public interface IApplicationCache : IEnumerable<string>
    {
        object this[string key] { get; set; }

        int Count { get; }
        /// <summary>
        /// removes an application key from the cache
        /// </summary>
        /// <param name="Key"></param>
        void RemoveKey(string Key);
        /// <summary>
        /// removes all application keys from the cache
        /// </summary>
        void RemoveAll();
        /// <summary>
        /// locks a single key so it is protected
        /// </summary>
        /// <param name="Key"></param>
        /// <returns></returns>
        bool LockKey(string Key);
        /// <summary>
        /// unlocks a single key
        /// </summary>
        /// <param name="Key"></param>
        void UnlockKey(string Key);

        void ExpireKeyAt(string vKey, int at);
        bool KeyExists(string Key);
        Type KeyType(string Key);

    };
    internal interface IKeySerializer
    {
        void KeyStates(
            out IList<string> changedKeys,
            out IList<string> newKeys,
            out IList<string> otherKeys,
            out IList<Tuple<string, int>> expireKeys,
            out IList<string> removedKeys);

        byte[] SerializeKey(string Key);
        void DeserializeKey(byte[] binaryString);
    };
}
