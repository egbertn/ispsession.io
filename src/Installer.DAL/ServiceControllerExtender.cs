using System;
using System.ComponentModel;
using System.ServiceProcess;
using System.Runtime.InteropServices;
using System.Security.Principal;
using ADCCure.Configurator.DAL;
namespace ADCCure.Configurator.Config
{
    public static class ServiceControllerExtension
    {
        
        const int SERVICE_NO_CHANGE = -1;// (&HFFFFFFFF)
        const int ERROR_INSUFFICIENT_BUFFER = 122;
        enum EnumInfoLevel : int
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
        #region PInvokes

        [DllImport(NtSecurityInterop.ADVAPI32, SetLastError = true)]
        internal static extern bool DeleteService(SafeHandle handle);

        [DllImport(NtSecurityInterop.ADVAPI32, CharSet = CharSet.Unicode, SetLastError = true)]
        static extern bool QueryServiceConfig2
                (SafeHandle hService,
                [MarshalAs(UnmanagedType.U4)] int dwInfoLevel,
                out IntPtr lpBuffer,
                int cbBufferSize,
                out int pcbBytesNeeded);

        [DllImport(NtSecurityInterop.ADVAPI32, CharSet = CharSet.Unicode, SetLastError = true)]
        static extern bool ChangeServiceConfig(
             SafeHandle hService,
             [MarshalAs(UnmanagedType.U4)]
            ServiceType dwServiceType,
             [MarshalAs(UnmanagedType.U4)] 
                ServiceStartMode dwStartType, int dwErrorControl,
             [In, MarshalAs(UnmanagedType.LPWStr)] string lpBinaryPathName,
             [In, MarshalAs(UnmanagedType.LPWStr)] string lpLoadOrderGroup,
            int lpdwTagId, //is out-opt specify 0
            // in fact an array
            [In, MarshalAs(UnmanagedType.LPWStr)] string lpDependencies,
             [In, MarshalAs(UnmanagedType.LPWStr)]
            string lpServiceStartName,
            [In, MarshalAs(UnmanagedType.LPWStr)]string lpPassWord,
            [In, MarshalAs(UnmanagedType.LPWStr)]string lpDisplayName);
        [DllImport(NtSecurityInterop.ADVAPI32, CharSet = CharSet.Unicode, SetLastError = true)]
        static extern bool ChangeServiceConfig2(SafeHandle hService,
                                                        [MarshalAs(UnmanagedType.U4)]
                                                        EnumInfoLevel dwInfoLevel, [In] ref InfoValue nfo);

        [DllImport(NtSecurityInterop.ADVAPI32, CharSet = CharSet.Unicode, SetLastError = true)]
        static extern bool QueryServiceConfig(SafeHandle hService,
                IntPtr lpServiceConfig,
                int cbBufSize,
                out int pcbBytesNeeded);

        #endregion
        // needed for ChangeServiceConfig2
        [StructLayout(LayoutKind.Explicit)]
        struct InfoValue
        {
            // cannot use internal  string + marshalas lpwstr because of fieldoffset(0) marshall error in .net
            [FieldOffset(0)]
            internal IntPtr lpDescription;
            [FieldOffset(0), MarshalAs(UnmanagedType.Bool)]
            internal bool fDelayedAutostart;
            [FieldOffset(0), MarshalAs(UnmanagedType.U4)]
            internal int dwServiceSidType; //none-0, SERVICE_SID_TYPE_RESTRICTED =3 or SERVICE_SID_TYPE_UNRESTRICTED =1 
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        struct QUERY_SERVICE_CONFIG
        {
            [MarshalAs(UnmanagedType.U4)]
            internal ServiceType dwServiceType;
            [MarshalAs(UnmanagedType.U4)]
            internal ServiceStartMode dwStartType;
            internal int dwErrorControl;
            [MarshalAs(UnmanagedType.LPWStr)]
            internal string lpBinaryPathName;
            [MarshalAs(UnmanagedType.LPWStr)]
            internal string lpLoadOrderGroup;
            internal int dwTagId;
            [MarshalAs(UnmanagedType.LPWStr)]
            internal string lpDependencies;
            [MarshalAs(UnmanagedType.LPWStr)]
            internal string lpServiceStartName;
            [MarshalAs(UnmanagedType.LPWStr)]
            internal string lpDisplayName;
        }

        /// <summary>
        /// Sets the Windows service startup mode to specified mode
        /// </summary>
        /// <param name="userId">sets the service username. </param>
        /// <param name="password">sets the service password</param>
        /// <param name="displayName">sets the service description name</param>
        /// <param name="controller"></param>
        public static void SetServiceStartupMode(this ServiceController controller, ServiceStartMode mode)
        {
            if (controller == null)
            {
                throw new ArgumentNullException("controller");
            }

            bool result = ChangeServiceConfig(controller.ServiceHandle,
                (ServiceType)SERVICE_NO_CHANGE, mode,
                   SERVICE_NO_CHANGE, null, null, 0, null, null, null, null);
            if (!result)
            {
                int win32Err = Marshal.GetLastWin32Error();
                throw new Win32Exception(win32Err);
            }
        }

