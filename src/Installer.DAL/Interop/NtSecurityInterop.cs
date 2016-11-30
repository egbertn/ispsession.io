using System;
using System.Collections.Generic;
using System.ServiceProcess;
using System.Text;
using System.Runtime.InteropServices;
using System.Runtime.ConstrainedExecution;
using System.ComponentModel;
using System.IO;
using System.Security.Principal;
using System.Diagnostics;
using System.Security.AccessControl;

namespace ADCCure.Configurator.DAL
{
    public static class NtSecurityInterop
    {
        [StructLayout(LayoutKind.Sequential)]
        public struct LSA_UNICODE_STRING
        {
            public LSA_UNICODE_STRING(string wrapString)
            {
                Length = (short)(wrapString.Length * 2);
                MaximumLength = (short)(Length + 2); //wchar
                Buffer = wrapString;
            }
            public short Length;
            public short MaximumLength;
            [MarshalAs(UnmanagedType.LPWStr)]
            public string Buffer;
        }

        public struct LUID
        {
            public long number;
        }

        //public enum LSA_AccessPolicy : long
        //{
        //    POLICY_VIEW_LOCAL_INFORMATION = 0x00000001L,
        //    POLICY_VIEW_AUDIT_INFORMATION = 0x00000002L,
        //    POLICY_GET_PRIVATE_INFORMATION = 0x00000004L,
        //    POLICY_TRUST_ADMIN = 0x00000008L,
        //    POLICY_CREATE_ACCOUNT = 0x00000010L,
        //    POLICY_CREATE_SECRET = 0x00000020L,
        //    POLICY_CREATE_PRIVILEGE = 0x00000040L,
        //    POLICY_SET_DEFAULT_QUOTA_LIMITS = 0x00000080L,
        //    POLICY_SET_AUDIT_REQUIREMENTS = 0x00000100L,
        //    POLICY_AUDIT_LOG_ADMIN = 0x00000200L,
        //    POLICY_SERVER_ADMIN = 0x00000400L,
        //    POLICY_LOOKUP_NAMES = 0x00000800L,
        //    POLICY_NOTIFICATION = 0x00001000L
        //}
        [StructLayout(LayoutKind.Sequential)]
        struct LSA_OBJECT_ATTRIBUTES
        {
            public int Length;
            public IntPtr RootDirectory;
            public IntPtr ObjectName; //IntPtr is of LSA_UNICODE_STRING
            public uint Attributes;
            public IntPtr SecurityDescriptor;
            public IntPtr SecurityQualityOfService;
        }

        sealed class SafeLsaHandle : SafeHandle
        {
            public SafeLsaHandle()
                : base(IntPtr.Zero, true)
            {
            }
            #region PInvokes
            [DllImport(ADVAPI32, SetLastError = false)]
            public static extern uint LsaClose(IntPtr ObjectHandle);
            #endregion
            public override bool IsInvalid
            {
                get { return IsClosed || handle == IntPtr.Zero; }
            }

            protected override bool ReleaseHandle()
            {
                return LsaClose(this.handle) == 0;
            }
        }
        /// <summary>
        /// SID_NAME_USE as System.Security.Principal.SidNameUse
        /// </summary>
        internal enum SidUse
        { 
          SidTypeUser            = 1,
          SidTypeGroup,
          SidTypeDomain,
          SidTypeAlias,
          SidTypeWellKnownGroup,
          SidTypeDeletedAccount,
          SidTypeInvalid,
          SidTypeUnknown,
          SidTypeComputer,
          SidTypeLabel
            } 

        [Flags]
        internal enum EnumAdsUserFlags : int
        {
            ADS_UF_DONT_EXPIRE_PASSWD = 0x10000,
            ADS_UF_PASSWD_CANT_CHANGE = 0x40
        }
        [Flags]
        public enum LocalMemoryFlags
        {
            LMEM_FIXED = 0x0000,
            LMEM_MOVEABLE = 0x0002,          
            LMEM_ZEROINIT = 0x0040,
            LMEM_MODIFY = 0x0080,
            LMEM_VALID_FLAGS = 0x0F72,
            LMEM_INVALID_HANDLE = 0x8000,
            LHND = (LMEM_MOVEABLE | LMEM_ZEROINIT),
            LPTR = (LMEM_FIXED | LMEM_ZEROINIT),
            NONZEROLHND = (LMEM_MOVEABLE),
            NONZEROLPTR = (LMEM_FIXED)
        }
        #region PInvokes
        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern IntPtr LocalAlloc(
            [MarshalAs(UnmanagedType.U4)] LocalMemoryFlags uFlags = LocalMemoryFlags.LPTR, 
         [MarshalAs(UnmanagedType.SysInt)] 
         int uBytes=0);
        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern IntPtr LocalFree(IntPtr hMem);

