using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Runtime.ConstrainedExecution;
using System.IO;
using System.Globalization;

namespace ADCCure.Configurator.DAL
{
    public static class MsiInterop
    {
        public enum tagINSTALLLEVEL 
        {
            INSTALLLEVEL_DEFAULT = 0,
            INSTALLLEVEL_MINIMUM = 1,
            INSTALLLEVEL_MAXIMUM = 0xFFFF
        }
        public enum tagINSTALLSTATE 
        {
            INSTALLSTATE_NOTUSED = -7,  // component disabled
            INSTALLSTATE_BADCONFIG = -6,  // configuration data corrupt
            INSTALLSTATE_INCOMPLETE = -5,  // installation suspended or in progress
            INSTALLSTATE_SOURCEABSENT = -4,  // run from source, source is unavailable
            INSTALLSTATE_MOREDATA = -3,  // return buffer overflow
            INSTALLSTATE_INVALIDARG = -2,  // invalid function argument
            INSTALLSTATE_UNKNOWN = -1,  // unrecognized product or feature
            INSTALLSTATE_BROKEN = 0,  // broken
            INSTALLSTATE_ADVERTISED = 1,  // advertised feature
            INSTALLSTATE_REMOVED = 1,  // component being removed (action state, not settable)
            INSTALLSTATE_ABSENT = 2,  // uninstalled (or action state absent but clients remain)
            INSTALLSTATE_LOCAL = 3,  // installed on local drive
            INSTALLSTATE_SOURCE = 4,  // run from source, CD or net
            INSTALLSTATE_DEFAULT = 5,  // use default, local or source
        } ;

        public enum tagINSTALLUILEVEL
        {
            INSTALLUILEVEL_NOCHANGE = 0,    // UI level is unchanged
            INSTALLUILEVEL_DEFAULT = 1,    // default UI is used
            INSTALLUILEVEL_NONE = 2,    // completely silent installation
            INSTALLUILEVEL_BASIC = 3,    // simple progress and error handling
            INSTALLUILEVEL_REDUCED = 4,    // authored UI, wizard dialogs suppressed
            INSTALLUILEVEL_FULL = 5,    // authored UI with wizards, progress, errors
            INSTALLUILEVEL_ENDDIALOG = 0x80, // display success/failure dialog at end of install
            INSTALLUILEVEL_PROGRESSONLY = 0x40, // display only progress dialog
            INSTALLUILEVEL_HIDECANCEL = 0x20, // do not display the cancel button in basic UI
            INSTALLUILEVEL_SOURCERESONLY = 0x100, // force display of source resolution even if quiet
        }
        internal enum tagServiceInfoLevel 
        {
            SERVICE_CONFIG_DELAYED_AUTO_START_INFO = 3,
            SERVICE_CONFIG_DESCRIPTION = 1,
            SERVICE_CONFIG_FAILURE_ACTIONS = 2,
            SERVICE_CONFIG_FAILURE_ACTIONS_FLAG = 4,
            SERVICE_CONFIG_PREFERRED_NODE = 9,
            SERVICE_CONFIG_PRESHUTDOWN_INFO = 7,
            SERVICE_CONFIG_REQUIRED_PRIVILEGES_INFO = 6,
            SERVICE_CONFIG_SERVICE_SID_INFO = 5,
            SERVICE_CONFIG_TRIGGER_INFO = 8
        }
        
        public enum tagMSICODE: uint
        {
            MSICODE_PRODUCT = 0x00000000, // product code provided
            MSICODE_PATCH = 0x40000000  // patch code provided
        }

