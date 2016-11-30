using ispsession.io.setup.util;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Configuration;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Windows;
using System.Xml;


namespace ispsession.io.setup
{
    /// <summary>
    /// does stuff
    /// </summary>
    public sealed class OtherConfig
    {
        private readonly Configuration _stateConfig;
        private XmlDocument _doc;
        private readonly ConfigurationSection _settings;
        private readonly KeyValueConfigurationCollection _appKeys;
        private readonly SettingElementCollection _settingsCollection;

        public ConnectionStringSettingsCollection ConnectionStrings
        {
            get { return _stateConfig.ConnectionStrings.ConnectionStrings; }
        }

        internal void Set(string settingName, string value)
        {
            if (_settings is ClientSettingsSection)
            {
                var xmlEl = _doc.CreateElement("value");
                xmlEl.InnerText = value;
                //assumes that the setting exists
                var elem = _settingsCollection.Get(settingName);
                elem.Value.ValueXml = xmlEl;
            }
            else
            {
                if (_appKeys[settingName] != null)
                {
                    _appKeys[settingName].Value = value;
                }
                else
                {
                    _appKeys.Add(settingName, value);
                }
            }
        }

        internal T Get<T>(string settingName, T defaultvalue)
        {
            if (_settings is ClientSettingsSection)
            {
                var elem = ((ClientSettingsSection)_settings).Settings.Get(settingName);
                if (_doc == null && elem != null)
                {
                    _doc = elem.Value.ValueXml.OwnerDocument;
                }
                if (elem.SerializeAs == SettingsSerializeAs.String)
                {
                    return !string.IsNullOrEmpty(elem.Value.ValueXml.InnerText) ?  
                        (T)Convert.ChangeType(elem.Value.ValueXml.InnerText, typeof(T))
                        : defaultvalue;
                }

            }
            else
            {
                return _appKeys[settingName] != null ? (T)Convert.ChangeType(_appKeys[settingName].Value, typeof(T)) : defaultvalue;
            }
            return default(T);
        }

        internal T Get<T>(string settingName)
        {
            var section = _settings as ClientSettingsSection;
            if (section != null)
            {
                var elem = section.Settings.Get(settingName);
                if (_doc == null && elem != null)
                {
                    _doc = elem.Value.ValueXml.OwnerDocument;
                }
                if (elem.SerializeAs == SettingsSerializeAs.String)
                {
                    return (T)Convert.ChangeType( elem.Value.ValueXml.InnerText, typeof(T));
                }

            }
            else
            {
                return  _appKeys[settingName] != null ? (T)Convert.ChangeType( _appKeys[settingName].Value, typeof(T)) : default(T);
            }
            return default(T);
        }
        internal void Save()
        {
            _stateConfig.Save();
        }
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="file">mydll.dll or myexe.exe</param>
        /// <param name="pPath">Xpath to section. e.g. 'appSettings'</param>
        internal OtherConfig (string file, string pPath)
        {
            if (!Path.IsPathRooted(file))
            {
                file = Path.Combine(  AppInfo.CurrentPath, file);
            }
            _stateConfig = ConfigurationManager.OpenExeConfiguration(file);
            var secttions = _stateConfig.GetSection(pPath);
            
            _settings = secttions;
            var section = _settings as ClientSettingsSection;
            if (section != null)
            {
                _settingsCollection = section.Settings;
            }
            else
            {
                _appKeys = ((AppSettingsSection)_settings).Settings;
            };
        }
    }

  
    sealed class Installer 
    {
        internal static int lConnPool = 1048576;
        public const string PRODUCT = "ISP Session";
        public const string ISPSESSIONSERVICE = "ASPSession Service";
     

        //internal static ICOMAdminCatalog GetComadminCatalog()
        //{
        //    var comAdmin = Type.GetTypeFromCLSID(new Guid("F618C514-DFB8-11D1-A2CF-00805FC79235"));
        //    var retVal = (ICOMAdminCatalog)Activator.CreateInstance(comAdmin);
        //    return retVal;
        //}
        
      
        sealed class CopyInfo
        {
            internal CopyInfo(string fullName, bool isFolder = false)
            {
                Path = fullName;
                this.IsFolder = isFolder;
            }

