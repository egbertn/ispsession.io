using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using System.Globalization;
using System.Runtime.InteropServices.ComTypes;
#if NET462 || NET463
using System.Runtime.Serialization.Formatters.Binary;
#endif
using System.Reflection;

namespace ispsession.io
{
    

    // notes: It would be ideally to use BinaryWriter, it handles nearly all types
    // but it is not compatible with ASP Session state. e.g. it writes Strings as UTF7 strings, instead of UTF8.
    // TO achieve the same performance as BinaryWriter, 
    // we have a member byte[] buffer _memoryBuff
    // future optimization using stackalloc &amp; unsafe code UnsafeStreamn
    // 
    public class StreamManager
    {

        protected byte[] _memoryBuff;
        private int _memSize;
        protected readonly Stream Str;
        public StreamManager()
        {
            EnsureMemory(0);
            Str = new MemoryStream();
        }
        public StreamManager(Stream stream)
        {
            EnsureMemory(0);
            Str = stream;
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="required"></param>
        /// <param name="exactSize">if true, will not round up to blocks of 0x1000</param>
        protected void EnsureMemory(int required, bool exactSize = false)
        {
            if (required >= _memSize || exactSize)
            {
                int newSize = exactSize == false ? ((required / 0x1000) + 1) * 0x1000 : required;
                _memoryBuff = new byte[newSize];//do not resize, it is slower
                _memSize = newSize;
            }
        }

        internal static readonly TraceSwitch TraceInfo = new TraceSwitch("ISPSession", "ISPsession Trace Switch");        
        //SIMPLE, ADC_ISPSTATE_PROVIDER_ENT,ADC_ISPSTATE_PROVIDER_ADV
        internal static readonly int[] Productid = { 26,27,28,29 };
        internal readonly static string ProductString = "ISP Session 9.0 State Provider";
        internal readonly static string MessageString = "The ispsession.io Module expired! We would welcome your order at <a href=\"https://ispsession.io/Purchase\">order here</a>";
        internal readonly static string MessageString2 = @"The ispsession.io Module should be licensed. Please contact ADC Cure for an updated license at information @adccure.nl";
        internal readonly static string MessageString3 = "LICENSE ERROR max = {0} requested ={1} \r\n";
        internal static int Maxinstances = 1000;
        
        internal static readonly Encoding _encoding = Encoding.UTF8;
        internal static void TraceError(string message, params object[] args)
        {
            if (TraceInfo.TraceError)
            {
                //TraceLevel.Info=3
                //var fmt = string.Format(message, args);
                //NativeMethods.OutputDebugStringW(fmt);                
                var fmt = $"Error {DateTime.UtcNow} {System.Threading.Thread.CurrentThread.ManagedThreadId} {string.Format(message, args)}";
                Debug.WriteLine(fmt);
            }
        }
        internal static void TraceInformation(string message, params object[] args)
        {
            if (TraceInfo.TraceInfo)
            {
                //var fmt = string.Format(message, args);
                //NativeMethods.OutputDebugStringW(fmt);
                var fmt = $"Information {DateTime.UtcNow} {System.Threading.Thread.CurrentThread.ManagedThreadId} {string.Format(message, args)}";
                Debug.WriteLine(fmt);
            }
        }
        //internal static void TraceInformation(StreamWriter log, string message)
        //{
        //    if (TraceInfo.TraceInfo)
        //    {
        //        log.Flush();
        //        log.BaseStream.Position = 0;
        //        var reader = new StreamReader(log.BaseStream, Encoding.UTF8);
        //        while (!reader.EndOfStream)
        //        {
        //            Trace.TraceInformation("Loginfo {0}", reader.ReadLine());
        //        }
        //    }
        //}


#if OPT
        internal unsafe char ReadChar()
        {
            Str.Read(_memoryBuff, 0, sizeof(char));
            fixed (byte* ptr = this._memoryBuff)
            {
                return *(char*)ptr;
            }
        }
#else
        internal char ReadChar()
        {
            Str.Read(_memoryBuff, 0, sizeof(char));
            return BitConverter.ToChar(_memoryBuff, 0);
        }

#endif

#if OPT
        internal unsafe short ReadInt16()
        {
            Str.Read(_memoryBuff, 0, sizeof(short));

            fixed (byte* ptr = this._memoryBuff)
            {
                return *(short*)ptr;
            }
        }
#else
        internal short ReadInt16()
        {
            Str.Read(_memoryBuff, 0, sizeof(short));
            return BitConverter.ToInt16(_memoryBuff, 0);
        }
#endif


#if OPT
        internal unsafe void WriteInt16(short value)
        {
            fixed (byte* ptr = this._memoryBuff)
            {
                *(short*)ptr = value;
            }
            Str.Write(_memoryBuff, 0, sizeof(short));
        }
#else
        internal void WriteInt16(short value)
        {
            Str.Write(BitConverter.GetBytes(value), 0, sizeof(short));
        }
#endif


#if OPT
        internal unsafe int ReadInt32()
        {
            Str.Read(_memoryBuff, 0, sizeof(int));
            fixed (byte* ptr = this._memoryBuff)
            {
                return *(int*)ptr;
            }
        }
#else
        internal int ReadInt32()
        {
            Str.Read(_memoryBuff, 0, sizeof(int));
            return BitConverter.ToInt32(_memoryBuff, 0);
        }
#endif

#if OPT
        internal unsafe uint ReadUInt32()
        {
            Str.Read(_memoryBuff, 0, sizeof(uint));

            fixed (byte* ptr = this._memoryBuff)
            {
                return *(uint*)ptr;
            }
        }
#else
        internal uint ReadUInt32()
        {
            Str.Read(_memoryBuff, 0, sizeof(uint));        
            return BitConverter.ToUInt32(_memoryBuff, 0);
        }
#endif

#if OPT
        internal unsafe void WriteInt32(int value)
        {
            fixed (byte* ptr = this._memoryBuff)
            {
                *(int*)ptr = value;
                Str.Write(_memoryBuff, 0, sizeof(int));

            }
        }
#else
        internal void WriteInt32(int value)
        {
            Str.Write(BitConverter.GetBytes(value), 0, sizeof(int));
        }
#endif



#if OPT
        internal unsafe void WriteUInt32(uint value)
        {
            fixed (byte* ptr = this._memoryBuff)
            {
                *(uint*)ptr = value;
            }
            Str.Write(_memoryBuff, 0, sizeof(uint));
        }
#else
        internal void WriteUInt32(uint value)
        {
            Str.Write(BitConverter.GetBytes(value), 0, sizeof(uint));
        }
#endif

        /// <summary>
        /// Persists a structure into buffer
        /// </summary>
        /// <typeparam name="T">persistable structure</typeparam>
        /// <param name="value">the structure to persist</param>
        /// <param name="siz">if you leave this 0, the size is calculated</param>
        internal void WriteStruct<T>(T value) where T : struct
        {
            int siz = Marshal.SizeOf<T>();

            var ptPoit = Marshal.AllocHGlobal(siz);
            Marshal.StructureToPtr(value, ptPoit, false);
            Marshal.Copy(ptPoit, _memoryBuff, 0, siz);
            Str.Write(_memoryBuff, 0, siz);
            //Marshal.DestroyStructure(ptPoit, typeof(T));
            Marshal.FreeHGlobal(ptPoit);

        }
        /// <summary>
        /// Retrieves structure of type T from Stream
        /// Note, .NET does not pad the structure to 4 alignment (pad=4 does not help). 
        /// </summary>
        /// <exception cref="OutofMemoryException"/>
        internal T GetStruct<T>() where T : struct
        {
            int  siz = Marshal.SizeOf<T>();
            EnsureMemory(siz);
            Str.Read(_memoryBuff, 0, siz);
            IntPtr ptPoit = IntPtr.Zero;
            try
            {
                ptPoit = Marshal.AllocHGlobal(siz);
                Marshal.Copy(_memoryBuff, 0, ptPoit, siz);
                return Marshal.PtrToStructure<T>(ptPoit);
            }
            catch
            {
                throw; //to do ISP exceptions
            }
            finally
            {
                if (ptPoit != IntPtr.Zero)
                    Marshal.FreeHGlobal(ptPoit);
            }
        }

#if OPT
        internal unsafe long ReadInt64()
        {
            Str.Read(_memoryBuff, 0, sizeof(long));
            fixed (byte* ptr = this._memoryBuff)
            {
                return *(long*)ptr;
            }
        }
#else
         internal long ReadInt64()
        {
            Str.Read(_memoryBuff, 0, sizeof(long));
            return BitConverter.ToInt64(_memoryBuff, 0);
        }
#endif

#if OPT
        internal unsafe ulong ReadUInt64()
        {
            Str.Read(_memoryBuff, 0, sizeof(ulong));
            fixed (byte* ptr = this._memoryBuff)
            {
                return *(ulong*)ptr;
            }
        }
#else
        internal ulong ReadUInt64()
        {
            Str.Read(_memoryBuff, 0, sizeof(ulong));
            return BitConverter.ToUInt64(_memoryBuff, 0);
        }
#endif

#if OPT
        internal unsafe void WriteInt64(long value)
        {
            fixed (byte* ptr = this._memoryBuff)
            {
                *(long*)ptr = value;
            }
             Str.Write(_memoryBuff, 0, sizeof(long));
        }
#else
        internal void WriteInt64(long value)
        {
            Str.Write(BitConverter.GetBytes(value), 0, sizeof(long));
        }
#endif

#if OPT
        internal unsafe void WriteUInt64(ulong value)
        {
            fixed (byte* ptr = this._memoryBuff)
            {
                *(ulong*)ptr = value;
                Str.Write(_memoryBuff, 0, sizeof(ulong));
            }
        }
#else
        internal void WriteUInt64(ulong value)
        {
            Str.Write(BitConverter.GetBytes(value), 0, sizeof(ulong));
        }
#endif

#if OPT
        internal unsafe float ReadFloat()
        {
            Str.Read(_memoryBuff, 0, sizeof(float));
            fixed (byte* ptr = _memoryBuff)
            {
                return *(float*)ptr;
            }
        }
#else
        internal float ReadFloat()
        {
            Str.Read(_memoryBuff, 0, sizeof(float));

            return BitConverter.ToSingle(_memoryBuff, 0);
        }
#endif

#if OPT
        internal unsafe void WriteFloat(float value)
        {
            fixed (byte* ptr = _memoryBuff)
            {
                *(float*)ptr = value;
                Str.Write(_memoryBuff, 0, sizeof(float));
            }
        }

#else
        internal void WriteFloat(float value)
        {
            Str.Write(BitConverter.GetBytes(value), 0, sizeof(float));
        }
#endif

#if OPT
        internal unsafe decimal ReadDecimal()
        {
            Str.Read(_memoryBuff, 0, sizeof(decimal));
            decimal ret;
            fixed (byte* ptr = _memoryBuff)
            {
                *(short*)ptr = 0; //clear vt (VARTYPE)
                Buffer.MemoryCopy(ptr, &ret, sizeof(decimal), sizeof(decimal));
            }
            return ret;
        }
#else
        internal decimal ReadDecimal()
        {
            Str.Read(_memoryBuff, 0, sizeof(decimal));
            _memoryBuff[0] = 0; //clear vt (VARTYPE)
            _memoryBuff[1] = 0;
            var ptr = Marshal.AllocHGlobal(sizeof(decimal));
            Marshal.Copy(_memoryBuff, 0, ptr, sizeof(decimal));
            var dec = Marshal.PtrToStructure<decimal>(ptr);
            Marshal.FreeHGlobal(ptr);
            return dec;
        }
#endif

#if OPT
        internal unsafe void WriteDecimal(decimal value)
        {
            fixed (byte* ptr = _memoryBuff)
            {
                Buffer.MemoryCopy(&value, ptr, sizeof(decimal), sizeof(decimal));
                *(short*)ptr = (short)VarEnum.VT_DECIMAL;
            }
            Str.Write(_memoryBuff, 0, sizeof(decimal));
        }
#else
        internal void WriteDecimal(decimal value)
        {
            var ptr = Marshal.AllocHGlobal(sizeof(decimal));
            Marshal.StructureToPtr(value, ptr, false);            
            Marshal.Copy(ptr, _memoryBuff, 0, sizeof(decimal));            
            Marshal.FreeHGlobal(ptr);
            _memoryBuff[0] = (byte)VarEnum.VT_DECIMAL;
            _memoryBuff[1] = 0; //to be sure
            //var bits = Decimal.GetBits(value);
            //var tgDecimal = new tagDECIMAL() 
            //{  
            //    wReserved = (ushort)VarEnum.VT_DECIMAL,
            //    scale = (byte)((bits[3] >>16)& 31),
            //    sign = (byte)((bits[3] >> 24) & 31),
            //    Hi32 = bits[2],
            //    Mid32 = bits[1],
            //    Lo32 = bits[0]
            //};
            //Buffer.BlockCopy(, 0, _memoryBuff, 0, sizeof(decimal));

            Str.Write(_memoryBuff, 0, sizeof(decimal));
        }
#endif
        internal decimal ReadCurrency()
        {
            return NativeMethods.FromOACurrency(ReadInt64());
        }
        internal void WriteCurrency(decimal value)
        {          
            WriteInt64(value.ToOACurrency());
        }
        internal bool ReadBoolean()
        {
            return ReadInt16() == -1;
        }
        internal void WriteBoolean(bool value)
        {
            WriteInt16(value ? (short)-1 : (short)0);
        }

#if OPT
        internal unsafe double ReadDouble()
        {
            Str.Read(_memoryBuff, 0, sizeof(double));
            fixed (byte* ptr = _memoryBuff)
            {
                return *(double*)ptr;
            }
        }
#else
         internal  double ReadDouble()
        {
            Str.Read(_memoryBuff, 0, sizeof(double));    

            return BitConverter.ToDouble(_memoryBuff, 0);
        }
#endif


#if OPT
        internal unsafe void WriteDouble(double value)
        {
            fixed (byte* ptr = this._memoryBuff)
            {
                *(double*)ptr = value;
                Str.Write(_memoryBuff, 0, sizeof(double));
            }
        }
#else
        internal void WriteDouble(double value)
        {
            Str.Write(BitConverter.GetBytes(value), 0, sizeof(double));
        }
#endif


#if OPT
        internal unsafe void WriteChar(char value)
        {
            fixed (byte* ptr = this._memoryBuff)
            {
                *(char*)ptr = value;
            }
            Str.Write(_memoryBuff, 0, sizeof(char));
        }
#else
        internal void WriteChar(char value)
        {
            Str.Write(BitConverter.GetBytes(value), 0, sizeof(char));
        }
#endif
    
        internal DateTime ReadDateTime()
        {
            return NativeMethods.FromOADate(ReadDouble());
        }
        internal void WriteDateTime(DateTime value)
        {
            WriteDouble(NativeMethods.ToOaDate(value));
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="seekBack">contains the Seeked pointer to which you can seek back. This supports 'peek into' without disurbing the Stream current position</param>        
        internal string ReadString()
        {
            var size = ReadInt32();
            EnsureMemory(size);
            Str.Read(_memoryBuff, 0, size);
            // int wideSize = _encoding.GetCharCount(_memoryBuff, 0, size);
            //var newstr = new char[wideSize];
            TraceInformation("ReadString bytesize={0}", size);

            return _encoding.GetString(_memoryBuff, 0, size);
            // fails because of a bug in .NET?
            //int done =  Encoding.UTF8.GetChars(_memoryBuff, 0, size, newstr, 0);
            //return new string(newstr);
        }
#if OPT
        internal unsafe void WriteString(string value)
#else
        internal void WriteString(string value)
#endif
        {
            var wideSize = value == null ? 0 : value.Length;
            var byteSize = 0;
            if (wideSize > 0)
            {
                
                byteSize = _encoding.GetByteCount(value);
                //on array starting with string length
                EnsureMemory(byteSize + sizeof(int));
                
                _encoding.GetBytes(value, 0, wideSize, _memoryBuff, sizeof(int));
#if OPT
                fixed (byte*ptr = _memoryBuff)
                {
                    *(int*)ptr = byteSize;
                }
#else
                Array.Copy(BitConverter.GetBytes(byteSize), _memoryBuff, sizeof(int));
#endif

                //one single operation, instead of two
                Str.Write(_memoryBuff, 0, byteSize + sizeof(int));
            }
            else
            {
                WriteInt32(0);
            }
            TraceInformation("WriteString size={0}, bytesize={1}", wideSize, byteSize);
        }


        internal void WriteValue(object data, VarEnum vT) //string keyName 
        {
            if ((vT & VarEnum.VT_ARRAY) == VarEnum.VT_ARRAY)
            {
                var psa = (Array)data;
                var ElSize = GetElementSize(vT ^ VarEnum.VT_ARRAY);
                var cDims = psa.Rank;
                var psaBound = new SAFEARRAYBOUND[cDims];
                var descriptor = new ARRAY_DESCRIPTOR()
                {
                    type = (short)vT,
                    ElemSize = ElSize,
                    Dims = cDims
                };
                var objType = psa.GetType();
                //jagged has syntax e.g. new string[][]
                Type elementType;
                var isJagged = (elementType = objType.GetElementType()).IsArray;

                TraceInformation("writing array type={0}, cDims {1}, ElSize {2}", vT, cDims, ElSize);
                WriteStruct(descriptor);

                vT ^= VarEnum.VT_ARRAY;
                // we must break compatibility with ISP SEssion 6.0
                // write the type!
                if (vT == VarEnum.VT_UNKNOWN || vT == VarEnum.VT_ERROR || vT == VarEnum.VT_VARIANT) //object
                {

                    WriteString(elementType.AssemblyQualifiedName);

                }
                else if (isJagged)
                {
                    WriteString("__jagged");
                    WriteString(elementType.FullName);
                }
                else
                {
                    WriteString(null); //write a null string
                }

                //an array should have at least one element Otherwise it's type would be VT_NULL or VT_EMPTY
                //and never get at this execution point
                var lMemSize = 1;
                for (var cx = 0; cx < cDims; cx++)
                {
                    SAFEARRAYBOUND sb;
                    sb.cElements = (uint)psa.GetUpperBound(cx);
                    sb.lLbound = psa.GetLowerBound(cx);
                    sb.cElements = sb.cElements - (uint)sb.lLbound + 1;
                    WriteStruct(sb);
                    psaBound[cx] = sb;
                    lMemSize *= (int)sb.cElements;
                }
                var lElements = lMemSize;
                lMemSize *= ElSize;
                if (!isJagged && ((vT == VarEnum.VT_I1) || (vT == VarEnum.VT_UI2) ||
                    (vT == VarEnum.VT_UI1) || (vT == VarEnum.VT_I2) || (vT == VarEnum.VT_I4) || (vT == VarEnum.VT_UI8) || (vT == VarEnum.VT_UI4) || (vT == VarEnum.VT_R4) ||
                    (vT == VarEnum.VT_R8) || (vT == VarEnum.VT_CY) || (vT == VarEnum.VT_DATE) || (vT == VarEnum.VT_BOOL) ||
                    (vT == VarEnum.VT_I8) )
                    )
                {
                    var straightToStream = vT == VarEnum.VT_UI1 && cDims == 1;
                    if (!straightToStream)
                    {
                        EnsureMemory(lMemSize);
                        Buffer.BlockCopy(psa, 0, _memoryBuff, 0, lMemSize);
                    }
                    Str.Write(straightToStream ? (byte[])psa : _memoryBuff, 0, lMemSize);
                }
                else if (!isJagged && ((vT == VarEnum.VT_BSTR || vT == VarEnum.VT_DECIMAL) && lElements > 0))
                {
                    var rgIndices = new int[cDims];
                    for (var x = 0; x < cDims; x++)
                    {
                        rgIndices[x] = psaBound[x].lLbound;
                    }
                    var dimPointer = 0; // next dimension will first be incremented
                    var findEl = 0;
                    for (;;)
                    {
                        if (rgIndices[dimPointer] <
                            (int)psaBound[dimPointer].cElements + psaBound[dimPointer].lLbound)
                        {
                            switch (vT)
                            {
                                case VarEnum.VT_BSTR:
                                    WriteString((string)psa.GetValue(rgIndices));
                                    break;
                                case VarEnum.VT_DECIMAL:
                                    WriteValue(psa.GetValue(rgIndices), VarEnum.VT_DECIMAL);
                                    break;
                            }
                            rgIndices[dimPointer]++;
                            //end of loop
                            if (++findEl == lElements)
                            {
                                break;
                            }
                        }
                        //carry stuff
                        else
                        {
                            //magic
                            while (++dimPointer <= cDims)
                            {
                                if (rgIndices[dimPointer] < (psaBound[dimPointer].cElements + psaBound[dimPointer].lLbound - 1))
                                {
                                    rgIndices[dimPointer]++;
                                    break;
                                }
                            }
                            //reset previous cols to initial lowerbound from left to 
                            // most right carry position
                            for (int z = 0; z < dimPointer; z++)
                                rgIndices[z] = psaBound[z].lLbound;
                            dimPointer = 0;
                        }

                    }
                }
                else if (isJagged || (vT == VarEnum.VT_VARIANT || vT == VarEnum.VT_UNKNOWN) && lElements > 0)
                {
                    var rgIndices = new int[cDims];
                    for (var x = 0; x < cDims; x++)
                    {
                        rgIndices[x] = psaBound[x].lLbound;
                    }
                    var dimPointer = 0; // next dimension will first be incremented
                    var findEl = 0;
                    for (;;)
                    {
                        if (rgIndices[dimPointer] <
                            (int)psaBound[dimPointer].cElements + psaBound[dimPointer].lLbound)
                        {
                            object pVar = psa.GetValue(rgIndices);
                            var vtype = ConvertTypeToVtEnum(pVar);
                            WriteInt16((short)vtype);
                            //----- recursive call ----- keep an eye on this
                            WriteValue(pVar, vtype);

                            rgIndices[dimPointer]++;
                            //end of loop
                            if (++findEl == lElements)
                            {
                                break;
                            }
                        }
                        //carry stuff
                        else
                        {
                            //magic
                            while (++dimPointer <= cDims)
                            {
                                if (rgIndices[dimPointer] < (psaBound[dimPointer].cElements + psaBound[dimPointer].lLbound - 1))
                                {
                                    rgIndices[dimPointer]++;
                                    break;
                                }
                            }
                            //reset previous cols to initial lowerbound from left to 
                            // most right carry position
                            for (int z = 0; z < dimPointer; z++)
                                rgIndices[z] = psaBound[z].lLbound;
                            dimPointer = 0;
                        }
                    }
                }

            }
            else //not an array but a simple value
            {
                switch (vT)
                {
                    case VarEnum.VT_UI1:
                        Str.WriteByte((byte)data);
                        break;
                    case VarEnum.VT_BOOL:
                        WriteBoolean((bool)data);
                        break;
                    case VarEnum.VT_EMPTY:
                    case VarEnum.VT_NULL:                     
                        // do nothing
                        break;
                    case VarEnum.VT_UI2:
                        WriteChar((char)data);
                        break;
                    case VarEnum.VT_I2:
                        WriteInt16((short)data);
                        break;
                    case VarEnum.VT_I4:
                        WriteInt32((int)data);
                        break;
                    case VarEnum.VT_I8:
                        WriteInt64((long)data);
                        break;
                    case VarEnum.VT_UI4:
                        WriteUInt32((uint)data);
                        break;
                    case VarEnum.VT_UI8:
                        WriteUInt64((ulong)data);
                        break;
                    case VarEnum.VT_R4:
                        WriteFloat((float)data);
                        break;
                    case VarEnum.VT_R8:
                        WriteDouble((double)data);
                        break;
                    case VarEnum.VT_DECIMAL:
                        WriteDecimal((decimal)data);
                        break;
                    case VarEnum.VT_CY:
                        WriteCurrency(((CurrencyWrapper)data).WrappedObject);
                        break;
                    case VarEnum.VT_DATE:
                        WriteDateTime((DateTime)data);
                        break;
                    case VarEnum.VT_BSTR:
                        WriteString((string)data);
                        break;
                    case VarEnum.VT_ERROR:
                    case VarEnum.VT_VARIANT: //object .NET class
                    case VarEnum.VT_UNKNOWN:
                        bool isComObject = isComObject = Marshal.IsComObject(data) && vT != VarEnum.VT_ERROR;
                        if (isComObject)
                        {
                            TraceInformation("Ser Com Object type {0}", TraceInfo.TraceInfo ? data.GetType().FullName : null);
                            IStream pstr;
                            int hr = NativeMethods.CreateStreamOnHGlobal(IntPtr.Zero, true, out pstr);
                            if (hr != 0)
                            {
                                Marshal.ThrowExceptionForHR(hr);
                            }
                            var persist = data as IPersistStream;
                            if (persist != null)
                            {
                                hr = NativeMethods.OleSaveToStream(persist, pstr);
                                Marshal.ReleaseComObject(persist);
                                if (hr != 0)
                                {
                                    Marshal.ThrowExceptionForHR(hr);
                                }
                                persist = null;
                            }
                            //Try IPersistStreamInit
                            else
                            {
                                var persistInit = data as IPersistStreamInit;
                                if (persistInit != null)
                                {
                                    OleSaveToStream2(persistInit, pstr);
                                    Marshal.ReleaseComObject(persistInit);
                                    persistInit = null;
                                }
                                else
                                {
                                    WriteInt32(0);
                                    TraceInformation("COM object NOT written");
                                    break;//dont continue
                                }
                            }
                            pstr.Commit(0);//STGC_DEFAULT
                            System.Runtime.InteropServices.ComTypes.STATSTG stats;
                            pstr.Stat(out stats, 1);//STATFLAG_NONAME
                            var streamLen = (int)stats.cbSize;
                            EnsureMemory(streamLen);
                            pstr.Seek(0, 0, IntPtr.Zero);//Position = 0
                            var didRead = IntPtr.Zero;
                            pstr.Read(_memoryBuff, streamLen, didRead);
                            Marshal.ReleaseComObject(pstr);
                            pstr = null;
                            WriteInt32(streamLen);//first the length of the blob
                            Str.Write(_memoryBuff, 0, streamLen);//second the blob itself
                            TraceInformation("COM object written bytesize={0}", streamLen);
                        }
#if NET462 || NET463
                        else if (data.GetType().IsSerializable)
                        {
                            
                            var bFormatter = new BinaryFormatter();
                            bFormatter.AssemblyFormat = System.Runtime.Serialization.Formatters.FormatterAssemblyStyle.Simple;
                            bFormatter.TypeFormat = System.Runtime.Serialization.Formatters.FormatterTypeStyle.TypesWhenNeeded;
                            using (var serializedObject = new MemoryStream())
                            {
                                bFormatter.Serialize(serializedObject, data);
                                var streamLen = (int)serializedObject.Position;
                                WriteInt32(streamLen);
                                serializedObject.Position = 0;
                                //indicate a .NET object
                                Str.WriteByte(99);
                                Str.WriteByte(55);
                                serializedObject.WriteTo(Str);
                                TraceInformation(".NET object written bytesize={0}, name={1}", streamLen, TraceInfo.TraceInfo ? data.GetType().Name : null);
                            }
#else
                        else
                        {
                            throw new NotSupportedException("Binary Serialisation (e.g. BinaryFormatter) is only supported when targeting NET461-NET463. Try alternate serialisation, e.g. as JSON string");
#endif
                        }
                        break;
                    default:
                        throw new NotSupportedException(string.Format("this type VarEnum = {0} is not (yet?) supported", vT));
                }
            }
        }
        /// <summary>
        /// Delivers memory storage size in bytes 
        /// Only useful for simple value types, not for strucs neither for 'Variant/object'
        /// </summary>
        internal static int GetElementSize(VarEnum vT)
        {
            switch (vT)
            {
                case VarEnum.VT_UI1:
                    return sizeof(byte);
                case VarEnum.VT_NULL: //TODO
                case VarEnum.VT_EMPTY:
                    return 0;
                case VarEnum.VT_BOOL:
                case VarEnum.VT_UI2:
                case VarEnum.VT_I2:
                    return sizeof(short);
                case VarEnum.VT_R4:
                case VarEnum.VT_I4:
                case VarEnum.VT_UI4:
                    return sizeof(int);
                case VarEnum.VT_CY:
                case VarEnum.VT_UI8:
                case VarEnum.VT_I8:
                    return sizeof(long);
                case VarEnum.VT_R8:
                case VarEnum.VT_DATE:
                    return sizeof(double);
                // on 64 bit windows, it is 24, on 32 bit Windows it is 16
                // however, we don't copy contiguous memory for Variants 
                // but we loop through each separate element
                case VarEnum.VT_VARIANT:
                case VarEnum.VT_DECIMAL:
                    return sizeof(decimal);
                //either 4 or 8 bytes on resp 64 / 32 bit windows
                // not used, we process bstr/string arrays by element, not by block
                case VarEnum.VT_BSTR:
                    return IntPtr.Size;
            }
            return 0;
        }
        internal object ReadValue(VarEnum vT)
        {
            /* section ReadValue */
            if ((vT & VarEnum.VT_ARRAY) == VarEnum.VT_ARRAY)
            {
                var descriptor = GetStruct<ARRAY_DESCRIPTOR>();

                vT ^= VarEnum.VT_ARRAY; //mask out

                var elSize = descriptor.ElemSize;
                var cDims = descriptor.Dims;
                var lMemSize = 1;
                bool isJagged = false;

                // we must break compatibility with ISP SEssion 6.0
                // get the type (which has been written in WriteValue(...)!
                var doGetType = (vT == VarEnum.VT_UNKNOWN || vT == VarEnum.VT_ERROR || vT == VarEnum.VT_VARIANT); //VT_VARIANT because we have object arrays with a specific type?
                var typeStr = default(string);

                if (doGetType)
                {
                    typeStr = ReadString();
                }
                else
                {
                    isJagged = ReadString() == "__jagged";
                    if (isJagged)
                    {
                        typeStr = ReadString(); //e.g. System.Int32[][] 
                    }
                }

                Type requestedType;
                try
                {
                    requestedType = doGetType || isJagged ? Type.GetType(typeStr) : ConvertEnumToType(vT);
                }
                catch (Exception ex)
                {
                    throw new TypeLoadException(string.Format("this Type cannot not be instantiated {0}", typeStr), ex);
                }

                TraceInformation("reading array type={0}, cDims {1}, ElSize {2}", requestedType, cDims, elSize);
                if (cDims == 0)
                {
                    
                    return Array.CreateInstance(requestedType, 0);
                }
                //calculate the size of the blob
                var lengths = new int[cDims];
                var lowerbounds = new int[cDims];
                for (var cx = 0; cx < cDims; cx++)
                {
                    var safebound = GetStruct<SAFEARRAYBOUND>();
                    var els = (int)safebound.cElements;
                    lMemSize *= els;
                    lengths[cx] = els;
                    lowerbounds[cx] = safebound.lLbound;
                }
                var psa = Array.CreateInstance(requestedType, lengths, lowerbounds);
                var lElements = lMemSize;
                lMemSize *= elSize;
                if (lElements == 0)
                {
                    return psa;
                }
                if (!isJagged && ((vT == VarEnum.VT_UI1) || (vT == VarEnum.VT_I2) || (vT == VarEnum.VT_I4) || (vT == VarEnum.VT_UI4) || (vT == VarEnum.VT_R4) || (vT == VarEnum.VT_R8)
                        || (vT == VarEnum.VT_CY) || (vT == VarEnum.VT_DATE) || (vT == VarEnum.VT_UI2)
                        || (vT == VarEnum.VT_BOOL) || (vT == VarEnum.VT_I8) || (vT == VarEnum.VT_UI8) || (vT == VarEnum.VT_I1))
                                )
                {
                    if (vT == VarEnum.VT_UI1 && cDims == 1)
                    {
                        //since byte arrays are quite common, this is an optimization
                        Str.Read((byte[])psa, 0, lMemSize);
                    }
                    //argh, not really efficient!
                    else
                    {
                        EnsureMemory(lMemSize);
                        Str.Read(_memoryBuff, 0, lMemSize);
                        Buffer.BlockCopy(_memoryBuff, 0, psa, 0, lMemSize);//blockcopy, it's a piece of cloth against the bleeding
                    }
                    return psa;
                }
                if (!isJagged && ((vT == VarEnum.VT_DECIMAL || vT == VarEnum.VT_BSTR) && lElements > 0))
                {
                    var psaBound = new SAFEARRAYBOUND[cDims];
                    var rgIndices = new int[cDims];
                    int dimPointer = 0; // next dimension will first be incremented
                    int findEl = 0;
                    for (int x = 0; x < cDims; x++)
                    {
                        psaBound[x].lLbound = psa.GetLowerBound(x);
                        psaBound[x].cElements = (uint)psa.GetUpperBound(x);
                        psaBound[x].cElements = psaBound[x].cElements - (uint)psaBound[x].lLbound + 1;
                        rgIndices[x] = psaBound[x].lLbound;
                    }
                    for (;;)//eternal loop, nice
                    {

                        if (rgIndices[dimPointer] < lengths[dimPointer] + lowerbounds[dimPointer])
                        {
                            switch(vT)
                            {
                                case VarEnum.VT_BSTR:
                                    psa.SetValue(ReadString(), rgIndices);
                                    break;
                                case VarEnum.VT_DECIMAL:
                                    psa.SetValue(ReadDecimal(), rgIndices);
                                    break;
                            }
                            
                            rgIndices[dimPointer]++;
                            if (++findEl == lElements)
                            {
                                return psa;  //eternal is not eternal                                
                            }
                        }
                        //carry stuff
                        else
                        {
                            //magic
                            while (++dimPointer <= cDims)
                            {
                                if (rgIndices[dimPointer] < (lengths[dimPointer] + lowerbounds[dimPointer] - 1))
                                {
                                    rgIndices[dimPointer]++;
                                    break;
                                }
                            }
                            //reset previous cols to initial lowerbound from left to 
                            // most right carry position
                            for (int z = 0; z < dimPointer; z++)
                                rgIndices[z] = lowerbounds[z];
                            dimPointer = 0;
                        }
                    }
                }
                if (isJagged || (vT == VarEnum.VT_VARIANT || vT == VarEnum.VT_UNKNOWN) && lElements > 0)
                {
                    var psaBound = new SAFEARRAYBOUND[cDims];
                    var rgIndices = new int[cDims];
                    int dimPointer = 0; // next dimension will first be incremented
                    int findEl = 0;
                    for (int x = 0; x < cDims; x++)
                    {
                        psaBound[x].lLbound = psa.GetLowerBound(x); // safearrays are 1 based, in .net 0 based
                        psaBound[x].cElements = (uint)psa.GetUpperBound(x);
                        psaBound[x].cElements = (uint)(psaBound[x].cElements - psaBound[x].lLbound + 1);
                        rgIndices[x] = psaBound[x].lLbound;
                    }
                    for (;;)//eternal loop, nice
                    {

                        if (rgIndices[dimPointer] < lengths[dimPointer] + lowerbounds[dimPointer])
                        {
                            vT = (VarEnum)ReadInt16();
                            psa.SetValue(ReadValue(vT), rgIndices);
                            rgIndices[dimPointer]++;
                            if (++findEl == lElements)
                            {
                                return psa;  //eternal is not eternal
                            }
                        }
                        //carry stuff
                        else
                        {
                            //magic
                            while (++dimPointer <= cDims)
                            {
                                if (rgIndices[dimPointer] < (lengths[dimPointer] + lowerbounds[dimPointer] - 1))
                                {
                                    rgIndices[dimPointer]++;
                                    break;
                                }
                            }
                            //reset previous cols to initial lowerbound from left to 
                            // most right carry position
                            for (int z = 0; z < dimPointer; z++)
                                rgIndices[z] = lowerbounds[z];
                            dimPointer = 0;
                        }
                    }
                }
            }
            else
            {
                switch (vT)
                {
                    case VarEnum.VT_UI1:
                        return Str.ReadByte();
                    case VarEnum.VT_I2:
                        return ReadInt16();
                    case VarEnum.VT_UI2:
                        return ReadChar();
                    case VarEnum.VT_NULL:
#if NET462 || NET463
                        return System.DBNull.Value;
#else
                        return DBNull.Value;
#endif
                    case VarEnum.VT_EMPTY:
                        return null;
                    case VarEnum.VT_BOOL:
                        return ReadBoolean();
                    case VarEnum.VT_UI4:
                        return ReadUInt32();
                    case VarEnum.VT_I4:
                        return ReadInt32();
                    case VarEnum.VT_UI8:
                        return ReadUInt64();
                    case VarEnum.VT_I8:
                        return ReadInt64();
                    case VarEnum.VT_R4:
                        return ReadFloat();
                    case VarEnum.VT_R8:
                        return ReadDouble();
                    case VarEnum.VT_CY:
                        return new CurrencyWrapper(ReadCurrency());
                    case VarEnum.VT_DECIMAL:
                        return ReadDecimal();
                    case VarEnum.VT_DATE:
                        return ReadDateTime();
                    case VarEnum.VT_BSTR:
                        return ReadString();
                    case VarEnum.VT_ERROR:
                    case VarEnum.VT_UNKNOWN:
                    case VarEnum.VT_VARIANT: //.NET object
                    case VarEnum.VT_DISPATCH:
                        int bytesInStream = ReadInt32();
                        if (bytesInStream == 0)
                        {
                            return null;
                        }
                        var posBackup = Str.Position;
                        bool isNetObject = Str.ReadByte() == 99 && Str.ReadByte() == 55;
                        if (!isNetObject)
                        {
                            Str.Position = posBackup; //back                             
                        }
                        object data;
                        EnsureMemory(bytesInStream);
                        //var bytes = new byte[BytesInStream];
                        Str.Read(_memoryBuff, 0, bytesInStream);
                        if (!isNetObject)
                        {
                            TraceInformation("Deser Com Object");
                            var hglob = Marshal.AllocHGlobal(bytesInStream);
                            Marshal.Copy(_memoryBuff, 0, hglob, bytesInStream);
                            IStream pstr;
                            var hr = NativeMethods.CreateStreamOnHGlobal(hglob, true, out pstr);
                            if (hr != 0) Marshal.ThrowExceptionForHR(hr);
                            var uknown = new Guid("00000000-0000-0000-C000-000000000046");

                            hr = NativeMethods.OleLoadFromStream(pstr, ref uknown, out data);
                            if (hr != 0) Marshal.ThrowExceptionForHR(hr);
                            if (data == null)
                            {
                                pstr.Seek(0, 0, IntPtr.Zero);//Position = 0
                                TraceInformation("IPersistStream failed, trying IPersistStreamInit");
                                data = OleLoadFromStream2(pstr, uknown);
                            }
                        }
                        else // it is a .NET serializable object
                        {
#if NET462 || NET463
                            TraceInformation("deser .NET");
                            var bFormatter = new BinaryFormatter();
                            bFormatter.TypeFormat = System.Runtime.Serialization.Formatters.FormatterTypeStyle.TypesWhenNeeded;
                            bFormatter.AssemblyFormat = System.Runtime.Serialization.Formatters.FormatterAssemblyStyle.Simple;
                            using (var mem = new MemoryStream(_memoryBuff, 0, bytesInStream))
                            {
                                data = bFormatter.Deserialize(mem);
                            }
                            TraceInformation("DeSerializing {0}", TraceInfo.TraceInfo ? data.GetType().Name : null);
#else
                            data = null;
#endif
                        }
                        return data;
                }
            }
            throw new NotImplementedException(string.Format("this type VarEum = {0} cannot (yet?) be deserialized by CSessionManaged", vT));
        }
        //TODO: support jagged arrays.
        internal static VarEnum ConvertTypeToVtEnum(object typObj)
        {
            if (typObj == null)
            {
                return VarEnum.VT_EMPTY;
            }
            var typ = typObj is Type ? (Type)typObj : typObj.GetType();
            if (typ.IsArray && typ.HasElementType)
            {
                return VarEnum.VT_ARRAY | ConvertTypeToVtEnum(typ.GetElementType());
            }
            switch (Type.GetTypeCode(typ))
            {
                case TypeCode.String:
                    return VarEnum.VT_BSTR;
                case TypeCode.Object:
                    return VarEnum.VT_VARIANT;
                case TypeCode.Byte:
                    return VarEnum.VT_UI1;
                case TypeCode.Boolean:
                    return VarEnum.VT_BOOL;
                case (TypeCode)2:
                    return VarEnum.VT_NULL;
                case TypeCode.Int16:
                    return VarEnum.VT_I2;
                case TypeCode.Char:
                    return VarEnum.VT_UI2;
                case TypeCode.Int32:
                    return VarEnum.VT_I4;
                case TypeCode.UInt32:
                    return VarEnum.VT_UI4;
                case TypeCode.Int64:
                    return VarEnum.VT_I8;
                case TypeCode.UInt64:
                    return VarEnum.VT_UI8;
                case TypeCode.Single:
                    return VarEnum.VT_R4;
                case TypeCode.Double:
                    return VarEnum.VT_R8;
                case TypeCode.Decimal:
                    return VarEnum.VT_DECIMAL;
                case TypeCode.DateTime:
                    return VarEnum.VT_DATE;
            }


            //if (typ == typeof(object))
            //    return VarEnum.VT_VARIANT;


            if (typ == typeof(CurrencyWrapper))
                return VarEnum.VT_CY;
            if (typ == typeof(Exception))
                return VarEnum.VT_ERROR;
            //            if (typ.IsValueType)
            //              return VarEnum.VT_RECORD; // TimeSpan and such
#if NET462 || NET463
            if (typ.IsImport || typ.IsSerializable) //
                return VarEnum.VT_UNKNOWN;
#endif
            //not supported VT_ERROR, Missing etc.
            throw new NotSupportedException(string.Format("this type {0} cannot be serialized. Make sure it is COM serializable or that it is decorated with SerializableAttribute", typ.FullName));
        }

        internal static Type ConvertEnumToType(VarEnum vT)
        {
            if ((vT & VarEnum.VT_ARRAY) == VarEnum.VT_ARRAY)
                throw new Exception("Cannot convert an array of type array.");
            switch (vT)
            {
                case VarEnum.VT_UI1:
                    return typeof(byte);
                case VarEnum.VT_EMPTY:
                    return null;
                case VarEnum.VT_UNKNOWN:
                case VarEnum.VT_VARIANT:
                    return typeof(object);
                case VarEnum.VT_BOOL:
                    return typeof(bool);
                case VarEnum.VT_NULL:
#if NET462 || NET463
                    return typeof(System.DBNull);
#else
                        return typeof(DBNull);
#endif
                case VarEnum.VT_I2:
                    return typeof(short);
                case VarEnum.VT_UI2:
                    return typeof(char);
                case VarEnum.VT_UI4:
                    return typeof(uint);
                case VarEnum.VT_UI8:
                    return typeof(ulong);
                case VarEnum.VT_I4:
                    return typeof(int);
                case VarEnum.VT_I8:
                    return typeof(long);
                case VarEnum.VT_R4:
                    return typeof(float);
                case VarEnum.VT_R8:
                    return typeof(double);
                case VarEnum.VT_ERROR:
                    return typeof(Exception);
                case VarEnum.VT_DECIMAL:
                    return typeof(decimal);
                case VarEnum.VT_CY:
                    return typeof(CurrencyWrapper);
                case VarEnum.VT_DATE:
                    return typeof(DateTime);
                case VarEnum.VT_BSTR:
                    return typeof(string);
                default:
                    return null;
            }
        }
     

        /// <summary>
        /// saves a COM object using IPersistStreamInit
        /// </summary>
        /// <param name="pPersistStmInit"></param>
        /// <param name="pStm"></param>
        /// <returns>true if supported and success</returns>
        public static bool OleSaveToStream2(IPersistStreamInit pPersistStmInit, IStream pStm)
        {
            if (pPersistStmInit == null || pStm == null)
                throw new ArgumentNullException();

            Guid clsd;
            var persist = (IPersist)pPersistStmInit;
            persist.GetClassID(out clsd);
            NativeMethods.WriteClassStm(pStm, ref clsd);
            pPersistStmInit.Save(pStm, true);

            return true;

        }

        public static object OleLoadFromStream2(IStream pStm, Guid iidInterface)
        {
            if (pStm == null || iidInterface == null) throw new ArgumentNullException("pStm");

            NativeMethods.ReadClassStm(pStm, out Guid clsd);
            var typ = Marshal.GetTypeFromCLSID(clsd);
            var obj = Activator.CreateInstance(typ);
            var pPersist = obj as IPersistStreamInit;
            if (pPersist == null)
            {
                pStm.Seek(0, 0, IntPtr.Zero); //STREAM_SEEK_CUR
                return null;
            }
            pPersist.Load(pStm);
            var face = IntPtr.Zero;
            var unk = IntPtr.Zero;
            try
            {
                unk = Marshal.GetIUnknownForObject(obj);
                Marshal.QueryInterface(unk, ref iidInterface, out face);
                return Marshal.GetObjectForIUnknown(face);
            }
            finally
            {
                if (face != IntPtr.Zero)
                {
                    Marshal.Release(face);
                }
                if (unk != IntPtr.Zero)
                {
                    Marshal.Release(unk);
                }
            }
        }
      
        private static readonly object l = new object();
        private static Dictionary<string, string> _cache;
        //works only for .NET 45
        public static string GetMetaData(string key)
        {
            if (_cache == null || !_cache.ContainsKey(key))
            {
                lock (l)
                {
                    if (_cache == null)
                    {
                        _cache = new Dictionary<string, string>(4);
                    }
                    foreach (var meta in typeof(NativeMethods).GetTypeInfo().Assembly.GetCustomAttributes<AssemblyMetadataAttribute>())
                    {
                        if (meta.Key == key)
                        {
                            _cache[key] = meta.Value;
                            break;
                        }
                    }
                    if (!_cache.ContainsKey(key))
                    {
                        throw new Exception(string.Format("meta key {0} not found", key));
                    }
                }
            }
            return _cache[key];
        }
        [DebuggerStepThrough]
        internal static byte[] HexToBytes(string h)
        {
            if (string.IsNullOrEmpty(h))
            {
                throw new ArgumentNullException(nameof(h), "Id cannot be empty");
            }
            var number_of_characters = h.Length;
            if (number_of_characters != 32)
            {
                throw new ArgumentException("Id length must be 32", nameof(h));
            }

            var bytes = new byte[number_of_characters / 2]; // Initialize our byte array to hold the converted string.

            int write_index = 0;

            for (int read_index = 0; read_index < number_of_characters; read_index++)
            {
                byte upper = FromCharacterToByte(h[read_index], read_index, 4);
                read_index++;
                byte lower = FromCharacterToByte(h[read_index], read_index);

                bytes[write_index++] = (byte)(upper | lower);
            }
            return bytes;

        }
        //do NOT REMOVE
        private static byte FromCharacterToByte(char character, int index, int shift = 0)
        {
            byte value = (byte)character;
            if (((0x40 < value) && (0x47 > value)) || ((0x60 < value) && (0x67 > value)))
            {
                if (0x40 == (0x40 & value))
                {
                    if (0x20 == (0x20 & value))
                        value = (byte)(((value + 0xA) - 0x61) << shift);
                    else
                        value = (byte)(((value + 0xA) - 0x41) << shift);
                }
            }
            else if ((0x29 < value) && (0x40 > value))
                value = (byte)((value - 0x30) << shift);
            else
                throw new InvalidOperationException(string.Format("Character '{0}' at index '{1}' is not valid alphanumeric character.", character, index));

            return value;
        }
#if !Demo
        public unsafe static uint GetHashCode2(string value)
        {
            if (string.IsNullOrEmpty(value)) return 0;
            var bytes = _encoding.GetBytes(value);
            uint outp;
            fixed (byte* ptr = bytes)
            {
                var result = NativeMethods.HashData(ptr, bytes.Length, (void*)&outp, 4);
                if (result != 0)
                {
                    return 0;
                }
            }

            return outp;
        }

        /// <summary>LicentieCheck
        /// do not modify these lines. It is illegal.
        /// position - 
        /// meaning
        /// position 1: License
        /// position 2-5 calculated license
        /// position 6-9 calculated hash on license (if license type = 4)        
        /// </summary>                
        public static bool LicentieCheck(byte[] license, string licensedfor)
        {
            if (license == null)
            {
                throw new ArgumentNullException("license", "License cannot be null");
            }
            if (string.IsNullOrEmpty(licensedfor))
            {
                throw new ArgumentNullException("licensedfor", "licensedfor cannot be null or empty");
            }
            var retVal = false;
            var licenseType = license[0];
            if (!Array.Exists( Productid,x => x ==licenseType)) //ISP Session 6.0 and others are excluded
            {
                return false;
            }

            var nt4Netbiosname = NativeMethods.GetNetBiosName(); //uppercase
            if (string.IsNullOrEmpty(nt4Netbiosname))
            {
                nt4Netbiosname = NativeMethods.GetNetBiosName(true); //lowercase often
            }
            var pdomInfo = NativeMethods.GetDomainInfo();
            string cwName = null;
            string workgroupname = null;
            if (!string.IsNullOrEmpty(pdomInfo.DomainName))
            {
                cwName = pdomInfo.DomainName;
            }
            else
            {
                var joinInfo = NativeMethods.GetJoinedDomain();
                workgroupname = string.IsNullOrEmpty(joinInfo.WorkGroup) ? joinInfo.Domain : joinInfo.WorkGroup;
            }
            TraceInformation("Names {0} {1}", nt4Netbiosname, cwName);
            var lines = new List<string>(licensedfor.Split(new[] { licensedfor.IndexOf("\r\n", StringComparison.Ordinal) > 0 ? "\r\n" : " " }, StringSplitOptions.None));

            lines.Insert(0, ProductString);
            var foundLicensedItem = lines.Exists(x => x == nt4Netbiosname || x == workgroupname || x == cwName);
            if (!foundLicensedItem && (licenseType != 4 && licenseType != 29))
            {
                TraceInformation("Could not find licensedItem {0} in allowed licensee {1}", cwName, licensedfor);
                return false;

            }
            licensedfor = string.Join(" ", lines);
            var hashcode = GetHashCode2(licensedfor);
            var checkCode = BitConverter.ToUInt32(license, 1);
            var checkCode2 = BitConverter.ToUInt32(license, 5);
            TraceInformation("given license {0} calculated license {1} license hash {2}", checkCode, hashcode, checkCode2);
            switch (licenseType)
            {
                case 27:
                case 28:
                case 26://isp session simple / advanced & ent
                    if (hashcode == checkCode)
                        retVal = true;
                    break;
                case 29://isp session blk this just checks the GUID, not the given domain name
                    var buf = string.Concat(licenseType.ToString(), checkCode.ToString());
                    if (GetHashCode2(buf) == checkCode2)
                        retVal = true;
                    break;
            }
            return retVal;
        }
#endif
    }
    public sealed class TraceSwitch
    {
        private readonly string name;
        private readonly string description;
        public TraceSwitch(string name, string description)
        {

            this.name = name;
            this.description = description;
        }
        public bool TraceInfo { get; set; }
        
        public bool TraceError { get; set; }
        
    }
    public sealed class DBNull
    {
        public static DBNull Value
        {
            get
            {
                return new DBNull();

            }
        }
    }
}