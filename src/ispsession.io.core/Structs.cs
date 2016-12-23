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
            ReEntrance = 0;
            Liquid = 0;
            ZLen = 0;
        }
        //TODO rename to nice names
        public int SizeofMeta;
        public DBTIMESTAMP LastUpdated;
        public int Expires;
        [MarshalAs(UnmanagedType.I2)]
        public short ReEntrance;
        [MarshalAs(UnmanagedType.I2)]//otherwise, defaults to BOOL(=4 bytes)
        public short Liquid;
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
#pragma warning disable 0649
    //P-Invoke muke   
    internal struct DOMAIN_CONTROLLER_INFO
    {
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
    [StructLayout(LayoutKind.Explicit, Size =8)]
    internal struct tagCURRENCY
    {
        [FieldOffset(0)]
        public uint Lo;
        [FieldOffset(4)]
        public int Hi;
        [FieldOffset(0)]
        public long int64;
    }
    //ms depcrecated this stuff
    internal enum VarEnum:short
    {
        VT_EMPTY = 0,
        VT_NULL = 1,
        VT_I2 = 2,
        VT_I4 = 3,
        VT_R4 = 4,
        VT_R8 = 5,
        VT_CY = 6,
        VT_DATE = 7,
        VT_BSTR = 8,
        VT_DISPATCH = 9,
        VT_ERROR = 10,
        VT_BOOL = 11,
        VT_VARIANT = 12,
        VT_UNKNOWN = 13,
        VT_DECIMAL = 14,
        VT_I1 = 16,
        VT_UI1 = 17,
        VT_UI2 = 18,
        VT_UI4 = 19,
        VT_I8 = 20,
        VT_UI8 = 21,
        VT_INT = 22,
        VT_UINT = 23,
        VT_VOID = 24,
        VT_HRESULT = 25,
        VT_PTR = 26,
        VT_SAFEARRAY = 27,
        VT_CARRAY = 28,
        VT_USERDEFINED = 29,
        VT_LPSTR = 30,
        VT_LPWSTR = 31,
        VT_RECORD = 36,
        VT_FILETIME = 64,
        VT_BLOB = 65,
        VT_STREAM = 66,
        VT_STORAGE = 67,
        VT_STREAMED_OBJECT = 68,
        VT_STORED_OBJECT = 69,
        VT_BLOB_OBJECT = 70,
        VT_CF = 71,
        VT_CLSID = 72,
        VT_VECTOR = 4096,
        VT_ARRAY = 8192,
        VT_BYREF = 16384
    }
    /// <summary>
    /// TODO: add more stuff, .NET has deprecated
    /// </summary>
    //[StructLayout(LayoutKind.Explicit, Size = 16)]
    internal struct tagVARIANT
    {
        //[FieldOffset(0), MarshalAs(UnmanagedType.I2)]
        internal VarEnum vt;

        //[FieldOffset(2)]
        internal ushort wReserved1;

        //[FieldOffset(4)]
        internal ushort wReserved2;

        //[FieldOffset(6)]
        internal ushort wReserved3;

        //[FieldOffset(8)]
        //internal tagCURRENCY cyVal;

        //[FieldOffset(8)]
        internal long llVal;

        //[FieldOffset(8)]
        //internal tagDECIMAL decVal;
    }
  //  [StructLayout(LayoutKind.Sequential, Size =16)]
    internal struct tagDECIMAL
    {
        public ushort wReserved;
        /// Specifies the number of digits to the right of the decimal point and ranges from 0 to 28.
        public byte scale;
        public byte sign; //0x80=negative
        public int Hi32;
        public int Lo32;
        public int Mid32;
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