        [DllImport(ADVAPI32, CharSet = CharSet.Unicode, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool LookupAccountName(
            [MarshalAs(UnmanagedType.LPWStr)] string systemName,
            [MarshalAs(UnmanagedType.LPWStr)] string accountName,
            [Out, MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.U1, SizeParamIndex = 3)] 
            byte[] sid,
            ref int cbSid,
            [MarshalAs(UnmanagedType.LPWStr)] string referencedDomainName,
            ref int cchReferenceDomainName,
            out SidUse peUSe);



        public  delegate void wincallback(IntPtr param);
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        internal struct ServiceNotify
        {
            internal int dwVersion;
            internal wincallback pfnNotifyCallback;
            internal IntPtr pContext;
            internal int dwNotificationStatus;
            [MarshalAs(UnmanagedType.U4)]
            ServiceControllerStatus ServiceStatus;
            internal int dwNotificationTriggered;
            internal string pszServiceNames;

        };
        
        [DllImport(ADVAPI32, CharSet = CharSet.Unicode, SetLastError = true)]
        private static extern int NotifyServiceStatusChange(SafeHandle hService, int dwNotifyMask, ServiceNotify notify );

        [DllImport(ADVAPI32, CharSet = CharSet.Unicode, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool LookupAccountSid(
            [MarshalAs(UnmanagedType.LPWStr)]
            string systemName,
            [In, MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.U1)]
             byte[] sid,
            [MarshalAs(UnmanagedType.LPWStr), Out]
            string name,
            ref int lengthName,
            [MarshalAs(UnmanagedType.LPWStr), Out]
            string DomainName,
            out SidUse sidUse
            );
   
        [DllImport(ADVAPI32, CharSet = CharSet.Unicode, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool LogonUser(
                            [MarshalAs(UnmanagedType.LPWStr)]
                            string lpszUserName,
                            [MarshalAs(UnmanagedType.LPWStr)]
                            string lpszDomain,
                            [MarshalAs(UnmanagedType.LPWStr)]
                            string lpszPassword,
                            int dwLogonType,
                            int dwLogonProvider, out Win32Handle hToken);

        [DllImport(ADVAPI32, SetLastError = true)]
        static extern int LsaRemoveAccountRights(IntPtr PolicyHandle,
              [In, MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.U1)]
                byte[] AccountSid,
              [MarshalAs(UnmanagedType.Bool)] bool AllRights,
              [In, MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 4)]
                LSA_UNICODE_STRING[] UserRights, int CountOfRights);

        [DllImport(ADVAPI32, SetLastError = true)]
        static extern int LsaAddAccountRights(IntPtr PolicyHandle,
                                        [In, MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.U1)]
                                                    byte[] AccountSid,
                                                [In, MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)]
                                                    LSA_UNICODE_STRING[] UserRights,
                                                int CountOfRights);

        [DllImport(ADVAPI32, SetLastError = true)]
        static extern int LsaOpenPolicy(
               [In] ref LSA_UNICODE_STRING SystemName,
               [In] ref LSA_OBJECT_ATTRIBUTES ObjectAttributes,
               uint DesiredAccess,
               out IntPtr handle);

        [DllImport(ADVAPI32, SetLastError = false)]
        static extern int LsaNtStatusToWinError(int Status);
        #endregion

        internal const string ADVAPI32 = "Advapi32.dll";        
        private const int MAXPATH = 260;


        const string SE_SERVICE_LOGON_NAME = "SeServiceLogonRight";
        public static bool TestLogonWrap(string userName, string passWord)
        {
            try
            {
                TestLogon(userName, passWord);
                return true;
            }
            catch (Win32Exception ex)
            {
                if ((uint)ex.ErrorCode == 0x80004005)
                {
                    return false;
                }
                throw;
            }
        }
       