        [Flags]
        public enum tagMSIINSTALLCONTEXT 
        {
            MSIINSTALLCONTEXT_FIRSTVISIBLE = 0,  // product visible to the current user
            MSIINSTALLCONTEXT_NONE = 0,  // Invalid context for a product
            MSIINSTALLCONTEXT_USERMANAGED = 1,  // user managed install context
            MSIINSTALLCONTEXT_USERUNMANAGED = 2,  // user non-managed context
            MSIINSTALLCONTEXT_MACHINE = 4,  // per-machine context
            MSIINSTALLCONTEXT_ALL = (MSIINSTALLCONTEXT_USERMANAGED | MSIINSTALLCONTEXT_USERUNMANAGED | MSIINSTALLCONTEXT_MACHINE),	// All contexts. OR of all valid values
            MSIINSTALLCONTEXT_ALLUSERMANAGED = 8,  // all user-managed contexts
        }
        internal const string msiDLL = "msi.dll";
        [DllImport(msiDLL, CharSet = CharSet.Unicode, SetLastError = false)]
        public static extern int MsiInstallProduct(
            [MarshalAs(UnmanagedType.LPWStr)] string szPackagePath,
                [MarshalAs(UnmanagedType.LPWStr)]  string szCommandLine);

      
        [DllImport(msiDLL, CharSet= CharSet.Unicode, SetLastError = false)]
        public static extern int MsiOpenProduct(
                [In, MarshalAs(UnmanagedType.LPWStr)] 
                        string productGUID,
                out MsiHandle msiHandle);

        [DllImport(msiDLL, CharSet = CharSet.Unicode, SetLastError = false)]
        public static extern int MsiOpenPackage(
                [MarshalAs(UnmanagedType.LPWStr)] 
                        string szPackagePath,
                out MsiHandle msiHandle);
         [DllImport(msiDLL,CharSet=CharSet.Unicode, SetLastError = false)]
        public static extern int MsiEnumClients(
               [MarshalAs(UnmanagedType.LPWStr)]
                string szComponent,
                int iProductIndex,
                [MarshalAs(UnmanagedType.LPWStr)]
                string lpProductBuf
            );
         internal const int ERROR_NO_MORE_ITEMS = 259;

         public static Guid GetLastProductCodeFromUpgradeCode(Guid upgradeCode)
         {
             var lst = GetProductCodeFromUpgradeCode(upgradeCode);
             
             return lst.Any()  ? lst.Last() : Guid.Empty;
         }
         public static bool IsUpgradeCodeInstalled(Guid upgradeCode)
         {
             var lst = GetProductCodeFromUpgradeCode(upgradeCode);
             return lst.Any();
         }
         public static IEnumerable<Guid> GetProductCodeFromUpgradeCode(Guid upgradeCode)
         {
             var upgradeCodeStr = upgradeCode.ToString("b");
             
             var buffer = new string('\0', 38);

             for (int x=0; ;x++ )
             {
                 int result = MsiEnumRelatedProducts(upgradeCodeStr, 0, x, buffer);
                 if (result == ERROR_NO_MORE_ITEMS)
                 {
                     break;
                 }
                 else if (result != 0)
                 {
                     throw new System.ComponentModel.Win32Exception(result);
                 }
                 yield return new Guid(buffer);
             }
         }

         public static bool IsMsiInstalled(Guid ProductCode)
         {
             if (ProductCode == Guid.Empty)
             {
                 return false;
             }
             MsiHandle msiHandle;
             int prod = MsiOpenProduct(ProductCode.ToString("b"), out msiHandle);
             if (!msiHandle.IsInvalid)
                 msiHandle.Close();

             return prod == 0;
         }
        /// <summary>
        /// the version from the MSI file
        /// </summary>
        /// <param name="msiFile"></param>
        /// <returns></returns>
         public static System.Version GetProductVersion(string msiFile)
         {
             MsiHandle msi;
             MsiInterop.MsiOpenPackage(msiFile, out msi);
             int buflen = 260;
             string buf = new string('\0', buflen);
             //does work only if ARPINSTALLLOCATION is included as a custom action set
             int msiResult = MsiGetProductProperty(msi, INSTALLPROPERTY_VERSIONSTRING, buf, ref buflen);
             if (msiResult == 0)
             {
                 msi.Close();
             }
             if (buflen > 0 && msiResult == 0)
             {

                 return new System.Version(buf.Substring(0, buflen));
             }

             return new System.Version("0.0.0");
         }
         public static System.Version GetProductVersion(Guid productGuid)
         {
             MsiHandle msi;
             MsiInterop.MsiOpenProduct(productGuid.ToString("b"), out msi);
             int buflen = 260;
             string buf = new string('\0', buflen);
             //does work only if ARPINSTALLLOCATION is included as a custom action set
             int msiResult = MsiGetProductProperty(msi, INSTALLPROPERTY_VERSIONSTRING, buf, ref buflen);
             if (msiResult == 0)
             {
                 msi.Close();
             }
             if (buflen > 0 && msiResult == 0)
             {

                 return new System.Version(buf.Substring(0, buflen));
             }
          
             return new System.Version(0,0,0);
         }

