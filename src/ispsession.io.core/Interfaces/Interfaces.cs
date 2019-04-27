using System;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;

namespace ispsession.io.core.Interfaces
{
    /// <summary>
    /// Definition for interface IPersist.
    /// </summary>
    [ComImport, InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Guid("0000010c-0000-0000-C000-000000000046")]
    public interface IPersist
    {
        void GetClassID( /* [out] */ out Guid pClassID);
    };

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