        /// <summary>
        /// just test if the password is ok
        /// we don't need to interactively logon or assume the service right logon
        /// </summary>
        /// <param name="userName"></param>
        /// <param name="passWord"></param>
        /// <returns></returns>
        public static bool TestLogon(string userName, string passWord)
        {
            string domain = null;
            int findSlash = userName.IndexOf('\\');
            const int LOGON32_PROVIDER_DEFAULT = 0;
            const int LOGON32_LOGON_NETWORK = 3;

            if (findSlash > 0)
            {
                domain = userName.Substring(0, findSlash);
                userName = userName.Substring(findSlash + 1);
            }
            Win32Handle usr;
            if (LogonUser(userName, domain, passWord, LOGON32_LOGON_NETWORK, LOGON32_PROVIDER_DEFAULT, out usr) == false)
            {
                int win32Err = Marshal.GetLastWin32Error();
                util.WriteToLog(string.Format("logon for user {0} failed", userName), util.MessageType.Warning);
             //   throw new Win32Exception(win32Err);
            }
            return true;

        }
        public static bool UserExists(string userName, out bool isAdmin, out NTAccount nta)
        {

            var idxSlash = userName.IndexOf('\\');
            nta = null;
            isAdmin = false;
            var machine = idxSlash >= 0 ? userName.Substring(0, idxSlash) : Environment.MachineName;
            if (idxSlash > 0)
            {
                userName = userName.Substring(idxSlash + 1);
            }
            if (machine == ".")
            {
                machine = Environment.MachineName;
            }
            var samLocal = (IADsContainer)Marshal.BindToMoniker(string.Format("WinNT://{0},computer", machine));

            try
            {
                var newUser = (IADsUser)samLocal.GetObject("user", userName);
                var sidToReturn = new SecurityIdentifier((byte[])newUser.Get("objectSid"), 0);
                nta = (NTAccount)sidToReturn.Translate(typeof(NTAccount));
                var admins = AdminGroup;
                var adminName = admins.Value;
                var slsh = adminName.IndexOf('\\');
                if (slsh > 0)
                {
                    adminName = adminName.Substring(slsh + 1);
                }
                var grp = (IADsGroup)samLocal.GetObject("group", adminName);
                isAdmin = grp.IsMember(newUser.ADsPath);
                return true;
            }
            catch
            {
                return false;
            }
        }
        public static bool UserExists(string userName, out bool isAdmin)
        {
            NTAccount nta;
            return UserExists(userName, out isAdmin, out nta);
        }
        /// <summary>
        /// formats a username to NT4 local format having a \ e.g. mydomain\user
        /// </summary>
        /// <param name="userName"></param>
        /// <returns></returns>
        public static string UserAccountFormat(string userName)
        {
            if (string.IsNullOrEmpty(userName)) return null;

            return (userName.IndexOf('\\') < 0) ?
                string.Format("{0}\\{1}", Environment.MachineName, userName) :
                userName;
        }
        public static NTAccount SystemService
        {
            get
            {
                var id = new SecurityIdentifier(
                    WellKnownSidType.LocalSystemSid, null);

                var str = (NTAccount)id.Translate(typeof(NTAccount));
                return str;

            }
        }
        public static NTAccount NetworkService
        {
            get
            {
                var id = (NTAccount)new SecurityIdentifier(
                    WellKnownSidType.NetworkServiceSid, null).Translate(typeof(NTAccount));

                return id;

            }
        }
        private static NTAccount AdminGroup
        {
            get
            {
                var id = new SecurityIdentifier(
                    WellKnownSidType.BuiltinAdministratorsSid, null);

                var str = (NTAccount)id.Translate(typeof(NTAccount));
                return str;

            }
        }
        /// <summary>
        /// To add a service (non-human)user to the local SAM
        /// if the username contains a domain DOMAIN\user it will be created on the domain
        /// The user will have 'user cannot change password' and 'password never expires' 
        /// </summary>
        /// <param name="userName"></param>
        /// <param name="password"></param>
        /// <param name="descriptionUser">a description for the suer</param>
        /// <param name="isAdmin">if false, will be user true makes it admin</param>
        public static NTAccount AddUser(string userName, string password, string descriptionUser, bool isAdmin)
        {
            try
            {
                var idxSlash = userName.IndexOf('\\');
                var machine = idxSlash >= 0 ? userName.Substring(0, idxSlash) : Environment.MachineName;
                if (idxSlash > 0)
                {
                    userName = userName.Substring(idxSlash + 1);
                }
                if (machine == ".")
                {
                    machine = Environment.MachineName;
                }
                var samLocal = (IADsContainer)Marshal.BindToMoniker(string.Format("WinNT://{0},computer", machine));
                
                var newUser = (IADsUser)samLocal.Create("user", userName);
                newUser.SetPassword(password);
                if (!string.IsNullOrEmpty(descriptionUser))
                {
                    newUser.Set("Description", descriptionUser);
                }

                newUser.SetInfo();
                var flags = (EnumAdsUserFlags)newUser.Get("UserFlags");
                newUser.Set("UserFlags", flags |
                        EnumAdsUserFlags.ADS_UF_DONT_EXPIRE_PASSWD |
                        EnumAdsUserFlags.ADS_UF_PASSWD_CANT_CHANGE);
                newUser.SetInfo();
                var sidToReturn = new SecurityIdentifier((byte[])newUser.Get("objectSid"), 0);

                var id = new SecurityIdentifier(
                        isAdmin ? WellKnownSidType.BuiltinAdministratorsSid :
                            WellKnownSidType.BuiltinUsersSid, null);
                var str = (NTAccount)id.Translate(typeof(NTAccount));

                // cut off Builtin\\
                var adminGroupName = str.Value;
                util.WriteToLog(string.Format("making user member of {0}", adminGroupName));
                var sep = adminGroupName.IndexOf('\\');
                if (sep > 0)
                {
                    adminGroupName = adminGroupName.Substring(sep + 1);
                }
               // ActiveDs.IADsGroup usr;
                //ActiveDs.IADsMembers
                var grp = (IADsGroup)samLocal.GetObject("group", adminGroupName);
                var path = newUser.ADsPath; //WinNT://DOMAIN/PC/user
                grp.Add(path);
                grp.SetInfo();
               util.WriteToLog(string.Format("created user {0}", userName));
                return (NTAccount)sidToReturn.Translate(typeof(NTAccount));
            }
            catch (Exception ex)
            {

                // NTAccount acc = new System.Security.Principal.NTAccount();
                util.WriteToLog(ex.Message, util.MessageType.Error);
                throw;

            }
        }