        /// <summary>
        ///calculate the path where this could have been installed
        /// unfortunately INSTALLPROPERTY_INSTALLLOCATION returns ""   
        /// </summary>
        /// <param name="guid">this product Guid equals ProductCode in MSI projects </param>
        /// <param name="targetPath"></param>
        /// <returns></returns>
        public static string GetProductInstallLocation(Guid productGuid, params string[] targetPath)
        {
            MsiHandle msi;
            MsiInterop.MsiOpenProduct(productGuid.ToString("b"), out msi);
           int buflen = 260;
            string buf = new string('\0', buflen);
            //does work only if ARPINSTALLLOCATION is included as a custom action set
             int msiResult = MsiGetProductProperty(msi, INSTALLPROPERTY_INSTALLLOCATION, buf, ref buflen);
             if (msiResult == 0)
            {
                msi.Close();
            }
             if (buflen > 0 && msiResult == 0)
            {
                return buf.Substring(0, buflen);
            }
             buf = null;
            //use slow method

            // ARPINSTALLLOCATION
            int valbuf = 38; //maxpath
            string ComponentGuid = new string('\0', valbuf);
            string ProductGuid = new string('\0', valbuf);
            string searchforGuid = productGuid.ToString("b");
            buflen = 260;
            string path = new string('\0', buflen);          
            valbuf++;
            for (int result = 0, x = 0; ; x++)
            {
                result = MsiEnumComponents(x, ComponentGuid);
                if (result != 0) break;
                for (int inner = 0, resulti = 0; resulti == 0; inner++)
                {
                    resulti = MsiEnumClients(ComponentGuid, inner, ProductGuid);
                    if (new Guid(ProductGuid) == productGuid)
                    {
                        buflen = 261;
                        resulti = MsiGetComponentPath(searchforGuid, ComponentGuid, path, ref buflen);
                        string sub = path.Substring(0, buflen);

                        if (!string.IsNullOrEmpty(Array.Find(targetPath, p=> sub.EndsWith(p, StringComparison.OrdinalIgnoreCase))))
                        {
                            return Path.GetDirectoryName(sub);
                        }
                    }
                }
            }
            return string.Empty;
        }

        //public static string GetProductProperty(string msiPackage, string propertyRequested)
        //{
        //    int valbuf = 260; //maxpath
        //    string buf = new string('\0', valbuf);
        //    MsiHandle msh;
        //    int result = MsiInterop.MsiOpenPackage(msiPackage, out msh);
        //    int aap = MsiGetProductProperty(msh, propertyRequested, buf, ref valbuf);

        //    msh.Close();
        //    if (aap == 0)
        //    {
        //        return buf.Substring(0, valbuf);
        //    }
        //    return string.Empty;
        //}
        public static string GetProductProperty(Guid guid, string propertyRequested)
        {
            int valbuf = 260; //maxpath
            string buf = new string('\0', valbuf);
            MsiHandle msh;
            int result = MsiOpenProduct(guid.ToString("b"), out msh);
            int aap = MsiGetProductProperty(msh, propertyRequested, null, ref valbuf);
           
            msh.Close();
            if (aap == 0)
            {
                return buf.Substring(0, valbuf);
            }
            return string.Empty;
        }
        [DllImport(msiDLL, SetLastError = false)]
        internal static extern int MsiSetInternalUI(tagINSTALLUILEVEL dwUILevel, ref MsiHandle phWnd);
       
