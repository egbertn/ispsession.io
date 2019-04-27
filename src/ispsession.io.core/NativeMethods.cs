using System;
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
        private static readonly byte[] HashDataLookup =  {
                  0x01, 0x0E, 0x6E, 0x19, 0x61, 0xAE, 0x84, 0x77, 0x8A, 0xAA, 0x7D, 0x76, 0x1B,
                  0xE9, 0x8C, 0x33, 0x57, 0xC5, 0xB1, 0x6B, 0xEA, 0xA9, 0x38, 0x44, 0x1E, 0x07,
                  0xAD, 0x49, 0xBC, 0x28, 0x24, 0x41, 0x31, 0xD5, 0x68, 0xBE, 0x39, 0xD3, 0x94,
                  0xDF, 0x30, 0x73, 0x0F, 0x02, 0x43, 0xBA, 0xD2, 0x1C, 0x0C, 0xB5, 0x67, 0x46,
                  0x16, 0x3A, 0x4B, 0x4E, 0xB7, 0xA7, 0xEE, 0x9D, 0x7C, 0x93, 0xAC, 0x90, 0xB0,
                  0xA1, 0x8D, 0x56, 0x3C, 0x42, 0x80, 0x53, 0x9C, 0xF1, 0x4F, 0x2E, 0xA8, 0xC6,
                  0x29, 0xFE, 0xB2, 0x55, 0xFD, 0xED, 0xFA, 0x9A, 0x85, 0x58, 0x23, 0xCE, 0x5F,
                  0x74, 0xFC, 0xC0, 0x36, 0xDD, 0x66, 0xDA, 0xFF, 0xF0, 0x52, 0x6A, 0x9E, 0xC9,
                  0x3D, 0x03, 0x59, 0x09, 0x2A, 0x9B, 0x9F, 0x5D, 0xA6, 0x50, 0x32, 0x22, 0xAF,
                  0xC3, 0x64, 0x63, 0x1A, 0x96, 0x10, 0x91, 0x04, 0x21, 0x08, 0xBD, 0x79, 0x40,
                  0x4D, 0x48, 0xD0, 0xF5, 0x82, 0x7A, 0x8F, 0x37, 0x69, 0x86, 0x1D, 0xA4, 0xB9,
                  0xC2, 0xC1, 0xEF, 0x65, 0xF2, 0x05, 0xAB, 0x7E, 0x0B, 0x4A, 0x3B, 0x89, 0xE4,
                  0x6C, 0xBF, 0xE8, 0x8B, 0x06, 0x18, 0x51, 0x14, 0x7F, 0x11, 0x5B, 0x5C, 0xFB,
                  0x97, 0xE1, 0xCF, 0x15, 0x62, 0x71, 0x70, 0x54, 0xE2, 0x12, 0xD6, 0xC7, 0xBB,
                  0x0D, 0x20, 0x5E, 0xDC, 0xE0, 0xD4, 0xF7, 0xCC, 0xC4, 0x2B, 0xF9, 0xEC, 0x2D,
                  0xF4, 0x6F, 0xB6, 0x99, 0x88, 0x81, 0x5A, 0xD9, 0xCA, 0x13, 0xA5, 0xE7, 0x47,
                  0xE6, 0x8E, 0x60, 0xE3, 0x3E, 0xB3, 0xF6, 0x72, 0xA2, 0x35, 0xA0, 0xD7, 0xCD,
                  0xB4, 0x2F, 0x6D, 0x2C, 0x26, 0x1F, 0x95, 0x87, 0x00, 0xD8, 0x34, 0x3F, 0x17,
                  0x25, 0x45, 0x27, 0x75, 0x92, 0xB8, 0xA3, 0xC8, 0xDE, 0xEB, 0xF8, 0xF3, 0xDB,
                  0x0A, 0x98, 0x83, 0x7B, 0xE5, 0xCB, 0x4C, 0x78, 0xD1 };

        /// <summary>
        /// emulates Shlwapi HashData
        /// </summary>
        internal static uint HashData(byte[] lpSrc, int nSrcLen, ref byte[] lpDest, int nDestLen)
        {
            int srcCount = nSrcLen - 1, destCount = nDestLen - 1;
            if (lpSrc == null || lpDest == null)
                return NativeMethods.E_INVALIDARG;
            while (destCount >= 0)
            {
                lpDest[destCount] = (byte)(destCount & 0xff);
                destCount--;
            }

            while (srcCount >= 0)
            {
                destCount = nDestLen - 1;
                while (destCount >= 0)
                {
                    lpDest[destCount] = HashDataLookup[lpSrc[srcCount] ^ lpDest[destCount]];
                    destCount--;
                }
                srcCount--;
            }
            return S_OK;
        }
        
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
        //internal unsafe static double ToOaDate(DateTime value)
        //{
        //    _SYSTEMTIME st;

        //    st.wYear = (short)value.Year;
        //    st.wMonth = (short)value.Month;
        //    st.wDay = (short)value.Day;
        //    st.wHour = (short)value.Hour;
        //    st.wMinute = (short)value.Minute;
        //    st.wMilliseconds = (short)value.Millisecond;
            
        //    double d;
        //    SystemTimeToVariantTime(&st, &d);
        //    return d;
        //}
        //internal unsafe static DateTime FromOADate(double d)
        //{
        //    _SYSTEMTIME st ;
        //    VariantTimeToSystemTime(d, &st);
        //    return  new DateTime(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wMilliseconds);
        ////}
        //internal unsafe static long ToOACurrency(this decimal value)
        //{
        //    tagDECIMAL ll;
        //    tagVARIANT theDec;
        //    Buffer.MemoryCopy(&value, &ll, sizeof(decimal), sizeof(decimal));
        //    ll.wReserved = (ushort)VarEnum.VT_DECIMAL;
        //    VariantChangeTypeEx(&theDec, &ll, 1033, 0, (short)VarEnum.VT_CY);
        //    return theDec.llVal;
        //}
        //internal unsafe static decimal FromOACurrency(long i64)
        //{
        //    tagDECIMAL theDec;
        //    //fake it to be a CUR
        //    tagVARIANT ll;
        //    ll.vt = VarEnum.VT_CY;
        //    ll.llVal = i64;
        //    VariantChangeTypeEx(&theDec, &ll, 1033, 0, (short) VarEnum.VT_DECIMAL);
        //    //decimal retVal;
        //    //theDec.wReserved = 0;
        //   // Buffer.MemoryCopy(&theDec, &retVal, sizeof(decimal), sizeof(decimal));            
        //    return new decimal(theDec.Lo32, theDec.Mid32, theDec.Hi32, theDec.sign == 0x80, theDec.scale);

        //}
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
