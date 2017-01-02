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

        [DllImport("Netapi32.dll")]
        internal static extern int NetApiBufferFree(IntPtr Buffer);

        [DllImport("Netapi32.dll", ExactSpelling = true, CharSet = CharSet.Unicode, SetLastError = true)]
        internal static extern int DsGetDcNameW(            
             string ComputerName,            
            string DomainName,
            IntPtr DomainGuid,            
            string SiteName,
            [MarshalAs(UnmanagedType.U4)]
            DSGETDCNAME_FLAGS Flags,
            out IntPtr pDOMAIN_CONTROLLER_INFO
        );
        [DllImport("Netapi32.dll", ExactSpelling = true, CharSet = CharSet.Unicode, SetLastError = true)]
        internal static extern int NetGetJoinInformation(string server, out IntPtr domain, out NetJoinStatus status);

        [DllImport("kernel32.dll", ExactSpelling = true, CharSet = CharSet.Unicode)]
        internal static extern void OutputDebugStringW(string fmt);

        [DllImport("kernel32.dll", SetLastError = true, ExactSpelling = true, CharSet = CharSet.Unicode)]
        internal static extern bool GetComputerNameExW(COMPUTER_NAME_FORMAT NameType,

           string lpBuffer, ref int lpnSize);

        //[DllImport("kernel32.dll", ExactSpelling = true)]
        //internal static extern IntPtr GlobalLock(IntPtr hMem);
        //[DllImport("kernel32.dll", ExactSpelling = true)]
        //[return: MarshalAs(UnmanagedType.Bool)]
        //internal static extern bool GlobalUnlock(IntPtr hMem);
        [DllImport("Shlwapi.dll")]
        internal unsafe static extern uint HashData(byte* pbData, int cbData, void* piet, int outputLen);
        [DllImport("oleaut32.dll")]
        internal static extern unsafe int VariantChangeTypeEx( void* pvargDest, void* pvarSrc, int lcid, short wFlags, short vt);

        [DllImport("oleaut32.dll")]
        internal unsafe static extern int VariantTimeToSystemTime(double vtime,  _SYSTEMTIME* lpSystemTime);
        [DllImport("oleaut32.dll")]
        internal static unsafe extern int SystemTimeToVariantTime(_SYSTEMTIME* lpSystemTime, double* OaDate);

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
        internal unsafe static double ToOaDate(DateTime value)
        {
            _SYSTEMTIME st;

            st.wYear = (short)value.Year;
            st.wMonth = (short)value.Month;
            st.wDay = (short)value.Day;
            st.wHour = (short)value.Hour;
            st.wMinute = (short)value.Minute;
            st.wMilliseconds = (short)value.Millisecond;
            
            double d;
            SystemTimeToVariantTime(&st, &d);
            return d;
        }
        internal unsafe static DateTime FromOADate(double d)
        {
            _SYSTEMTIME st ;
            VariantTimeToSystemTime(d, &st);
            return  new DateTime(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wMilliseconds);
        }
        internal unsafe static long ToOACurrency(this decimal value)
        {
            tagDECIMAL ll;
            tagVARIANT theDec;
            Buffer.MemoryCopy(&value, &ll, sizeof(decimal), sizeof(decimal));
            ll.wReserved = (ushort)VarEnum.VT_DECIMAL;
            VariantChangeTypeEx(&theDec, &ll, 1033, 0, (short)VarEnum.VT_CY);
            return theDec.llVal;
        }
        internal unsafe static decimal FromOACurrency(long i64)
        {
            tagDECIMAL theDec;
            //fake it to be a CUR
            tagVARIANT ll;
            ll.vt = VarEnum.VT_CY;
            ll.llVal = i64;
            VariantChangeTypeEx(&theDec, &ll, 1033, 0, (short) VarEnum.VT_DECIMAL);
            //decimal retVal;
            //theDec.wReserved = 0;
           // Buffer.MemoryCopy(&theDec, &retVal, sizeof(decimal), sizeof(decimal));            
            return new decimal(theDec.Lo32, theDec.Mid32, theDec.Hi32, theDec.sign == 0x80, theDec.scale);

        }
        const int ErrorSuccess = 0;
        internal sealed class JoinInformation
        {
            public string Domain { get; internal set; }
            public string WorkGroup { get; internal set; }
        }
        internal static JoinInformation GetJoinedDomain()
        {
            var retVal = new JoinInformation();

            var pDomain = IntPtr.Zero;
            try
            {
                NetJoinStatus status;
                var result = NetGetJoinInformation(null, out pDomain, out status);
                if (result == ErrorSuccess)
                {
                    switch (status)
                    {
                        case NetJoinStatus.NetSetupDomainName:
                            retVal.Domain = Marshal.PtrToStringUni(pDomain);
                            break;
                        case NetJoinStatus.NetSetupWorkgroupName:
                            retVal.WorkGroup = Marshal.PtrToStringUni(pDomain);
                            break;
                        default:
                            return null; //indicate standalone
                    }
                }
            }
            finally
            {
                if (pDomain != IntPtr.Zero) NetApiBufferFree(pDomain);
            }
            return retVal;
        }

        internal static DOMAIN_CONTROLLER_INFO GetDomainInfo()
        {
            var domainInfo = new DOMAIN_CONTROLLER_INFO();

            var pDci = IntPtr.Zero;
            // var guidClass = new GuidClass();
            try
            {
                var val = DsGetDcNameW(null, null, IntPtr.Zero, null,
                  DSGETDCNAME_FLAGS.DS_DIRECTORY_SERVICE_REQUIRED |
                    DSGETDCNAME_FLAGS.DS_RETURN_DNS_NAME, out pDci);
                //check return value for error
                if (ErrorSuccess == val)
                {
                    domainInfo = Marshal.PtrToStructure<DOMAIN_CONTROLLER_INFO>(pDci);
                }
            }
            finally
            {
                if (pDci != IntPtr.Zero)
                    NetApiBufferFree(pDci);
            }
            return domainInfo;
        }
        internal static string GetNetBiosName(bool giveDnsName = false)
        {
            const int maxComputernameLength = 15;
            var compNameLength = giveDnsName ? 255 : maxComputernameLength;
            var nt4Netbiosname = new string('0', compNameLength);
            compNameLength++;
            return GetComputerNameExW(
                giveDnsName ? COMPUTER_NAME_FORMAT.ComputerNameDnsHostname :
                    COMPUTER_NAME_FORMAT.ComputerNameNetBIOS, nt4Netbiosname, ref compNameLength) ? nt4Netbiosname.Substring(0, compNameLength) : null;
        }

        //[DllImport("ole32.dll", ExactSpelling = true)]
        //internal static extern int GetHGlobalFromStream(IStream pstm, out IntPtr phglobal);

    }
}
