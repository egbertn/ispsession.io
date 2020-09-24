﻿using System;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;
using ispsession.io.core.Interfaces;

namespace ispsession.io.core
{
    internal sealed class MemHandle : SafeHandle
    {
        private const int GMEM_INVALID_HANDLE = 0x8000;
        [DllImport(NativeMethods.KERNEL32)]
        private static extern void GlobalFree(SafeHandle handle);
        public MemHandle() : base(new IntPtr(GMEM_INVALID_HANDLE), true)
        {

        }
        public override bool IsInvalid => base.handle == new IntPtr(GMEM_INVALID_HANDLE);

        protected override bool ReleaseHandle()
        {
            if (!IsInvalid && !IsClosed)
            {
                GlobalFree(this);
                this.SetHandleAsInvalid();
                return true;
            }
            return false;
        }
    }

    internal static class NativeMethods
    {
        internal const string KERNEL32 = "kernel32.dll";
        internal const string OLE32 = "ole32.dll";

        internal const uint STG_INVALID_POINTER = 0x80030009;
        internal const uint S_FALSE = 0x0001;
        internal const uint S_OK = 0x0000;
        internal const uint E_INVALIDARG = 0x80070057;
        internal const uint E_OUTOFMEMORY = 0x8007000E;
        internal const uint STG_E_INVALIDPOINTER = 0x80030009;
        internal const uint STG_E_INVALIDFUNCTION = 0x80030001;
        internal const uint E_NOTIMPL = 0x80004001;

        internal enum AllocFlags
        {
            GHND = GMEM_MOVABLE | GMEM_ZEROINIT,
            GMEM_FIXED = 0x0,
            GMEM_MOVABLE = 0x0002,
            GMEM_ZEROINIT = 0x0040,
            GPTR = GMEM_FIXED | GMEM_ZEROINIT
        }
        [DllImport(KERNEL32, SetLastError = true)]
        internal static extern MemHandle GlobalAlloc(AllocFlags flags, [MarshalAs(UnmanagedType.SysInt)] IntPtr size);

        [DllImport(KERNEL32)]
        [return: MarshalAs(UnmanagedType.SysInt)]
        internal static extern UIntPtr GlobalSize(MemHandle handle);
        [DllImport(KERNEL32, SetLastError = true)]
        internal static extern IntPtr GlobalLock(MemHandle handle);

        [DllImport(KERNEL32)]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool GlobalUnlock(MemHandle handle);

        [DllImport("ole32.dll", ExactSpelling = true)]
        internal static extern int OleLoadFromStream(IStream pStorage, [In] ref Guid iid, [MarshalAs(UnmanagedType.IUnknown)] out object ppvObj);
        [DllImport("ole32.dll", ExactSpelling = true)]
        internal static extern int OleSaveToStream(IPersistStream pPersistStream, IStream pStream);
        [DllImport("ole32.dll", ExactSpelling = true)]
        internal static extern int CreateStreamOnHGlobal(SafeHandle hGlobal, bool fDeleteOnRelease,
           out IStream ppstm);

        const int ErrorSuccess = 0;
        internal sealed class JoinInformation
        {
            public string Domain { get; internal set; }
            public string WorkGroup { get; internal set; }
        }
        internal unsafe static JoinInformation GetJoinedDomain()
        {
            var retVal = new JoinInformation();

            var workstation = Environment.MachineName;
            var domainName = Environment.UserDomainName;
            if (workstation == domainName)
            {
                return retVal; // they are equal so not joined to a domein
            }
            retVal.Domain = domainName;
            return retVal;
        }

        internal unsafe static DOMAIN_CONTROLLER_INFO GetDomainInfo()
        {

            var domainInfo = new DOMAIN_CONTROLLER_INFO() { DomainName = System.Net.NetworkInformation.IPGlobalProperties.GetIPGlobalProperties().DomainName.ToLowerInvariant() };
            return domainInfo;
        }

        internal static string GetNetBiosName(bool giveDnsName = false)
        {

            return Environment.MachineName;

        }
    }
}
