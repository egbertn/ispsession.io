using System;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;

namespace ispsession.io
{

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
        } // NETSETUP_JOIN_STATUS
        internal enum COMPUTER_NAME_FORMAT
        {
            ComputerNameNetBIOS,
            ComputerNameDnsHostname,
            ComputerNameDnsDomain,
            ComputerNameDnsFullyQualified,
            ComputerNamePhysicalNetBIOS,
            ComputerNamePhysicalDnsHostname,
            ComputerNamePhysicalDnsDomain,
            ComputerNamePhysicalDnsFullyQualified
        }

        [DllImport("Netapi32.dll", SetLastError = true)]
        internal static extern int NetApiBufferFree(IntPtr Buffer);

        [DllImport("Netapi32.dll", ExactSpelling = true, CharSet = CharSet.Unicode, SetLastError = true)]
        internal static extern int DsGetDcNameW(
            [MarshalAs(UnmanagedType.LPWStr)]
             string ComputerName,
            [MarshalAs(UnmanagedType.LPWStr)]
            string DomainName,
            [In] IntPtr DomainGuid,
            [MarshalAs(UnmanagedType.LPWStr)]
            string SiteName,
            [MarshalAs(UnmanagedType.U4)]
            DSGETDCNAME_FLAGS Flags,
            out IntPtr pDOMAIN_CONTROLLER_INFO
        );
        [DllImport("Netapi32.dll", ExactSpelling = true, CharSet = CharSet.Unicode, SetLastError = true)]
        internal static extern int NetGetJoinInformation([MarshalAs(UnmanagedType.LPWStr)] string server, out IntPtr domain, out NetJoinStatus status);

        //[DllImport("kernel32.dll", SetLastError = false, ExactSpelling = true, CharSet = CharSet.Unicode)]
        //internal static extern void OutputDebugStringW([MarshalAs(UnmanagedType.LPWStr)] string fmt);

        [DllImport("kernel32.dll", SetLastError = true, ExactSpelling = true, CharSet = CharSet.Unicode)]
        internal static extern bool GetComputerNameExW([MarshalAs(UnmanagedType.U4)] COMPUTER_NAME_FORMAT NameType,

           [MarshalAs(UnmanagedType.LPWStr), In] string lpBuffer, ref int lpnSize);

        //[DllImport("kernel32.dll", ExactSpelling = true)]
        //internal static extern IntPtr GlobalLock(IntPtr hMem);
        //[DllImport("kernel32.dll", ExactSpelling = true)]
        //[return: MarshalAs(UnmanagedType.Bool)]
        //internal static extern bool GlobalUnlock(IntPtr hMem);
        [DllImport("Shlwapi.dll", SetLastError = false, ExactSpelling = true)]
        internal unsafe static extern uint HashData([In, MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.U1, SizeParamIndex = 1)]
                                byte[] pbData, int cbData,
                             void* piet,
                            int outputLen);
        [DllImport("oleaut32.dll", SetLastError = false)]
        internal static extern int VariantChangeTypeEx( out tagVARIANT pvargDest, [In] ref tagVARIANT pvarSrc, int lcid, short wFlags, [MarshalAs(UnmanagedType.I2)] VarEnum vt);

        [DllImport("oleaut32.dll", SetLastError = false)]
        internal static extern int VariantTimeToSystemTime(double vtime, [In] ref _SYSTEMTIME lpSystemTime);
        [DllImport("oleaut32.dll", SetLastError = false)]
        internal static extern int SystemTimeToVariantTime([In] ref _SYSTEMTIME lpSystemTime, out double OaDate);

        [DllImport("ole32.dll", ExactSpelling = true)]
        internal static extern int ReadClassStm(IStream pStm, out Guid clsid);
        [DllImport("ole32.dll", ExactSpelling = true)]
        internal static extern int WriteClassStm(IStream pStm, [In] ref Guid clsid);
        [DllImport("ole32.dll", ExactSpelling = true)]
        internal static extern int OleLoadFromStream(IStream pStorage, [In] ref Guid iid, [MarshalAs(UnmanagedType.IUnknown)] out object ppvObj);
        [DllImport("ole32.dll", ExactSpelling = true)]
        internal static extern int OleSaveToStream(IPersistStream pPersistStream, IStream pStream);
        [DllImport("ole32.dll", ExactSpelling = true)]
        internal static extern int CreateStreamOnHGlobal(IntPtr hGlobal, bool fDeleteOnRelease,
           out IStream ppstm);
        internal static double ToOaDate(DateTime value)
        {
            _SYSTEMTIME st = new _SYSTEMTIME()
            {
                wYear = (short)value.Year,
                wMonth = (short)value.Month,
                wDay = (short)value.Day,
                wHour = (short)value.Hour,
                wMinute = (short)value.Minute,
                wMilliseconds = (short)value.Millisecond
            };
            double d;
            NativeMethods.SystemTimeToVariantTime(ref st, out d);
            return d;
        }
        internal static DateTime FromOADate(double d)
        {
            _SYSTEMTIME st = new _SYSTEMTIME();
            NativeMethods.VariantTimeToSystemTime(d, ref st);
            return  new DateTime(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wMilliseconds);
        }
        //[DllImport("ole32.dll", ExactSpelling = true)]
        //internal static extern int GetHGlobalFromStream(IStream pstm, out IntPtr phglobal);

    }
}
