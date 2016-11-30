using System;
using System.Runtime.InteropServices;
using ispsession.Configurator.DAL.Interop;

namespace ispsession.io.setup.util
{
    public sealed class SecurityProperties : ISecurityInformation
    {
        public SecurityProperties()
        {
           //var h= Interop.CreateSecurityPage(this);
                if (!Interop.EditSecurity(IntPtr.Zero, this))
                {
                    int hr = Marshal.GetLastWin32Error();
                    Marshal.ThrowExceptionForHR(hr);
                }
                
            
        }

        int ISecurityInformation.GetObjectInformation(out SI_OBJECT_INFO nfo)
        {
          //  var bytes =System.IO.File.GetAccessControl(@"C:\\temp\ADCResourceDictionary.log").GetSecurityDescriptorBinaryForm();
            nfo = new SI_OBJECT_INFO();

            nfo.dwFlags=(int) SI_OBJECT_INFO_FLAGS.SI_EDIT_PERMS;
            nfo.pszPageTitle = "hello world";
           
            //nfo.pszServerName = "BETELGEUSE";

            //nfo.pszPageTitle = "edit file perm"; //SI_PAGE_TITLE
           // nfo.hInstance = 0;
            return 0;
            
        }

        int ISecurityInformation.GetSecurity(SECURITY_INFORMATION info, out IntPtr SecurityDescriptor, bool wantDefault)
        {
           //RawSecurityDescriptor g;
            //NtSecurityInterop.allo
            SecurityDescriptor = IntPtr.Zero;
            return 0;
        }

        int ISecurityInformation.SetSecurity(SECURITY_INFORMATION info, IntPtr sd)
        {
            return 0;
        }

        int ISecurityInformation.GetAccessRights(ref Guid pguidObjectType, int dwFlags, out SI_ACCESS[] ppAccess, out int pcAccesses, out int access)
        {
            access = 0;
            pcAccesses = 0;
            ppAccess = null;
    return 0;
        }

        int ISecurityInformation.MapGeneric(ref Guid guidObjectType, ref ACE_HEADER pAceFlags, ref int accessMask)
        {
            return 0;
        }

        int ISecurityInformation.GetInheritTypes(SI_INHERITED_TYPE[] inheritedtypes, out int count)
        {
            count = 0;
            return 0 ;
        }

        int ISecurityInformation.PropertySheetPageCallback(IntPtr hwnd, int msg, SI_PAGE_TYPE pt)
        {
            if (pt == SI_PAGE_TYPE.SI_PAGE_PERM)
            {
            }
            return 0;
            //throw new NotImplementedException();
        }
    }
}
