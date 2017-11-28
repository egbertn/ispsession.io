using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;

namespace ispsession.io.Interfaces
{

    public interface IApplicationCache : IEnumerable<string>
    {
        object this[ string key] {get;set;}

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
        /// locks a single key
        /// </summary>
        /// <param name="Key"></param>
        /// <returns></returns>
        bool LockKey(string Key);
        void UnlockKey(string Key);

        void ExpireKeyAt(string vKey, int at);
        bool KeyExists(string Key);
        Type KeyType(string Key);

    };
    internal interface IKeyManager
    {

        int KeyCount { get; }

        void KeyStates(
            out IList<string> changedKeys,
            out IList<string> newKeys,
            out IList<string> otherKeys,
            out IList<Tuple<string, int>> expireKeys,
            out IList<string> removedKeys);

        byte[] SerializeKey(string Key);
        void DeserializeKey(byte[] binaryString);
    };

/// <summary>
/// Definition for interface IPersist. Do not use 
/// </summary>
[ComImport, InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Guid("0000010c-0000-0000-C000-000000000046")]
    public interface IPersist
    {
        void GetClassID( /* [out] */ out Guid pClassID);
    };

    /// <summary>
    /// Defintion for COM interop. Do not use
    /// </summary>
    [ComImport, InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Guid("00000109-0000-0000-C000-000000000046")]
    public interface IPersistStream : IPersist
    {
        [PreserveSig]
        int IsDirty();
        void Load([In] IStream pStm);
        void Save([In] IStream pStm, [In, MarshalAs(UnmanagedType.Bool)] bool fClearDirty);
        void GetSizeMax(out long pcbSize);
    };
    /// <summary>
    /// Definition for COM interop. Do not use
    /// </summary>
    [ComImport, InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Guid("7FD52380-4E07-101B-AE2D-08002B2EC713")]
    public interface IPersistStreamInit : IPersist
    {
        [PreserveSig]
        int IsDirty();
        void Load([In] IStream pStm);
        void Save([In] IStream pStm, [In, MarshalAs(UnmanagedType.Bool)] bool fClearDirty);
        void GetSizeMax(out long pcbSize);
        void InitNew();
    };
}