        [DllImport(msiDLL, CharSet=CharSet.Unicode, SetLastError = false)]
        public static extern int MsiGetProductProperty( MsiHandle hProduct,
            [MarshalAs(UnmanagedType.LPWStr)]
             string szProperty,
            [MarshalAs(UnmanagedType.LPWStr, SizeParamIndex=3)]
             string lpValueBuf,
            ref int pcchValueBuf);

        [DllImport(msiDLL, CharSet = CharSet.Unicode)]
        public static extern int MsiSourceListEnumSources(
                [MarshalAs(UnmanagedType.LPWStr)]
                string szProductCodeOrPatchCode,
                [MarshalAs(UnmanagedType.LPWStr)]
                string szUserSid, 
                [MarshalAs(UnmanagedType.U4)]
                tagMSIINSTALLCONTEXT dwContext, 
                [MarshalAs(UnmanagedType.U4)]
                tagMSICODE dwOptions, 
            int dwIndex,
              [MarshalAs(UnmanagedType.LPWStr, SizeParamIndex=6)]
            string szSource, ref int pcchSource);
        [DllImport(msiDLL, CharSet=CharSet.Unicode, SetLastError = false)]
         internal static extern int MsiEnumComponents(
            int iComponentIndex,
            [MarshalAs(UnmanagedType.LPWStr)]
            string lpComponentBuf);

        [DllImport(msiDLL, CharSet = CharSet.Unicode, SetLastError = false)]
        internal static extern int MsiEnumRelatedProducts(
             [In, MarshalAs(UnmanagedType.LPWStr)]
            string lpUpgradeCode,
                int dwReserved,
                int iProductIndex,
             [MarshalAs(UnmanagedType.LPWStr)]
                 string lpProductBuf);


   [DllImport(msiDLL, CharSet=CharSet.Unicode, SetLastError = false)]
        internal static extern int MsiGetComponentPath(
            [MarshalAs(UnmanagedType.LPWStr)]
            string szProduct,
            [MarshalAs(UnmanagedType.LPWStr)]
            string  szComponent,
  [MarshalAs(UnmanagedType.LPWStr, SizeParamIndex=3)]
            string   lpPathBuf,
            ref  int pcchBuf
            );

        [DllImport(msiDLL, CharSet=CharSet.Unicode, SetLastError = false)]
        internal static extern int MsiConfigureProductEx(
            [MarshalAs(UnmanagedType.LPWStr)] 
                    string szProduct,
            [MarshalAs(UnmanagedType.U4)] tagINSTALLLEVEL iInstallLevel,
            [MarshalAs(UnmanagedType.U4)] tagINSTALLSTATE eInstallState,
            [MarshalAs(UnmanagedType.LPWStr)] 
                    string szCommandLine);
        public const string INSTALLPROPERTY_INSTALLLOCATION  = "InstallLocation";
        public const string INSTALLPROPERTY_PUBLISHER = "Publisher";
        public const string INSTALLPROPERTY_VERSIONSTRING = "ProductVersion";

    }
    /// <summary>
    /// represents a wrapped *MSIHANDLE
    /// </summary>
    public sealed class MsiHandle : SafeHandle
    {
        public MsiHandle()
            : base(IntPtr.Zero, true)
        {
        }
        [DllImport(MsiInterop.msiDLL, SetLastError = false)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        private static extern int MsiCloseHandle(IntPtr handle);

        public override bool IsInvalid
        {
            get { return IsClosed || handle == IntPtr.Zero; }
        }

        protected override bool ReleaseHandle()
        {
            return MsiCloseHandle(this.handle) == 0;
        }
    }
}