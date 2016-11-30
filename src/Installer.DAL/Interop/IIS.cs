using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;
using System.IO;
using System.Windows.Forms;
using Microsoft.Win32;

namespace ADCCure.Configurator.DAL.Interop
{
    internal enum EnumAppPool:int
    {
        MD_APPPOOL_IDENTITY_TYPE_LOCALSYSTEM = 0,
        MD_APPPOOL_IDENTITY_TYPE_LOCALSERVICE = 1,
        MD_APPPOOL_IDENTITY_TYPE_NETWORKSERVICE = 2,
        MD_APPPOOL_IDENTITY_TYPE_SPECIFICUSER = 3
    }

    public sealed class Site
    {
        /// <summary>
        /// set description as in 'Default Web Site'
        /// </summary>
        public string SiteName { get; set; }
        /// <summary>
        /// complete path as in /LM/W3SVC/1/
        /// </summary>
        public string AdsPath { get; set; }
        /// <summary>
        /// Site instance ID as the 1 in IIS://LOCALHOST/W3SVC/1/ROOT
        /// </summary>
        public int Instance { get; set; }
        public override string ToString()
        {
            return SiteName;
        }
    }
    //[Guid("4B42E390-0E96-11d1-9C3F-00A0C922E703"), ComImport(), InterfaceType(ComInterfaceType.InterfaceIsDual)]
    //interface IISBaseObject 
    //{
    //    [return: MarshalAs(UnmanagedType.Struct)]
    //    object GetDataPaths( string bstrName, int lnAttribute) ;
        
    //    [return: MarshalAs(UnmanagedType.IDispatch)]
    //    object GetPropertyAttribObj(string bstrName);
        
    //};
    /// <summary>
    /// generic IIS interop utils
    /// We also could use DirectoryEntry which is a wrapper around IADs, however
    /// that is a managed class with lots of 'little known facts about' which make
    /// in my opinion, the logic more complex than straight using IADs
    /// </summary>
    public static class IIS
    {  /// <summary>
        /// IIS://LOCALHOST/W3SVC
        /// </summary>
        public const string WEBSERVER = "IIS://LOCALHOST/W3SVC";
        public const string IIS_APP_POOL_CLASS = "IIsApplicationPool";
        private static bool? _IIS6MetaBase;
        private static bool? _IISIsInstalled;
        private static bool GoOn()
        {
            if (_IIS6MetaBase == null)
            {
                int i;
                bool b; //bogus
                bool IIS6MetaBase, IISIsInstalled;
                IISOptionsEnabled(null, out IIS6MetaBase, out b, out IISIsInstalled, out i);
                _IIS6MetaBase = IIS6MetaBase;
                _IISIsInstalled = IISIsInstalled;
            }
            return _IIS6MetaBase.Value;
        }
        public static bool VDirExists(IList<Site> sites, string virtualDir, out int foundOnInstance)
        {
          
            foundOnInstance = 0;
            if (!GoOn()) return false;
            foreach (var site in sites)
            {
                try
                {
                    var baseObj = (IADsContainer)Marshal.BindToMoniker(String.Format("{0}/{1}/ROOT/{2}", WEBSERVER, site.Instance, virtualDir));
                    Marshal.ReleaseComObject(baseObj);
                    foundOnInstance = site.Instance;
                    return true;
                }
                catch
                {
                }
            }
            return false;
        }

