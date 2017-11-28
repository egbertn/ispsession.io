using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Runtime.InteropServices;
using System.Web;
using System.Web.SessionState;
/*
* Some useful docs type mapping from Ole to .NET
http://technet.microsoft.com/en-us/video/2x07fbw8(v=vs.60).aspx
*/
namespace ispsession.io
{
    public sealed class PersistUtil: StreamManager
    {
        public PersistUtil()
        {

        }
        public PersistUtil(Stream stream)
            : base(stream)
        {

        }

        public static byte[] LocalContents(IHttpSessionState items, out int zLen, bool compress)
        {
            var persistUtil = new PersistUtil();
            zLen = 0;

          
            var itemCount = items.Count;
            TraceInformation("LocalContents keycount={0}", itemCount);
            persistUtil.WriteProperty("Els", itemCount);

            // we must write it orderdered,  in order that ISP Session 6.0
            // has the muke in the right loading order (it assumes it to be like that)
            foreach (var key in from string s in items.Keys
                                orderby s descending
                                select s)
            {
                persistUtil.WriteProperty(key, items[key]);
            }
            //retrieve ZLEN, the uncompressed length
            zLen = persistUtil.ResetStream();


            if (compress)
            {
                using (var compressed = new MemoryStream(zLen / 2))
                using (var zip = new DeflateStream(compressed, CompressionMode.Compress, true))
                {
                    persistUtil.Str.CopyTo(zip);
                    zip.Close();//force a flush
                    return compressed.ToArray();
                }
            }
            return persistUtil.GetBytes();//get all as uncompressed
        }
        public byte[] GetBytes()
        {

            var l = (int)Str.Length; //_memoryBuff
            //var retVal = new byte[l];;
            EnsureMemory(l, true);
            Str.Position = 0;//StartAtPosition
            Str.Read(_memoryBuff, 0, l);
            return _memoryBuff;
            
        }
        /// <summary>
        /// sets position to 0 and returns the actual position (zLen)
        /// </summary>
        internal int ResetStream()
        {
            var zLen = (int)Str.Position;
            Str.Position = 0;
            return zLen;
        }
        /// <summary>
        /// The stream must contain only, and only the proper bytes, no meta ;
        /// </summary>
        /// <param name="pSequentialStream">A seekable and writeable stream, ie MemoryStream</param>
        internal static SessionStateItemCollection LocalLoad(Stream pSequentialStream)
        {
            var persistUtil = new PersistUtil(pSequentialStream);

            var retVal = new SessionStateItemCollection();
            string sTemp = persistUtil.ReadProperty(out object varTemp);
            if (sTemp != "Els")
            {
                throw new HttpException("Corrupt Session State");
            }
            var lVDictElements = (int)varTemp;
            if ((lVDictElements < 0) || (lVDictElements > short.MaxValue))
            {
                throw new HttpException("Too many Session variables");
            }

            for (var cx = lVDictElements-1; cx >= 0; cx--)
            {
                sTemp = persistUtil.ReadProperty(out varTemp);
                if (!string.IsNullOrEmpty(sTemp))
                    retVal[sTemp] = varTemp;
            }

            return retVal;
        }

        internal void WriteProperty(string propName, object data)
        {
            WriteString(propName);            
            //this area is written twice
            //first, just to reservce the space
            // later it will be filled with Int32 length size of the position just before
            // the NEXT property
            var keepPointer = (int)Str.Position;
            WriteInt32(keepPointer);

            var vType = ConvertTypeToVtEnum(data);
            WriteInt16((short)vType);
            WriteValue(data, vType);
            TraceInformation("WriteProperty {0}, type={1}", propName, vType);
            var curPos = Str.Position;
            Str.Position = keepPointer;
            WriteInt32((int)curPos);
            Str.Position = curPos; //restore
        }
        
        internal string ReadProperty(out object data)
        {
            string keyName = ReadString();
            var nextPosition = ReadInt32();
            //read next position  these 4 bytes are 'wasted' but could be used to skip unsupported data types, so we 
            // could jump to the next variable and leave the unsupported type 'as is'...

            var vT = (VarEnum)ReadInt16();
            data = ReadValue( vT);
            System.Diagnostics.Contracts.Contract.Assert(Str.Position == nextPosition);
            TraceInformation("ReadProperty {0} type {1}", keyName, vT);
            return keyName;
        }

    }
}