        public static void DeleteUser(this SecurityIdentifier sid)
        {
            var accountName = string.Empty;
            try
            {
                var nta = (NTAccount)sid.Translate(typeof(NTAccount));
                accountName = nta.Value;
            }
            catch
            {
                util.WriteToLog("Cannot delete non existing account " + sid.Value, util.MessageType.Error);
                return;
            }

            try
            {

                var idxSlash = accountName.IndexOf('\\');
                var machine = idxSlash >= 0 ? accountName.Substring(0, idxSlash) : Environment.MachineName;
                if (idxSlash >= 0)
                {
                    accountName = accountName.Substring(idxSlash + 1);
                }
                var localMachine = (IADsContainer)Marshal.BindToMoniker(string.Format("WinNT://{0},computer", machine));
                localMachine.Delete("user", accountName);

                util.WriteToLog("Deleted Account " + accountName);
            }
            catch (Exception e)
            {
                util.WriteToLog("Failed to delete Account " + accountName, util.MessageType.Error);
                throw e;
            }
        }
        /// <summary>
        /// translates e.g. "NT Authority\NetworkService" or "DOMAIN\User" to a valid NTAccount
        /// Note; NTAccount can do this as well, but not for builtin accounts.
        /// </summary>
        /// <param name="account"></param>
        /// <returns></returns>
        public static NTAccount LookupAccount(string account)
        {
            var byteLen = 0;
            var byteDomainLen = 0;
            SidUse peUse;
            var result = LookupAccountName(null, account, null, ref byteLen, null, ref byteDomainLen, out peUse);
            var err = Marshal.GetLastWin32Error();
            if (result == false && byteLen == 0)
            {
                throw new Win32Exception(err);
            }
            var domain = new string('\0', byteDomainLen);
            var sidBytes = new byte[byteLen];
            result = LookupAccountName(null, account, sidBytes, ref byteLen, domain, ref byteDomainLen, out peUse);
            if (result == false)
            {
                throw new Win32Exception(Marshal.GetLastWin32Error());
            }
            return (NTAccount)new SecurityIdentifier(sidBytes, 0).Translate(typeof(NTAccount));
        }
        public static void SetRunasServicePolicy(SecurityIdentifier sid, bool remove)
        {
            string uName = (sid.Translate(typeof(NTAccount))).Value;
            int idxSlash = uName.IndexOf('\\');
            var machine = idxSlash >= 0 ? uName.Substring(0, idxSlash) : Environment.MachineName;

            var objectAttr = new LSA_OBJECT_ATTRIBUTES();
            LSA_UNICODE_STRING systemName = new LSA_UNICODE_STRING(machine),
                requestedRights = new LSA_UNICODE_STRING(SE_SERVICE_LOGON_NAME);

            objectAttr.Length = Marshal.SizeOf(objectAttr);
            const uint access = 0xF0FFF; //tells us that we will modify the policy
            IntPtr handle ;
            int ret = LsaOpenPolicy(ref systemName, ref objectAttr, access, out handle);
            if (ret != 0)
            {
                throw new Win32Exception(LsaNtStatusToWinError(ret));
            }

           var bt = new byte[sid.BinaryLength];
            sid.GetBinaryForm(bt, 0);
            ret = !remove ? LsaAddAccountRights(handle, bt, new[] { requestedRights }, 1) : 
                LsaRemoveAccountRights(handle, bt, false, new[] { requestedRights }, 1);

            if (ret != 0)
                throw new Win32Exception(LsaNtStatusToWinError(ret));
            SafeLsaHandle.LsaClose(handle);
        }
        internal static void EnsureACLReadExecute(string file, bool isDir, SecurityIdentifier sid)
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