        public static void RemoveService(this ServiceController controller)
        {
            if (controller == null)
            {
                throw new ArgumentNullException("controller");
            }
            bool result = DeleteService(controller.ServiceHandle);

            if (!result)
            {
                int win32Err = Marshal.GetLastWin32Error();
                throw new Win32Exception(win32Err);
            }
        }
        public static void StopService(string serviceName)
        {
            var controller = new ServiceController(serviceName);
            if (controller.Status == ServiceControllerStatus.Running)
            {
                controller.Stop();
                controller.WaitForStatus(ServiceControllerStatus.Stopped);
            }
        }
        public static void SetServiceDescription(this ServiceController controller, string description)
        {
            if (controller == null)
            {
                throw new ArgumentNullException("controller");
            }
            // if "" then the description deleted msnd
            //http://msdn.microsoft.com/en-us/library/ms685156(v=VS.85).aspx
            if (description != null)
            {
                var value = new InfoValue();
                var str = Marshal.StringToCoTaskMemUni(description);
                value.lpDescription = str;
                bool result = ChangeServiceConfig2(controller.ServiceHandle, EnumInfoLevel.SERVICE_CONFIG_DESCRIPTION, ref value);
                Marshal.FreeCoTaskMem(str);
                if (!result)
                {
                    int win32Err = Marshal.GetLastWin32Error();
                    throw new Win32Exception(win32Err);
                }
            }
        }
        public static void SetServiceName(this ServiceController controller, string displayName)
        {
            if (controller == null)
            {
                throw new ArgumentNullException("controller");
            }
            bool result = ChangeServiceConfig(controller.ServiceHandle, (ServiceType)SERVICE_NO_CHANGE, (ServiceStartMode)SERVICE_NO_CHANGE,
                    SERVICE_NO_CHANGE, null, null, 0, null, null, null, displayName);
            if (!result)
            {
                int win32Err = Marshal.GetLastWin32Error();
                throw new Win32Exception(win32Err);
            }

            // if "" then the description deleted msnd
            //http://msdn.microsoft.com/en-us/library/ms685156(v=VS.85).aspx

        }
        /// <summary>
        /// returns LocalSystem or DOMAIN\user
        /// </summary>
        /// <param name="controller"></param>
        /// <returns></returns>
        public static NTAccount GetServiceLogin(this ServiceController controller)
        {
            int needed = 0;
            QueryServiceConfig(controller.ServiceHandle, IntPtr.Zero, 0, out needed);
            var ptr = IntPtr.Zero;
            try
            {
                ptr = Marshal.AllocCoTaskMem(needed);
                bool result = QueryServiceConfig(controller.ServiceHandle, ptr, needed, out needed);
                if (result)
                {
                    var cfg = Marshal.PtrToStructure< QUERY_SERVICE_CONFIG>(ptr);


                    var nta = NtSecurityInterop.LookupAccount(cfg.lpServiceStartName);
                    return nta;
                }
                else
                {
                    int dwErr = Marshal.GetLastWin32Error();
                    throw new Win32Exception(dwErr);
                }
            }
            finally
            {
                if (ptr != IntPtr.Zero)
                {
                    Marshal.FreeCoTaskMem(ptr);
                }
            }
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="serviceName">The name by which NT addresses this service</param>
        /// <param name="userId"></param>
        /// <param name="password"></param>
        public static void SetServiceControllerLogin(string serviceName, string userId, string password)
        {
            var svc = new ServiceController(serviceName);
            var acc = new NTAccount(userId);
            svc.SetServiceLogin(acc, password, null);
        }

        /// <summary>
        /// Sets Windows service to specified username and password and description
        /// if userid is a windows builtin account such as localsystem leave password null
        /// </summary>      
        /// <param name="userId"></param>
        /// <param name="password"></param>
        /// <param name="displayName">null for nothing "" for delete, or @[path\]dllname,-strID for multi language</param>
        public static void SetServiceLogin(this ServiceController controller, NTAccount userId, string password, string displayName)
        {
            if (controller == null)
            {
                throw new ArgumentNullException("controller");
            }

            var result = ChangeServiceConfig(controller.ServiceHandle, (ServiceType)SERVICE_NO_CHANGE, (ServiceStartMode)SERVICE_NO_CHANGE,
                SERVICE_NO_CHANGE, null, null, 0, null, userId.Value, password, null);
            if (!result)
            {
                int win32err = Marshal.GetLastWin32Error();
                throw new Win32Exception(win32err);
            }
            // if "" then the description deleted msnd
            //http://msdn.microsoft.com/en-us/library/ms685156(v=VS.85).aspx
            if (displayName != null)
            {
                InfoValue value = new InfoValue();
                IntPtr str = Marshal.StringToCoTaskMemUni(displayName);
                value.lpDescription = str;
                result = ChangeServiceConfig2(controller.ServiceHandle, EnumInfoLevel.SERVICE_CONFIG_DESCRIPTION, ref value);
                Marshal.FreeCoTaskMem(str);
                if (!result)
                {
                    int win32Err = Marshal.GetLastWin32Error();
                    throw new Win32Exception(win32Err);                    
                }
            }
        }

        public static ServiceStartMode GetServiceStartupMode(this ServiceController svc)
        {
            if (svc == null) throw new ArgumentNullException("svc");
            int neededBytes = 0;

            bool result = QueryServiceConfig(svc.ServiceHandle, IntPtr.Zero, 0, out neededBytes);
            int win32err = Marshal.GetLastWin32Error();
            if (win32err == ERROR_INSUFFICIENT_BUFFER)
            {
                IntPtr ptr = IntPtr.Zero;
                try
                {
                    ptr = Marshal.AllocCoTaskMem(neededBytes);
                    result = QueryServiceConfig(svc.ServiceHandle, ptr, neededBytes, out neededBytes);
                    win32err = Marshal.GetLastWin32Error();
                    if (result)
                    {
                        var config = Marshal.PtrToStructure< QUERY_SERVICE_CONFIG>(ptr);
                        return config.dwStartType;
                    }
                    else
                    {
                        throw new Win32Exception(win32err, "QueryServiceConfig failed");
                    }
                }
                finally
                {
                    if (ptr != IntPtr.Zero)
                    {
                        Marshal.FreeCoTaskMem(ptr);
                    }
                }
            }
            throw new Win32Exception(win32err, "QueryServiceConfig failed");
        }
    }
}