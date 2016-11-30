using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Reflection;
using System.IO;
using System.Diagnostics;


namespace ADCCure.Configurator.DAL
{

    public static class util
    {
       // public static ILog logger = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType.Name);
        public enum MessageType
        {
            Info,
            Error,
            Warning
        }
        public static void WriteToLog(ref string Text)
        {
            WriteToLog(ref Text, MessageType.Info);
        }
        public static void WriteToLog(string Text)
        {
            WriteToLog(ref Text, MessageType.Info);
        }
        public static void WriteToLog(ref string Text, MessageType type)
        {
            if (type == MessageType.Info)
            {
                //logger.Info(Text);
                Trace.TraceInformation(Text);
            }
            else if (type == MessageType.Error)
            {
                //logger.Error(Text);
                Trace.TraceError(Text);
            }
            else if (type == MessageType.Warning)
            {
                //logger.Warn(Text);
                Trace.TraceWarning(Text);
            }
            
        }
        public static void WriteToLog(string Text, MessageType type)
        {
            if (type == MessageType.Info)
            {
                //logger.Info(Text);
                Trace.TraceInformation(Text);
            }
            else if (type == MessageType.Error)
            {
                //logger.Error(Text);
                Trace.TraceError(Text);
            }
            else if (type == MessageType.Warning)
            {
                //logger.Warn(Text);
                Trace.TraceWarning(Text);
            }
        }

        public const string KERNEL32 = "kernel32.dll";
        private const int MAXPATH = 260;
        [DllImport(KERNEL32, CharSet=CharSet.Unicode, SetLastError = true)]
        private static extern int GetSystemWindowsDirectory([MarshalAs(UnmanagedType.LPWStr)] string lpBuffer, int uSize);
        

        /// <summary>
        /// File.CopyFile has the drawback it cannot copy over paths longer than 260 (MAXPATH) 
        /// </summary>
        /// <param name="lpExistingFileName"></param>
        /// <param name="lpNewFileName"></param>
        /// <param name="bFailIfExists"></param>
        /// <returns></returns>
        [DllImport(KERNEL32, CharSet = CharSet.Unicode, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool CopyFile(
            [MarshalAs(UnmanagedType.LPWStr)] string lpExistingFileName,
            [MarshalAs(UnmanagedType.LPWStr)] string lpNewFileName,
            [MarshalAs(UnmanagedType.Bool)] bool bFailIfExists);
        
        [DllImport(KERNEL32, SetLastError = true, CallingConvention = CallingConvention.Winapi)]  
            [return: MarshalAs(UnmanagedType.Bool)]  
            internal static extern bool IsWow64Process([In] IntPtr hProcess, out bool lpSystemInfo);  

  
        /// <summary>
        /// returns the physical application path
        /// </summary>
        /// <returns></returns>
        public static string AppPath
        {
            get
            {
                return Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
            }
        }
        public static void EnsureProcessKilled(string executable)
        {
            executable = Path.GetFileNameWithoutExtension(executable);
            foreach (Process p in Process.GetProcesses())
            {
                if (string.Compare(p.ProcessName, executable, true) == 0)
                {
                    p.Kill();
                    break;
                }
            }
        }
        /// <summary>
        /// avoid starting this program twice
        /// returns true if the app already is in memory
        /// </summary>
        /// <returns></returns>
        public static bool PrevInstance()
        {
            try
            {
                Process cp = Process.GetCurrentProcess();
                Trace.TraceInformation("current process {0}", cp.ProcessName);
                foreach (Process p in Process.GetProcesses())
                {
                    if (cp.Id != p.Id && cp.ProcessName == p.ProcessName)
                    {
                        Trace.TraceInformation("current process {0} {1} matched with {2},{3}", cp.ProcessName, cp.Id, p.ProcessName, p.Id);
                        return true;
                    }
                }
            }
            catch (Exception ex)
            {
                Trace.TraceError("Previous Instance Check went wrong with {0}", ex.Message);
            }
            return false;
        }
        public static string WinDir()
        {
            string s = new string('\0', MAXPATH);
            int chars = GetSystemWindowsDirectory(s, MAXPATH);
            return s.Substring(0, chars);
        }
        /// <summary>
        /// returns full path to InstallUtil.exe
        /// </summary>
        /// <returns></returns>
        public static string GetInstallUtil()
        {
            object ob = new object();
            return Path.Combine(Path.GetDirectoryName(ob.GetType().Assembly.Location), "InstallUtil.exe");
        }

        /// <summary>
        /// tests if msmq is locally installed or not
        /// </summary>
        public static bool IsMessageQueueueInstalled
        {
            get
            {//MSMQ.MSMQApplication.1
                var msqeueuGUID = new Guid("{D7D6E086-DCCD-11d0-AA4B-0060970DEBAE}");
                try
                {
                    Type msmqInstance = Type.GetTypeFromCLSID(msqeueuGUID);
                    object testit = Activator.CreateInstance(msmqInstance);
                    System.Runtime.InteropServices.Marshal.ReleaseComObject(testit);
                    testit = null;
                    return true;
                }
                catch
                {
                    return false;
                }
            }
        }

        /// <summary>
        /// As name suggests Replaces Variables InConfigFile
        /// </summary>
        /// <param name="file"></param>
        /// <param name="findReplaces"></param>
        /// <param name="AskedVariables"></param>
        public static void ReplaceVariablesInConfigFile(string file, List<FindReplaceVar> findReplaces)
        {
            string origBuf = File.ReadAllText(file, Encoding.UTF8);
            var buf = new StringBuilder(origBuf);
            if (findReplaces != null)
            {
                foreach (FindReplaceVar var in findReplaces)
                {
                    buf.Replace(var.Find, var.ToString());
                }
            }
            if (origBuf.IndexOf(']') > 0 && origBuf.IndexOf('[') > 0)
            {
                buf.Replace('[', '_');
                buf.Replace(']', '_');
                File.WriteAllText(file, buf.ToString(), Encoding.UTF8);
            }
        }
        /// <summary>
        /// gets the Program Files folder, defaults to the x86 folder if we run on AMD64 CPU
        /// </summary>
        public static string GetProgramFiles
        {
            get
            {
                if (!IsWindowsX64Process || Environment.GetEnvironmentVariable("PROCESSOR_ARCHITECTURE") == "x86")
                {
                    return Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles);
                }
                return Environment.GetEnvironmentVariable("ProgramFiles(x86)");
            }
        }
        /// <summary>
        /// can be use to avoid writing the XML to a e.g. cd 
        /// </summary>
        /// <param name="file"></param>
        /// <returns></returns>
        public static bool FileIsWritable(string file)
        {
            if (!File.Exists(file))
            {
                try
                {
                    using (var fs = File.Create(file))
                    {
                    }
                }
                catch
                {
                    return false;
                }
            }
            var fi = new FileInfo(file);
            if (fi.IsReadOnly)
            {
                return true;
            }
            try
            {
                using (FileStream fio = fi.Open(FileMode.Open, FileAccess.ReadWrite))
                {
                    if (fi.Length > 0)
                    {
                        byte bt = (byte)fio.ReadByte();
                        fio.Position--;//write back
                        fio.WriteByte(bt);
                    }
                    else
                    {
                        fio.WriteByte(0);
                        fio.SetLength(0);
                    }
                    
                    return true;
                }
            }
            catch
            {
                return false;
            }

        }
        public static bool IsWindowsX64Process
        {
            get
            {
                // a pointer on x64 is 8 bytes length
                return IntPtr.Size == 8;
            }
        }
    }
}