            internal readonly bool IsFolder; // if false, it is a file
            internal readonly string Path;
        }
        public static bool CopyAll(string sourcePath, string destPath)
        {
            var all = ReadAllInfo(sourcePath);
            var result = MessageBoxResult.OK;
            var destInfo = new DirectoryInfo(destPath);
            int sourcePathLen = sourcePath.Length;
            foreach (var p in all)
            {
                //string dest = Path.Combine(destPath, 
                //(p.Path.Length > sourcePathLen ? Path.GetDirectoryName(p.Path.Substring(sourcePathLen+1)) : 
                //string.Empty));

                string dest = p.Path.Length > sourcePathLen ? destPath + p.Path.Substring(sourcePathLen)
                        :
                    dest = destPath;

                if (p.IsFolder)
                {
                    string newfold = dest; // Path.Combine(destPath, Path.GetFileName(p.Path));
                    if (!Directory.Exists(newfold))
                    {
                        Directory.CreateDirectory(newfold);
                    }
                }
                else
                {
                    //string newfold = Path.Combine(dest, Path.GetFileName(p.Path)); ;// Path.Combine(Path.Combine(destPath, dest), Path.GetFileName(p.Path));
                    try
                    {
                        ispsession.Configurator.DAL.util.CopyFile(string.Concat(@"\\?\", p.Path), string.Concat(@"\\?\", dest), false);
                        int error = Marshal.GetLastWin32Error();
                        if (error != 0)
                        {
                            throw new Win32Exception(error, p.Path);
                        }
                        //File.Copy(string.Concat(@"\\?\", p.Path), string.Concat(@"\\?\", dest), true);
                    }
                    catch (Exception ex)
                    {
                        result = MessageBox.Show(string.Concat("Cannot copy: ", ex.Message), AppInfo.AssemblyTitle, MessageBoxButton.OKCancel);
                        if (result == MessageBoxResult.Cancel) break;
                    }

                }
            }
            return result == MessageBoxResult.OK;
        }
        private static IEnumerable<CopyInfo> ReadAllInfo(string path)
        {
            var retVal = new List<CopyInfo>();
            //util.WriteToLog("readAllInfo: " + path);
            if (!Directory.Exists(path))
            {
                throw new DirectoryNotFoundException(string.Format("{0} not found", path));
            }
            ReadAllInfo_Recursive(retVal, new DirectoryInfo(path));
            return retVal;
        }
        private static void ReadAllInfo_Recursive(List<CopyInfo> folderInfo, DirectoryInfo nfo)
        {
            folderInfo.Add(new CopyInfo(nfo.FullName, true));
            folderInfo.AddRange(nfo.GetFiles().Select(finf => new CopyInfo(finf.FullName)));
            foreach (var dirinfo in nfo.GetDirectories())
            {
                ReadAllInfo_Recursive(folderInfo, dirinfo);
            }
        }

        
        internal static void Install(string action, int pool)
        {

            string installoruninstall = action;
            Trace.TraceInformation("action {0} pool {1}", action, pool);
            //MessageBox.Show(installoruninstall);
            try
            {
                if (!string.IsNullOrEmpty(installoruninstall))
                {
                    if (installoruninstall == "install")
                    {
                      //  Util.ASPService(true);                        
                        var path = Path.Combine(AppInfo.CurrentPath, "install ISP Session.cmd");
                        Trace.TraceInformation("Path {0}", path);
                        var procStart = new ProcessStartInfo(Environment.GetEnvironmentVariable("ComSpec"), "/C " + "\"" + path + "\"")
                        {
                            CreateNoWindow = true,
                            UseShellExecute = false
                        };
                        var proc = Process.Start(procStart);
                        proc.WaitForExit();
                        var exitCode = proc.ExitCode;
                        if (exitCode != 0)
                        {
                            //TODO: supply a WFC window!
                            MessageBox.Show(string.Format("Please run {0} manually, something went wrong during registration of the COM dlls. Exitcode {1}", path, exitCode), AppInfo.AssemblyTitle, MessageBoxButton.OK, MessageBoxImage.Error);
                        }


                        Util.FixSecurity(Path.Combine(AppInfo.CurrentPath, "CSession.dll"), false);
                        Util.FixSecurity(Path.Combine(AppInfo.CurrentPath, "CSession64.dll"), false);
                        Util.FixSecurity(Path.Combine(AppInfo.CurrentPath, "CSession.dll.config"), false);
                     //   Util.FixSecurity(Path.Combine(AppInfo.CurrentPath, "Service\\ASPSession.exe.config"), false);
                        
                    }
                    else if (installoruninstall.StartsWith("u"))
                    {
                     //   Util.ASPService(false);
                        var path = Path.Combine(AppInfo.CurrentPath, "uninstall ISP Session.cmd");

                        var procStart = new ProcessStartInfo(Environment.GetEnvironmentVariable("ComSpec"), "/C " + "\"" + path + "\"")
                        {
                            CreateNoWindow = true,
                            UseShellExecute = false
                        };
                        var proc = Process.Start(procStart);
                        proc.WaitForExit();
                        var exitCode = proc.ExitCode;
                        if (exitCode != 0)
                        {
                            //TODO: supply a WFC window!
                            MessageBox.Show(string.Format("Please run {0} manually, something went wrong during registration of the COM dlls", path), AppInfo.AssemblyTitle, MessageBoxButton.OK, MessageBoxImage.Error);
                        }
                        //TODO remove database                        

                    }
                }
            }
            catch (Exception ex)
            {
                Trace.TraceError("Install failed with {0}", ex);
            }

        }
    }
}