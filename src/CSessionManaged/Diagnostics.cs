using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ispsession.io
{
    internal static class Diagnostics
    {
        internal static readonly TraceSwitch TraceInfo = new TraceSwitch("ISPSession", "ISPsession Trace Switch");

        internal static void TraceError(string message, params object[] args)
        {
            if (TraceInfo.TraceError)
            {
                //TraceLevel.Info=3
                Trace.TraceError(message, args);
            }
        }
        internal static void TraceInformation(string message, params object[] args)
        {
            if (TraceInfo.TraceInfo)
            {
                Trace.TraceInformation(message, args);
            }
        }
        internal static void TraceInformation(StreamWriter log, string message)
        {
            if (TraceInfo.TraceInfo)
            {
                log.Flush();
                log.BaseStream.Position = 0;
                var reader = new StreamReader(log.BaseStream, Encoding.UTF8);
                while (!reader.EndOfStream)
                {
                    Trace.TraceInformation("Loginfo {0}", reader.ReadLine());
                }
            }
        }

    }
}