                bool found = false;

                var requestedSec = isDir ? new FileSystemAccessRule(identifier, FileSystemRights.FullControl, InheritanceFlags.ContainerInherit, PropagationFlags.InheritOnly, AccessControlType.Allow)
                    : new FileSystemAccessRule(identifier, FileSystemRights.ReadAndExecute, AccessControlType.Allow);
                foreach (AccessRule authRule in f.GetAccessRules(true, true, typeof(SecurityIdentifier)))
                {
                    if (authRule.IdentityReference == requestedSec.IdentityReference && authRule.AccessControlType == AccessControlType.Allow)
                    {
                        found = true;
                        break;
                    }
                }
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

        /// <summary>
        /// 
        /// </summary>
        /// <param name="install">true run svcutil with install, false = remove</param>
        /// <param name="serviceAssembly">the full path to executing assembly</param>
        /// <param name="additionalParams"></param>
        internal static void ConfigureManagedService(bool install, string serviceAssembly, Dictionary<string, string> additionalParams)
        {
            string sPath = Path.Combine(util.AppPath, serviceAssembly);
            var sb = new StringBuilder();
            if (!install)
            {
                sb.Append("-u ");
            }
            if (additionalParams != null)
            {
                foreach (string key in additionalParams.Keys)
                {
                    sb.AppendFormat("/{0}={1} ", key, additionalParams[key]);
                }
            }
            sb.Append(' ');
            sb.Append('"');
            sb.Append(sPath);
            sb.Append('"');
            //MessageBox.Show(string.Format("{0}+{1}", GetInstallUtil(), sPath));
            string sutil = util.GetInstallUtil();
            string getSb = sb.ToString();
            util.WriteToLog(string.Format("running {0} with {1}", sutil, getSb));
            var proc = Process.Start(sutil, sb.ToString());
            bool result = proc.Start();
            result = proc.WaitForExit(5000);

        }  
   
        public static void SetRunasServicePolicy(this NTAccount currentUser, bool remove)
        {
            var translated = (SecurityIdentifier)currentUser.Translate(typeof(SecurityIdentifier));
            // we don't want this for Network Service or other built in accounts
            // only for users
            if (translated.IsAccountSid())
            {
                SetRunasServicePolicy(translated, remove);
            }
        }

        public static void ChangePassword(this NTAccount nta, string p)
        {
            string userName = nta.Value;
            string machine = null;
            int idxSlash = userName.IndexOf('\\');
            if (idxSlash > 0)
            {
                machine = userName.Substring(0, idxSlash);
                userName = userName.Substring(idxSlash + 1);
            }
            else
            {
                machine = Environment.MachineName;
            }
            
            var localMachine = (IADsContainer)Marshal.BindToMoniker(string.Format("WinNT://{0},computer", machine));
            var newUser = (IADsUser)localMachine.GetObject("user", userName);
            newUser.SetPassword(p);
        }

        
    }
}