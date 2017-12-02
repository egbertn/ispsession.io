using System.IO;

namespace ispsession.io
{
    internal class AppPersistUtil: StreamManager
    {
        internal AppPersistUtil()
        {

        }
        internal AppPersistUtil(Stream str) : base(str)
        {

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
    }
}
