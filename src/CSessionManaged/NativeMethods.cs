using ispsession.io.Interfaces;
using System;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;

namespace ispsession.io
{

    internal sealed class MemHandle : SafeHandle
    {
        private const int GMEM_INVALID_HANDLE = 0x8000;
        [DllImport("Kernel32.dll", SetLastError = true)]
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
        [Flags]
        internal enum DSGETDCNAME_FLAGS : uint
        {
            DS_FORCE_REDISCOVERY = 0x00000001,
            DS_DIRECTORY_SERVICE_REQUIRED = 0x00000010,
            DS_DIRECTORY_SERVICE_PREFERRED = 0x00000020,
            DS_GC_SERVER_REQUIRED = 0x00000040,
            DS_PDC_REQUIRED = 0x00000080,
            DS_BACKGROUND_ONLY = 0x00000100,
            DS_IP_REQUIRED = 0x00000200,
            DS_KDC_REQUIRED = 0x00000400,
            DS_TIMESERV_REQUIRED = 0x00000800,
            DS_WRITABLE_REQUIRED = 0x00001000,
            DS_GOOD_TIMESERV_PREFERRED = 0x00002000,
            DS_AVOID_SELF = 0x00004000,
            DS_ONLY_LDAP_NEEDED = 0x00008000,
            DS_IS_FLAT_NAME = 0x00010000,
            DS_IS_DNS_NAME = 0x00020000,
            DS_RETURN_DNS_NAME = 0x40000000,
            DS_RETURN_FLAT_NAME = 0x80000000
        }
        internal enum NetJoinStatus
        {
            NetSetupUnknownStatus = 0,
            NetSetupUnjoined,
            NetSetupWorkgroupName,
            NetSetupDomainName
        } 
        internal enum AllocFlags
        { GHND = GMEM_MOVABLE | GMEM_ZEROINIT,
            GMEM_FIXED = 0x0,
            GMEM_MOVABLE = 0x0002,
            GMEM_ZEROINIT = 0x0040,
            GPTR = GMEM_FIXED | GMEM_ZEROINIT
        }

        [DllImport("Kernel32.dll", SetLastError = true)]
        internal static extern  MemHandle GlobalAlloc(AllocFlags flags, [MarshalAs(UnmanagedType.SysInt)] IntPtr size);

        [DllImport("Kernel32.dll")]
        [return: MarshalAs(UnmanagedType.SysInt)]
        internal static extern UIntPtr GlobalSize(MemHandle handle);
        [DllImport("Kernel32.dll", SetLastError = true)]
        internal static extern IntPtr GlobalLock(MemHandle handle);

        [DllImport("Kernel32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal static extern bool GlobalUnlock(MemHandle handle);

    

        [DllImport("Netapi32.dll")]
        internal unsafe static extern int NetApiBufferFree(void* Buffer);

        [DllImport("Netapi32.dll", ExactSpelling = true, CharSet = CharSet.Unicode, SetLastError = true)]
        internal static extern int DsGetDcNameW(
             string ComputerName,
            string DomainName,
            [In] IntPtr DomainGuid,
            string SiteName,
            [MarshalAs(UnmanagedType.U4)]
            DSGETDCNAME_FLAGS Flags,
            out IntPtr pDOMAIN_CONTROLLER_INFO
        );
        [DllImport("Netapi32.dll")]
        internal unsafe static extern int NetGetJoinInformation(void* server, char** domain, int* status);
        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, ExactSpelling = true)]
        internal static extern void OutputDebugStringW(string debugString);

      
        //[DllImport("kernel32.dll", ExactSpelling = true)]
        //internal static extern IntPtr GlobalLock(IntPtr hMem);
        //[DllImport("kernel32.dll", ExactSpelling = true)]
        //[return: MarshalAs(UnmanagedType.Bool)]
        //internal static extern bool GlobalUnlock(IntPtr hMem);
        [DllImport("Shlwapi.dll")]
        internal unsafe static extern uint HashData(byte* pbData, int cbData,
                            void* piet,
                            int outputLen);
        [DllImport("oleaut32.dll")]
        internal static unsafe extern int VariantTimeToSystemTime(double vtime, _SYSTEMTIME * lpSystemTime);
        [DllImport("oleaut32.dll")]
        internal static unsafe extern int SystemTimeToVariantTime( _SYSTEMTIME* lpSystemTime, double* OaDate);
        [DllImport("ole32.dll", ExactSpelling = true)]
        internal static extern int ReadClassStm(IStream pStm, out Guid clsid);

        [DllImport("ole32.dll", ExactSpelling = true)]
        internal static extern int OleLoadFromStream(IStream pStorage, [In] ref Guid iid, [MarshalAs(UnmanagedType.IUnknown)] out object ppvObj);
        [DllImport("ole32.dll", ExactSpelling = true)]
        internal static extern int OleSaveToStream(IPersistStream pPersistStream, IStream pStream);
        [DllImport("ole32.dll", ExactSpelling = true)]
        internal static extern int CreateStreamOnHGlobal(SafeHandle hGlobal, bool fDeleteOnRelease,
           out IStream ppstm);

        //internal unsafe static double ToOaDate(DateTime value)
        //{
        //    _SYSTEMTIME st;

        //    st.wYear = (short)value.Year;
        //    st.wMonth = (short)value.Month;
        //    st.wDay = (short)value.Day;
        //    st.wHour = (short)value.Hour;
        //    st.wMinute = (short)value.Minute;
        //    st.wSecond = (short)value.Second;
        //    st.wMilliseconds = (short)value.Millisecond;

        //    double d;
        //    NativeMethods.SystemTimeToVariantTime(&st, &d);
        //    return d;
        //}
        //internal unsafe static DateTime FromOADate(double d)
        //{
            
        //    _SYSTEMTIME st;
        //    NativeMethods.VariantTimeToSystemTime(d, &st);
        //    return new DateTime(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
        //}


    }
}
