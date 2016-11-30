using System;
using System.Diagnostics;

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Data.SqlClient;
using System.Data;
using System.Globalization;
using ispsession.io;
using System.Runtime.InteropServices;
using ispsession.io.Interfaces;
using System.Runtime.InteropServices.ComTypes;
using ADODB;

namespace UnitTestProject1
{
    [TestClass]
    public class UnitTest1
    {
        [DllImport("oleaut32.dll", SetLastError = false)]
        internal static extern int VariantChangeTypeEx(out tagVARIANT pvargDest, [In] ref tagVARIANT pvarSrc, int lcid, short wFlags, [MarshalAs(UnmanagedType.I2)] VarEnum vt);

        [StructLayout(LayoutKind.Explicit, Size = 8)]
        public struct CURRENCY
        {
            [FieldOffset(0)]
            public uint Lo;
            [FieldOffset(4)]
            public int Hi;
            [FieldOffset(0)]
            public long int64;
        }
        public enum VarEnum : short
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
        [StructLayout(LayoutKind.Explicit, Size = 16)]
        public struct tagVARIANT
        {
            [FieldOffset(0), MarshalAs(UnmanagedType.I2)]
            public VarEnum vt;

            [FieldOffset(2)]
            public ushort wReserved1;

            [FieldOffset(4)]
            public ushort wReserved2;

            [FieldOffset(6)]
            public ushort wReserved3;

            [FieldOffset(8)]
            public CURRENCY cyVal;

            [FieldOffset(8)]
            public long llVal;

            [FieldOffset(8)]
            public tagDECIMAL decVal;
        }
        [StructLayout(LayoutKind.Sequential, Size = 16)]
        public struct tagDECIMAL
        {
            public ushort wReserved;
            /// Specifies the number of digits to the right of the decimal point and ranges from 0 to 28.
            public byte scale;
            public byte sign; //0x80=negative
            public int Hi32;
            public int Mid32;
            public int Lo32;
        }
        enum doit
        {
            Een,
            Twee,
            Drie
        }
        public static byte[] HexToGuid(string h)
        {
            var bts = new byte[16];
            for (int x= 30,y=15; x>=0; x-=2,y--)
            {
                bts[y] = byte.Parse(h.Substring(x, 2), NumberStyles.AllowHexSpecifier);
            }
            return bts;
            //return new Guid(bts);
        }
        [TestMethod]
        public void TestInteropDomain()
        {
          //  var dom = ADCCure.ISPSession.Helpers.GetDomainInfo();
            var join = ispsession.io.Helpers.GetJoinedDomain();
            var cName = ispsession.io.Helpers.GetNetBiosName(true);
            var hash = ispsession.io.Helpers.GetHashCode2("hello");
            var licensed = ispsession.io.Helpers.LicentieCheck(Helpers.HexToBytes("03715291842D539DB22B5A3068577658"), "MERCURY");
          //  var meta = ADCCure.ISPSession.Helpers.GetMetaData("EXPIREAT");
        }
        [TestMethod]
        public void TestOleSave()
        {
            
            //ADODB._Recordset oRs = (ADODB._Recordset) Activator.CreateInstance(Type.GetTypeFromProgID("ADODB.Recordset.6.0"));
            
            //oRs.Fields.Append("id", ADODB.DataTypeEnum.adVarChar, 100);
            //oRs.CursorLocation = CursorLocationEnum.adUseClient;
            //oRs.Open();
            //oRs.AddNew ();
            //oRs.Fields[0].Value = "Hello World";
            
            ////IPersistStreamInit init = (IPersistStreamInit)oRs;
            //IStream str;
            //Helpers.CreateStreamOnHGlobal(IntPtr.Zero, true, out str);

            //Helpers.OleSaveToStream1(oRs, str);
            ////Marshal.Release(oRsP);            

            //Helpers.OleSaveToStream2(oRs, str); ;

        }
        //[TestMethod]
        //public void TestGetSession()
        //{
        //    var App_Key = HexToGuid("F01FB5D0BCA6CE4C8CC773916F281C32");
        //    var Guid = HexToGuid("EB12C36127C43742AA78E70124DE0688");
        //    using (var conn = new SqlConnection("Data Source=(local);Password=Geen123_;Persist Security Info=True;User ID=aspsession;Initial Catalog=ISPSESSION;"))
        //    using (var cmd = new SqlCommand("EXEC dbo.pSessionFetch @App_Key, @GUID", conn))
        //    {
        //        var pars = cmd.Parameters;
        //        cmd.CommandType = CommandType.Text;
        //        pars.Add("@GUID", SqlDbType.Binary, 16).Value = Guid;
        //        pars.Add("@App_Key", SqlDbType.Binary, 16).Value = App_Key;
        //        conn.Open();
        //            // Retrieve session item data from the data source.
        //        var reader = cmd.ExecuteReader(CommandBehavior.SingleRow | CommandBehavior.CloseConnection);
        //        if (reader.Read())
        //        {
        //            var str = reader.GetStream(7);
                  
