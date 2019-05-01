using System;
using System.Diagnostics;
using System.IO;
using System.Text;

namespace ispsession.io.core
{
    internal sealed class TraceSwitch
    {
        internal string Name { get; }
        internal string Description { get; }
        public TraceSwitch(string name, string description)
        {
            this.Name = name;
            this.Description = description;
        }
        public bool TraceInfo { get; set; }

        public bool TraceError { get; set; }
    }
    internal static class Diagnostics
    {
        internal static readonly TraceSwitch TraceInfo = new TraceSwitch("ISPSession", "ISPsession Trace Switch");

        internal static void TraceError(string message, params object[] args)
        {
            if (TraceInfo.TraceError)
            {
                //TraceLevel.Info=3
                //var fmt = string.Format(message, args);
                //NativeMethods.OutputDebugStringW(fmt);                
                var fmt = $"Error {DateTimeOffset.Now} {System.Threading.Thread.CurrentThread.ManagedThreadId} {string.Format(message, args)}";
                Debug.WriteLine(fmt);
            }
        }
        internal static void TraceInformation(string message, params object[] args)
        {
            if (TraceInfo.TraceInfo)
            {
                //var fmt = string.Format(message, args);
                //NativeMethods.OutputDebugStringW(fmt);
                var fmt = $"Information {DateTimeOffset.Now} {System.Threading.Thread.CurrentThread.ManagedThreadId} {string.Format(message, args)}";
                Debug.WriteLine(fmt);
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
