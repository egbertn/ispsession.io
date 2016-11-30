using System;
using System.Runtime.InteropServices;

namespace ispsession.io
{
    public struct DBTIMESTAMP
    {
        public DBTIMESTAMP(bool bogus = false)
        {
            DateTime dt = DateTime.Now;
            year = (short)dt.Year;
            month = (short)dt.Month;
            day = (short)dt.Day;
            hour = (short)dt.Hour;
            minute = (short)dt.Minute;
            second = (short)dt.Second;
            fraction = dt.Millisecond;
        }
        /// <summary>
        /// converts this instance to a DateTime
        /// </summary>
        public DateTime ToDateTime()
        {
            return new DateTime(year, month, day, hour, minute, second, fraction);
        }
      
        public short year;
        public short month;
        public short day;
        public short hour;
        public short minute;
        public short second;
        public int fraction;
    };
    public struct PersistMetaData
    {

        public PersistMetaData(bool bogus)//empty constructor not allowed
        {
            //.net 4.5.1 and higher
            SizeofMeta = Marshal.SizeOf<PersistMetaData>(); 
            LastUpdated = new DBTIMESTAMP();
            Expires = 20;
            ReEntrance = false;
            Liquid = false;
            ZLen = 0;
        }
        //TODO rename to nice names
        public int SizeofMeta;
        public DBTIMESTAMP LastUpdated;
        public int Expires;
        [MarshalAs(UnmanagedType.VariantBool)]
        public bool ReEntrance;
        [MarshalAs(UnmanagedType.VariantBool)]//otherwise, defaults to BOOL(=4 bytes)
        public bool Liquid;
        public int ZLen;
        //warning do not add without changing the size, also take note of memory padding. 
        // If you add a 'bool' it could be rounded to 4 bytes up. So only work with alignment of 4 bytes. (At least, this was the theory)
    };
    //specify 12, otherwise it becomes 16 (rounding to 4 byte padding)
    [StructLayout(LayoutKind.Sequential, Size = 12)]
    internal struct ARRAY_DESCRIPTOR
    {
        public short type;
        public int ElemSize;
        public int Dims;
    };
    internal struct SAFEARRAYBOUND
    {
        internal uint cElements;
        internal int lLbound;
    }
    //P-Invoke muke   
    internal struct DOMAIN_CONTROLLER_INFO
    {
#pragma warning disable 0649
        [MarshalAs(UnmanagedType.LPWStr)]
        internal string DomainControllerName;
        [MarshalAs(UnmanagedType.LPWStr)]
        internal string DomainControllerAddress;
        internal uint DomainControllerAddressType;
        internal Guid DomainGuid;
        [MarshalAs(UnmanagedType.LPWStr)]
        internal string DomainName;
        [MarshalAs(UnmanagedType.LPWStr)]
        internal string DnsForestName;
        internal uint Flags;
        [MarshalAs(UnmanagedType.LPWStr)]
        internal string DcSiteName;
        [MarshalAs(UnmanagedType.LPWStr)]
        internal string ClientSiteName;
    }
    internal struct _SYSTEMTIME
    {
        internal short wYear;
        internal short wMonth;
        internal short wDayOfWeek;
        internal short wDay;
        internal short wHour;
        internal short wMinute;
        internal short wSecond;
        internal short wMilliseconds;
    }
}