        /// <summary>
        /// does a flat (not recursive) enum from a specific root site
        /// </summary>
        /// <param name="site"></param>
        /// <param name="subPath"></param>
        /// <param name="instanceIde"></param>
        /// <returns></returns>
        internal static IList<string> getVdirs(int instanceIde)
        {
            var retVal = new List<string>();
            var baseObj = (IADsContainer)Marshal.BindToMoniker(String.Format("{1}/{0}/ROOT", instanceIde, WEBSERVER));
            var venum = (IEnumVARIANT)baseObj._newEnum;
            var regvar = new object[1];
            var fetched = IntPtr.Zero;
            while (venum.Next(1, regvar, fetched) == 0)
            {
                var aap = (IADs)regvar[0];
                if (aap.Class == "IIsWebVirtualDir")
                {
                    string path = aap.ADsPath;
                    path = String.Concat("/LM", path.Remove(0, 15).Replace('\\', '/'));
                    string cn = Path.GetFileName((string)aap.Get("AppRoot"));
                    retVal.Add(cn);
                }
            }
            return retVal;
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="appPool"></param>
        /// <param name="userAccount"></param>
        /// <param name="password"></param>
        /// <param name="classicMode"></param>
        /// <param name="NetVersion"></param>
        /// <returns></returns>
        public static IADs CreateAppPool(string appPool, string userAccount, string password, bool classicMode, float NetVersion)
        {
            if (!GoOn()) return null;
            var baseObj = (IADsContainer)Marshal.BindToMoniker(WEBSERVER + "/APPPOOLS");

            IADs pAppPool;

            try
            {
                pAppPool = (IADs)baseObj.GetObject("IIsApplicationPool", appPool);
            }
            catch
            {
                pAppPool = (IADs)baseObj.Create("IIsApplicationPool", appPool);
            }

            var identType = EnumAppPool.MD_APPPOOL_IDENTITY_TYPE_NETWORKSERVICE;
            if (!string.IsNullOrEmpty(userAccount))
            {
                pAppPool.Set("WAMUserName", userAccount);
                pAppPool.Set("WAMUserPass", password);
                identType = EnumAppPool.MD_APPPOOL_IDENTITY_TYPE_SPECIFICUSER;
            }
            pAppPool.Set("AppPoolIdentityType", identType);
            // format as 2.0/4.0/4.5
            pAppPool.Set("ManagedRuntimeVersion", string.Format("v{0:0.0}", NetVersion));
            pAppPool.Set("ManagedPipelineMode", classicMode ? 1 : 0);
            pAppPool.SetInfo();
            return pAppPool;
        }
        public static void IISOptionsEnabled(string machine, out bool IIs6Metabase, out bool WcfHttp, out bool IIsIsInstalled, out int IISVersion)
        {
             
            IISVersion = 0;
            IIs6Metabase =
            WcfHttp =
            IIsIsInstalled = false;
            try
            {//expanding is done automatically
                RegistryKey hive =
                    String.IsNullOrEmpty(machine) ? Registry.LocalMachine : RegistryKey.OpenRemoteBaseKey(RegistryHive.LocalMachine, machine);
                RegistryKey inetSetup = hive.OpenSubKey(@"SOFTWARE\Microsoft\InetStp", false);
                string temp = (string)inetSetup.GetValue("PathWWWRoot", null);
                if (!String.IsNullOrEmpty(temp))
                {
                    IIsIsInstalled = true;
                }
                int value = (int)inetSetup.GetValue("MajorVersion", 6, RegistryValueOptions.DoNotExpandEnvironmentNames);
                if (value > 0)
                {
                    IISVersion = value;
                }
            }
            catch
            {
                //iis seems not installed here! ANyway, give a second chance
                string temp = Environment.ExpandEnvironmentVariables(@"%SystemDrive%\inetpub\wwwroot");
                IIsIsInstalled = Directory.Exists(temp);
            }


            IADsContainer objMachine;
            try
            {
                objMachine = (IADsContainer)Marshal.BindToMoniker(String.Format("IIS://{0}", String.IsNullOrEmpty(machine) ? "LOCALHOST" : machine));
                IIs6Metabase = true;
              
            }
            catch
            {
                return;

            }
            var checkScriptMap = (IADs)objMachine.GetObject("IIsWebService", "W3SVC");
            if ((from var in (object[])checkScriptMap.Get("SCRIPTMAPS") select ((string)var).Split(',')).Any(mapping => mapping[0] == ".svc"))
            {
                WcfHttp = true;
            }
            Marshal.ReleaseComObject(objMachine);
        }

        
        public static IList<Site> Getsites
        {
            get
            {
                
                var retVal = new List<Site>();
                if (!GoOn())
                {
                    appPools = new List<string>();
                    return retVal;
                }
                var baseObj = (IADsContainer)Marshal.BindToMoniker(WEBSERVER);
                var venum = (IEnumVARIANT)baseObj._newEnum;
                Marshal.ReleaseComObject(baseObj);
                var regvar = new object[1];
                var fetched = new IntPtr();
                while (venum.Next(1, regvar, fetched) == 0)
                {
                    var aap = (IADs)regvar[0];
                    switch (aap.Class)
                    {
                        case "IIsWebServer":
                        {
                            string path = aap.ADsPath;
                            path = "/LM" + path.Remove(0, 15).Replace('\\', '/');
                            var comment = (string)aap.Get("ServerComment");
                            if (String.IsNullOrEmpty(comment))
                            {
                                comment = (string)aap.Get("AppFriendlyName");
                            }
                            retVal.Add(new Site()
                            {
                                AdsPath = path,
                                Instance = Int32.Parse(path.Substring(path.LastIndexOf('/') + 1)),
                                SiteName = comment
                            });
                        }
                            break;
                        case "IIsApplicationPools":
                            SetAppPools((IADsContainer)aap);
                            break;
                    }
                }
                Marshal.ReleaseComObject(venum);
                Marshal.ReleaseComObject(baseObj);
                return retVal;
            }
        }
        public static bool IISVdirExists(string sInstance, string virtDirName)
        {
            var baseObj = (IADsContainer)Marshal.BindToMoniker(WEBSERVER);
            try
            {
                var pParentDirCont = (IADsContainer)baseObj.GetObject("IIsWebVirtualDir", string.Concat(sInstance, "/", "ROOT"));
                try
                {
                    //it might exist already
                    var p = (IADs)pParentDirCont.GetObject("IIsWebVirtualDir", virtDirName);
                    Marshal.ReleaseComObject(p);
                }
                catch
                {
                    return false;

                }
                finally
                {
                    Marshal.ReleaseComObject(pParentDirCont);
                    Marshal.ReleaseComObject(baseObj);

                }
                return true;
            }
            catch
            {
            }
            return false;

        }
        internal static IADs CreateIISVDir(IADs parentDir, string virtDirName, string appPool, string physPath, string friendlyName)
        {
            if (parentDir == null) return null;
            if (!GoOn()) return null;
            var pParentDirCont = parentDir as IADsContainer;
            if (pParentDirCont == null)
            {
                return null;
            }
            IADs p;
            try
            {
                //it might exist already
                p = (IADs)pParentDirCont.GetObject("IIsWebVirtualDir", virtDirName);
            }
            catch
            {
                try
                { // eg. \inetpub\wwwroot\mydir already exists but is not an application.
                    // in that case the physpath does not need to be set.
                    p = (IADs)pParentDirCont.GetObject("IIsWebDirectory", virtDirName);
                }
                catch
                {
                    p = (IADs)pParentDirCont.Create("IIsWebVirtualDir", virtDirName);
                    p.Set("Path", physPath);
                }
            }

            p.Set("AccessScript", true);
            p.Set("AccessRead", true);

            var parentLmPath = parentDir.ADsPath;
            parentLmPath = string.Concat("/LM", parentLmPath.Remove(0, 15));
            //Type invoke = p.GetType();
            //invoke.InvokeMember("AppCreate", System.Reflection.BindingFlags.InvokeMethod, null, null, new object[] { typeof(bool) });


            p.Set("AppRoot", string.Concat(parentLmPath, "/", virtDirName));
            p.Set("AspAllowSessionState", false);
            p.Set("AccessFlags", 513); //read |execute
            p.Set("AppFriendlyName", friendlyName);
            p.Set("AppPoolid", appPool);
            p.SetInfo();
            return p;
        }
        public static bool RemoveIISVdir(string sInstance, string virtDirName)
        {
            if (!GoOn()) return false;
            var baseObj = (IADsContainer)Marshal.BindToMoniker(WEBSERVER);
            var pIISWebSErver = (IADsContainer)baseObj.GetObject("IIsWebVirtualDir", sInstance + "/ROOT");
            try
            {
                pIISWebSErver.Delete("IIsWebVirtualDir", virtDirName);
                return true;
            }
            catch (Exception ex)
            {
                Trace.TraceError("RemoveIISVdir failed with message {0}", ex.Message);
                return false;
            }
            finally
            {
                //does not remove physcial path IIsWebDirectory
                // basically, if it is a physical directory, the application just will be removed 
                Marshal.ReleaseComObject(pIISWebSErver);
                Marshal.ReleaseComObject(baseObj);
            }
        }
        public static IADs CreateIISVDir(string sInstance, string virtDirName, string appPool, string physPath, string friendlyName)
        {
            if (!GoOn()) return null;
            if (sInstance.StartsWith("/LM/"))
                sInstance = sInstance.Remove(0, 9);
            var baseObj = (IADsContainer)Marshal.BindToMoniker(WEBSERVER);
         
            var pIISWebSErver = (IADs)baseObj.GetObject("IIsWebVirtualDir", String.Concat(sInstance, "/", "ROOT"));
            Marshal.ReleaseComObject(baseObj);
            return CreateIISVDir(pIISWebSErver, virtDirName, appPool, physPath, friendlyName);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="appPool"></param>
        /// <param name="userAccount"></param>
        /// <param name="password"></param>
        /// <param name="classicMode"></param>
        /// <returns></returns>
        public static IADs CreateAppPool(string appPool, string userAccount, string password, bool classicMode)
        {
            if (!GoOn()) return null;
            var baseObj = (IADsContainer)Marshal.BindToMoniker(WEBSERVER + "/APPPOOLS");

            IADs pAppPool = null;

            try
            {
                pAppPool = (IADs)baseObj.GetObject(IIS_APP_POOL_CLASS, appPool);
            }
            catch
            {
                pAppPool = (IADs)baseObj.Create(IIS_APP_POOL_CLASS, appPool);
            }
            
            var identType = EnumAppPool.MD_APPPOOL_IDENTITY_TYPE_NETWORKSERVICE;
            if (!String.IsNullOrEmpty(userAccount))
            {
                pAppPool.Set("WAMUserName", userAccount);
                pAppPool.Set("WAMUserPass", password);
                identType = EnumAppPool.MD_APPPOOL_IDENTITY_TYPE_SPECIFICUSER;
            }
            pAppPool.Set("AppPoolIdentityType", identType);
            pAppPool.Set("ManagedRuntimeVersion", "v2.0");
            pAppPool.Set("ManagedPipelineMode", classicMode ? 1: 0);
            pAppPool.SetInfo();
            return pAppPool;
        }
        public static IADs GetIISVDir(string sInstance, string virtDirName)
        {
            if (!GoOn()) return null;
            sInstance = sInstance.StartsWith("/LM/") ? sInstance.Remove(0, 9) : sInstance;
            var baseObj = (IADsContainer)Marshal.BindToMoniker(WEBSERVER);
            return  (IADs)baseObj.GetObject("IIsWebVirtualDir", String.Concat(sInstance,"/ROOT", "/" , virtDirName));
          
        }
        private static IList<string> appPools;
        private static void SetAppPools(IADsContainer appPoolsContainer)
        {
            if (!GoOn()) return ;
            appPools = new List<string>();
            var venum = (IEnumVARIANT)appPoolsContainer._newEnum;
            var regvar = new object[1];
            var fetched = IntPtr.Zero;
            while (venum.Next(1, regvar, fetched) == 0)
            {
                var aap = (IADs)regvar[0];
                if (aap.Class == IIS_APP_POOL_CLASS)
                {
                    string path = aap.ADsPath;
                    appPools.Add(path.Substring(path.LastIndexOf('/') + 1));
                }
            }
            Marshal.ReleaseComObject(venum);
        }
        public static IList<string> getAppools
        {
            get
            {
                return appPools;
            }
        }
        /// <summary>
        /// retrieves the IIS rootpath e.g. c:\inetpub\wwwroot
        /// </summary>
        /// <param name="sInstance"></param>
        /// <returns></returns>
        public static string GetIISPath
        {
            get
            {
                //%SystemDrive%\inetpub\wwwroot
                //HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\InetSpt\PathWWWRoot (expandstring)
                //var obj = System.Runtime.InteropServices.Marshal.BindToMoniker("IIS://localhost/W3SVC") as IADs;
                //string temp = (string)obj.Get("KeyType");
                try
                {//expanding is done automatically
                    return (string)Registry.LocalMachine.OpenSubKey(@"SOFTWARE\Microsoft\InetStp", false).GetValue("PathWWWRoot", null);

                }
                catch
                {
                    //iis seems not installed here! ANyway, give a second chance
                    string temp = Environment.ExpandEnvironmentVariables(@"%SystemDrive%\inetpub\wwwroot");
                    return Directory.Exists(temp) ? temp : String.Empty;
                }
            }
        }
        /// <summary>
        /// retrieves the path for local IIS for a given instance where instance is a number like 1 in IIS://LOCALHOST/W3SVC/1/ROOT
        /// </summary>
        /// <param name="sInstance"></param>
        /// <returns></returns>
        public static string RootPath(string sInstance)
        {
            //if (AdsPath.StartsWith("/LM"))
            //{
            //    //remove /LM/W3SVC
            //    AdsPath = AdsPath.Remove(0, 9);
            //}
            //else if (AdsPath.StartsWith(WEBSERVER))
            //{
            //    AdsPath = AdsPath.Remove(0, WEBSERVER.Length);
            //}
            var baseObj = (IADs)Marshal.BindToMoniker(String.Concat(WEBSERVER, "/", sInstance, "/ROOT"));
            return (string)baseObj.Get("Path");
        }
    }
}
