using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;

namespace ispsession.io.Interfaces
{
    //[ComImport, Guid("0c733a30-2a1c-11ce-ade5-00aa0044773d"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    //public unsafe interface ISequentialStream
    //{
    //    [PreserveSig]
    //    uint Read([Out, MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] byte[] pv, uint cb,  uint* pcbRead);
    //    [PreserveSig]
    //    uint Write([MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] byte[] pv, uint cb,  uint* pcbWritten);

    //}
    //[ComImport, Guid("0000000c-0000-0000-C000-000000000046"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    //public unsafe interface IStream: ISequentialStream
    //{
    //    [PreserveSig]
    //    uint Seek(long dlibMove, uint dwOrigin, long* plibNewPosition);
    //    [PreserveSig]
    //    uint SetSize(long libNewSize);
    //    [PreserveSig]
    //    uint CopyTo(IStream pstm, long cb, long* pcbRead,  long* pcbWritten);
    //    [PreserveSig]
    //    uint Commit(uint grfCommitFlags);
    //    [PreserveSig]
    //    uint Revert();
    //    [PreserveSig]
    //    uint LockRegion(long libOffset, long cb, uint dwLockType);
    //    [PreserveSig]
    //    uint UnlockRegion(long libOffset, long cb, uint dwLockType);
    //    [PreserveSig]
    //    uint Stat(out System.Runtime.InteropServices.ComTypes.STATSTG pstatstg, uint grfStatFlag);
    //    [PreserveSig]
    //    uint Clone(out IStream ppstm);
    //}

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