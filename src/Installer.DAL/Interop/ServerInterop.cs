using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace ADCCure.Configurator.DAL.Interop
{

    public enum NetServerTypeEnum : uint
    {
            SV_TYPE_WORKSTATION = 0x00000001
    ,
            SV_TYPE_SERVER = 0x00000002
                ,
            SV_TYPE_SQLSERVER = 0x00000004
                ,
            SV_TYPE_DOMAIN_CTRL = 0x00000008
                ,
            SV_TYPE_DOMAIN_BAKCTRL = 0x00000010
                ,
            SV_TYPE_TIME_SOURCE = 0x00000020
                ,
            SV_TYPE_AFP = 0x00000040
                ,
            SV_TYPE_NOVELL = 0x00000080
                ,
            SV_TYPE_DOMAIN_MEMBER = 0x00000100
                ,
            SV_TYPE_PRINTQ_SERVER = 0x00000200
                ,
            SV_TYPE_DIALIN_SERVER = 0x00000400
                ,
            SV_TYPE_XENIX_SERVER = 0x00000800
                ,
            SV_TYPE_SERVER_UNIX = 0x00000800
                ,
            SV_TYPE_NT = 0x00001000
                ,
            SV_TYPE_WFW = 0x00002000
                ,
            SV_TYPE_SERVER_MFPN = 0x00004000
                ,
            SV_TYPE_SERVER_NT = 0x00008000
                ,
            SV_TYPE_POTENTIAL_BROWSER = 0x00010000
                ,
            SV_TYPE_BACKUP_BROWSER = 0x00020000
                ,
            SV_TYPE_MASTER_BROWSER = 0x00040000
                ,
            SV_TYPE_DOMAIN_MASTER = 0x00080000
                ,
            SV_TYPE_SERVER_OSF = 0x00100000
                ,
            SV_TYPE_SERVER_VMS = 0x00200000
                ,
            SV_TYPE_WINDOWS = 0x00400000  /* Windows95 and above */
                ,
            SV_TYPE_DFS = 0x00800000  /* Root of a DFS tree */
                ,
            SV_TYPE_CLUSTER_NT = 0x01000000  /* NT Cluster */
                ,
            SV_TYPE_TERMINALSERVER = 0x02000000  /* Terminal Server(Hydra) */
                ,
            SV_TYPE_CLUSTER_VS_NT = 0x04000000  /* NT Cluster Virtual Server Name */
                ,
            SV_TYPE_DCE = 0x10000000  /* IBM DSS (Directory and Security Services) or equivalent */
                ,
            SV_TYPE_ALTERNATE_XPORT = 0x20000000  /* return list for alternate transport */
                ,
            SV_TYPE_LOCAL_LIST_ONLY = 0x40000000  /* Return local list only */
                ,
            SV_TYPE_DOMAIN_ENUM = 0x80000000
                , SV_TYPE_ALL = 0xFFFFFFFF  /* handy for NetServerEnum2 */

        }

        [StructLayout(LayoutKind.Sequential,CharSet = CharSet.Unicode)]
        internal struct ServerInfo100
        {
            public int sv100_platform_id; //500 =PLATFORM_ID_NT
            [MarshalAs(UnmanagedType.LPWStr)]
            public string sv100_name;
        }
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        internal struct ServerInfo101
        {
            public int sv101_platform_id;
            [MarshalAs(UnmanagedType.LPWStr)]
            public string sv101_name;
            public int sv101_version_major;
            public int sv101_version_minor;
            public int sv101_type;
            [MarshalAs(UnmanagedType.LPWStr)]
            public string sv101_comment;

        }
       
        public static class Interop
        {
            internal const int MAX_PREFERRED_LENGTH = -1;
            internal const int ERROR_NO_BROWSER_SERVERS_FOUND = 6118;
            internal const int ERROR_MORE_DATA = 234;
            internal const int ERROR_NOT_SUPPORTED = 50;
            internal const int NERR_RemoteErr = 2127;
            internal const int NERR_ServerNotStarted = 2114;
            internal const int NERR_ServiceNotInstalled = 2184;
            internal const int NERR_WkstaNotStarted = 2188;
            internal const int NERR_Success = 0;
            [DllImport("Netapi32.dll", CharSet = CharSet.Unicode, SetLastError = false, ExactSpelling = true)]
            internal extern static int NetServerEnum(
                [In, MarshalAs(UnmanagedType.LPWStr)]
            string servername, //null
                int level,//101
                [Out] 
            out IntPtr buf,
                   int prefmaxlen, //-1
                [Out]  out int entriesread,
                [Out] out int totalentries,
                NetServerTypeEnum servertype,//setup.NetServerEnum.SV_TYPE_SERVER
                [In, Optional, MarshalAs(UnmanagedType.LPWStr)] string domain,
                ref IntPtr resumehandle);
            [DllImport("Netapi32.dll", SetLastError = false)]
            internal extern static int NetApiBufferFree([In] IntPtr buf);
       
            
            [DllImport("Advapi32.dll", SetLastError = true, ExactSpelling = true)]
            static extern void MapGenericMask(ref uint AccessMask, [In] ref GENERIC_MAPPING GenericMapping);
            [DllImport("Aclui.dll", SetLastError = true, ExactSpelling = true)]
            static internal extern IntPtr CreateSecurityPage(
                ISecurityInformation psi);
            [DllImport("Aclui.dll", SetLastError = true, ExactSpelling = true)]
            [return: MarshalAs(UnmanagedType.Bool)]
            static extern public bool EditSecurity(IntPtr hwndOwner, ISecurityInformation psi);

        }
        /*
         * 
    #define OWNER_SECURITY_INFORMATION       (0x00000001L)
    #define GROUP_SECURITY_INFORMATION       (0x00000002L)
    #define DACL_SECURITY_INFORMATION        (0x00000004L)
    #define SACL_SECURITY_INFORMATION        (0x00000008L)
    #define LABEL_SECURITY_INFORMATION       (0x00000010L)*/
        [Flags]
        public enum SECURITY_INFORMATION : int
        {
            OWNER_SECURITY_INFORMATION = 1,
            GROUP_SECURITY_INFORMATION = 2,
            DACL_SECURITY_INFORMATION = 4,
            SACL_SECURITY_INFORMATION = 8,
            LABEL_SECURITY_INFORMATION = 0x10
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct GENERIC_MAPPING
        {
            int GenericRead;
            int GenericWrite;
            int GenericExecute;
            int GenericAll;
        }

        [Flags]
        public enum SI_OBJECT_INFO_FLAGS : int
        {
            SI_EDIT_PERMS = 0x0, // always implied
            SI_EDIT_OWNER = 0x00000001,
            SI_EDIT_AUDITS = 0x00000002,
            SI_CONTAINER = 0x00000004,
            SI_READONLY = 0x00000008,
            SI_ADVANCED = 0x00000010,
            SI_RESET = 0x00000020,//equals to SI_RESET_DACL|SI_RESET_SACL|SI_RESET_OWNER
            SI_OWNER_READONLY = 0x00000040,
            SI_EDIT_PROPERTIES = 0x00000080,
            SI_OWNER_RECURSE = 0x00000100,
            SI_NO_ACL_PROTECT = 0x00000200,
            SI_NO_TREE_APPLY = 0x00000400,
            SI_PAGE_TITLE = 0x00000800,
            SI_SERVER_IS_DC = 0x00001000,
            SI_RESET_DACL_TREE = 0x00004000,
            SI_RESET_SACL_TREE = 0x00008000,
            SI_OBJECT_GUID = 0x00010000,
            SI_EDIT_EFFECTIVE = 0x00020000,
            SI_RESET_DACL = 0x00040000,
            SI_RESET_SACL = 0x00080000,
            SI_RESET_OWNER = 0x00100000,
            SI_NO_ADDITIONAL_PERMISSION = 0x00200000,
            SI_VIEW_ONLY = 0x00400000,
            SI_PERMS_ELEVATION_REQUIRED = 0x01000000,
            SI_AUDITS_ELEVATION_REQUIRED = 0x02000000,
            SI_OWNER_ELEVATION_REQUIRED = 0x04000000,
            SI_MAY_WRITE = 0x10000000, //not sure if user can write permission
            SI_EDIT_ALL = (SI_EDIT_PERMS | SI_EDIT_OWNER | SI_EDIT_AUDITS)
        }


        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        public struct SI_OBJECT_INFO
        {
            public int dwFlags;
            public IntPtr hInstance;          // resources (e.g. strings) reside here
            [MarshalAs(UnmanagedType.LPStruct)]
            public string pszServerName;      // must be present
            [MarshalAs(UnmanagedType.LPStruct)]
            public string pszObjectName;      // must be present
            [MarshalAs(UnmanagedType.LPStruct)]
            public string pszPageTitle;       // only valid if SI_PAGE_TITLE is set
            public Guid guidObjectType;     // only valid if SI_OBJECT_GUID is set
        }
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        public struct SI_INHERITED_TYPE
        {
            IntPtr pguid;
            int dwFlags;
            [MarshalAs(UnmanagedType.LPWStr)]
            string pszName;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct SI_ACCESS
        {
            IntPtr pguid;
            int mask;
            [MarshalAs(UnmanagedType.LPWStr)] // may be resource ID
            string pszName;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct ACE_HEADER
        {
            public byte AceType;
            public byte AceFlags;
            public short AceSize;
        }
        public enum SI_PAGE_TYPE : int
        {
            SI_PAGE_PERM = 0,
            SI_PAGE_ADVPERM,
            SI_PAGE_AUDIT,
            SI_PAGE_OWNER,
            SI_PAGE_EFFECTIVE,
            SI_PAGE_TAKEOWNERSHIP
        };

        [ComImport(), Guid("965FC360-16FF-11d0-91CB-00AA00BBB723"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
        public interface ISecurityInformation
        {
            [PreserveSig]
            int GetObjectInformation(out SI_OBJECT_INFO nfo);
            // AccessRights GetAccessRights(Guid type, ObjectInfoFlags flags);
            [PreserveSig]
            int GetSecurity(SECURITY_INFORMATION info, out IntPtr SecurityDescriptor /*LocalFree*/, bool wantDefault);
            [PreserveSig]
            int SetSecurity(SECURITY_INFORMATION info, [In] IntPtr sd);

            [PreserveSig]
            int GetAccessRights([In]   ref Guid pguidObjectType,
                               int dwFlags,
               [MarshalAs(UnmanagedType.LPStruct, SizeParamIndex = 3)]
            out SI_ACCESS[] ppAccess,
                   out int pcAccesses, out int Rights);
            [PreserveSig]
            int MapGeneric([In] ref Guid guidObjectType, // must be GUID_NULL 
                /*            A pointer to the AceFlags member of the ACE_HEADER structure from the ACE whose access mask is being mapped. */
                [In] ref ACE_HEADER pAceFlags, ref int accessMask);

            [PreserveSig]
            int GetInheritTypes(
                [MarshalAs(UnmanagedType.LPStruct, SizeParamIndex = 1)]
            SI_INHERITED_TYPE[] inheritedtypes, out int count);
            [PreserveSig]
            int PropertySheetPageCallback(IntPtr hwnd, int msg, SI_PAGE_TYPE pt);

        };
    }