        //            var items = PersistUtil.LocalLoad(str);
        //            Assert.IsTrue((string)items["a0"] == "Smith");
        //            //Assert.IsTrue((DateTime)items["a2"] == new DateTime(2014,09,05,11,18,43));//05/09/2014 11:18:43                    
        //            var testit = (string)((object[,])items["a3"])[1, 1] == "Bye";

        //        }
                

        //    }
        //}
        //[TestMethod]
        //public void TestGetAndSetSession()
        //{
        //    var App_Key = HexToGuid("F01FB5D0BCA6CE4C8CC773916F281C32");
        //    var Guid = HexToGuid("EB12C36127C43742AA78E70124DE0688");
        //    using (var conn = new SqlConnection("Data Source=(local);Password=Geen123_;Persist Security Info=True;User ID=aspsession;Initial Catalog=ISPSESSION;"))
        //    using (var cmd = new SqlCommand("EXEC dbo.pSessionFetch @App_Key, @GUID", conn))
        //    {
        //        var pars = cmd.Parameters;
                
        //        cmd.CommandType = CommandType.Text;
        //        pars.Add("@GUID", SqlDbType.Binary, 16).Value = Guid;
        //        pars.Add("@App_Key", SqlDbType.Binary, 16).Value = App_Key;
        //        conn.Open();
        //        // Retrieve session item data from the data source.
        //        var reader = cmd.ExecuteReader(CommandBehavior.SingleRow | CommandBehavior.CloseConnection);
        //        if (reader.Read())
        //        {
        //            var str = reader.GetStream(7);
                    
        //            var items = PersistUtil.LocalLoad(str);
        //            Assert.IsTrue((string)items["a0"] == "Smith");
        //            //Assert.IsTrue((DateTime)items["a2"] == new DateTime(2014,09,05,11,18,43));//05/09/2014 11:18:43                    
        //            var testit = (string)((object[,])items["a3"])[1, 1] == "Bye";
        //            var objChange = (object[,])items["a3"];
        //            var myDoubles = new double[2] { 1.2345D, 23.45D};
        //            items["mydoubles"] = myDoubles;
        //            items["mychar"] = 'a';
        //            var mybytes = new byte[10]  {1,2,3,4,5,6,7,8,9,10};
        //            items["mybytes"] = mybytes;
        //            items["somestr"] = new string[] { "een", "twee", "drie" };
        //            objChange[1, 1] = "Bye + Change!";
        //            int zLen;
        //            var newStream = PersistUtil.LocalContents(items, out zLen, false);

        //        }


        //    }
        //}
        [TestMethod]
        public void TestMethod1()
        {
            tagVARIANT dest;
            tagVARIANT src = new tagVARIANT() { vt = VarEnum.VT_I8, llVal = -123456};
            var res = VariantChangeTypeEx(out dest, ref src, 1033, 0, VarEnum.VT_CY);

             src.llVal = 0;
            src.vt = 0;
            res = VariantChangeTypeEx(out src, ref dest, 1033, 0, VarEnum.VT_I8);
            Console.WriteLine(src.llVal);
            Console.ReadKey();
            //var pietje = StartupType<doit>.StartupTypes;
            //foreach (var x in pietje)
            //{
            //    Trace.TraceInformation("enum value {0} display {1}", x.Value, x.Display);
            //}

        }
    }
}
