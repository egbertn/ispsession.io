using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using ispsession.io.setup.util;
using System.IO;
using System.Diagnostics;
using System.Security.AccessControl;
using System.Security.Principal;

namespace ispsession.io.setup
{
    public static class Util
    {

        internal static IList<IPAddress> GetLocalAddresses()
        {
            //System.Net.NetworkInformation.NetworkInterface.GetAllNetworkInterfaces()[0].GetIPProperties().GetIPv4Properties().
            return (Dns.GetHostAddresses(Dns.GetHostName()));
        }

        public sealed class Site
        {
            public string SiteName { get; set; }
            public string AdsPath { get; set; }
            public int Instance { get; set; }

            public override string ToString()
            {
                return SiteName;
            }
        }

    /// <summary>
        /// ensures that the DLL allows the network service account
        /// </summary>
        /// <param name="file"></param>
        internal static void FixSecurity(string file, bool isDir)
        {
            bool exists = isDir ? Directory.Exists(file) : File.Exists(file);
            if (isDir && !exists)
            {
                Directory.CreateDirectory(file);
                exists = true;
            }
            if (exists)
            {
                var f = isDir ?
                    (FileSystemSecurity)Directory.GetAccessControl(file, AccessControlSections.Access)
                    :
                    File.GetAccessControl(file, AccessControlSections.Access);

                //SecurityIdentifier iis = new SecurityIdentifier(WellKnownSidType.i, null);
                var identifier = new SecurityIdentifier(isDir ? WellKnownSidType.LocalServiceSid : WellKnownSidType.NetworkServiceSid, null);

                var requestedSec = isDir ? new FileSystemAccessRule(identifier, FileSystemRights.FullControl, InheritanceFlags.ContainerInherit, PropagationFlags.InheritOnly, AccessControlType.Allow)
                    : new FileSystemAccessRule(identifier, FileSystemRights.ReadAndExecute, AccessControlType.Allow);
                bool found = f.GetAccessRules(true, true, typeof (SecurityIdentifier)).
                    Cast<AccessRule>().Any(authRule => authRule.IdentityReference == requestedSec.IdentityReference && authRule.AccessControlType == AccessControlType.Allow);
                if (!found)
                {

                    f.AddAccessRule(requestedSec);
                    if (isDir)
                    {
                        Directory.SetAccessControl(file, (DirectorySecurity)f);

                    }
                    else
                        File.SetAccessControl(file, (FileSecurity)f);
                }
            }
        }
    
        //internal static void RemoveCommApp()
        //{
        //    Trace.TraceInformation("Enter RemoveCommApp");
        //    var comadm = Installer.GetComadminCatalog();
        //    var objApplicationsColl = (ICatalogCollection)comadm.GetCollection("Applications");

        //    objApplicationsColl.Populate();
        //    for (int iItem = 0; iItem < objApplicationsColl.Count; iItem++)
        //    {
        //        var objZooApp = (ICatalogObject)objApplicationsColl.get_Item(iItem);
        //        if (((string)objZooApp.Name).StartsWith(Installer.PRODUCT))
        //        {
        //            Trace.WriteLine("Removing " + Installer.PRODUCT);
        //            objApplicationsColl.Remove(iItem);
        //            break;
        //        }
        //    }
        //    objApplicationsColl.SaveChanges();
        //    Trace.TraceInformation("Exit RemoveCommApp");
        //}
        //internal static void ASPService(bool install)
        //{
            //string sPath = Path.Combine(Path.Combine(AppInfo.CurrentPath, "Service"), "ASPSession.exe");
            //MessageBox.Show(string.Format("{0}+{1}", GetInstallUtil(), sPath));
            //var proc = Process.Start(sPath, (install ? "-service " : "-UnregServer"));
            //bool result = proc.Start();
            //result = proc.WaitForExit(5000);

        //}
        static string GetInstallUtil()
        {
            object ob = new object();
            return Path.Combine(Path.GetDirectoryName(ob.GetType().Assembly.Location), "InstallUtil.exe");
        }
        //static string GetNetPath()
        //{
        //    RegistryKey reg = Registry.LocalMachine.OpenSubKey("Software\\Microsoft\\.NETFramework", false);
        //    return (string)reg.GetValue("InstallRoot", null, RegistryValueOptions.DoNotExpandEnvironmentNames);
        //}
        internal const string ISPSessionGuid = "{D64ABC73-3B0E-4E85-92CD-215B03CAC996}";
        static bool isCSessionInstalled()
        {

            try
            {
                //Activator.CreateInstance(type);
                Type type = Type.GetTypeFromCLSID(new Guid(ISPSessionGuid));
                return true;
            }
            catch
            {
                return false;
            }
        }
     
        //internal static void InstallCOMApp(int lConnPool)
        //{
        //    Trace.TraceInformation("InstallCOMApp {0}", lConnPool);
        //    var comadm = Installer.GetComadminCatalog();
        //    var objApplicationsColl = (ICatalogCollection)comadm.GetCollection("Applications");
        //    var objZooApp = (ICatalogObject)objApplicationsColl.Add();
        //    objZooApp.set_Value("Activation", COMAdminActivationOptions.COMAdminActivationInproc);
        //    string appName, sGUID;
        //    appName = Installer.PRODUCT + (AppInfo.CurrentPath.IndexOf("x86") > 0 ? "(32 bit)" : string.Empty);

        //    objZooApp.set_Value("Name", appName);
        //    objZooApp.set_Value("Description", "Provides Load balancing for ASP Session state management");
        //    objZooApp.set_Value("AccessChecksLevel", COMAdminAccessChecksLevelOptions.COMAdminAccessChecksApplicationLevel);
        //    objZooApp.set_Value("ApplicationAccessChecksEnabled", false);
        //    sGUID = (string)objZooApp.Key;
        //    objApplicationsColl.SaveChanges();
        //    string arch = Path.Combine(AppInfo.CurrentPath, "CSession.DLL");
        //    Trace.TraceInformation("CSession.DLL GUID {0}", sGUID);
        //    Util.FixSecurity(arch, false);
        //    comadm.ImportComponent(sGUID, ISPSessionGuid);
        //    //comadm.InstallComponent(sGUID, arch, null, null);
        //    objApplicationsColl = (ICatalogCollection)objApplicationsColl.GetCollection("Components", sGUID);
        //    objApplicationsColl.Populate();
        //    var oComObj = (ICatalogObject)objApplicationsColl.get_Item(0);
        //    oComObj.set_Value("IISIntrinsics", true);
        //    oComObj.set_Value("EventTrackingEnabled", false);
        //    oComObj.set_Value("Transaction", COMAdminTransactionOptions.COMAdminTransactionNone);
        //    oComObj.set_Value("MaxPoolSize", lConnPool); //TODO
        //    oComObj.set_Value("JustInTimeActivation", false);
        //    oComObj.set_Value("Synchronization", COMAdminSynchronizationOptions.COMAdminSynchronizationNone);
        //    oComObj.set_Value("ObjectPoolingEnabled", true);
        //    objApplicationsColl.SaveChanges();
        //    Trace.TraceInformation("Exit InstallCOMapp");
        //}
        
    }
}  
