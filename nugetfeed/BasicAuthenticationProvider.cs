using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.Principal;
using System.Text;
using System.Web;

namespace SimpleBasicAuthentication
{
    public class BasicAuthenticationProvider
    {
        private const string C_USERCREDENTIALSLIST = "UserCredentialsList";

        public static bool Authenticate(HttpContext context, bool checkForSsl)
        {
           var list = context.Cache[C_USERCREDENTIALSLIST] as IEnumerable<string>;
            if (list == null)
            {
                list = PopulateUserCredentials(context);
            }
            bool result;
            if (checkForSsl)
            {
                if (!context.Request.IsSecureConnection)
                {
                    result = false;
                    return result;
                }
            }
            var headers = context.Request?.Headers;
            
            if (headers == null)
            {
                return false;
            }
            var  keys = headers.AllKeys; 
            
            if (!keys.Contains("Authorization"))
            {
                result = false;
            }
            else
            {
                string authHeader = headers["Authorization"];
                IPrincipal user;
                if (TryGetPrincipal(list, authHeader, out user))
                {
                    HttpContext.Current.User = user;
                    result = true;
                }
                else
                {
                    result = false;
                }
            }
            return result;
        }

        private static IEnumerable<string> PopulateUserCredentials(HttpContext context)
        {
            var server = context.Server;

            var list = File.ReadAllLines(server.MapPath("~/App_Data/UserCredentials.txt"), Encoding.UTF8);            
            context.Cache[C_USERCREDENTIALSLIST] = list;
            return list;
        }

        private static bool TryGetPrincipal(IEnumerable<string> userCredentials, string authHeader, out IPrincipal principal)
        {
            var array = ParseAuthHeader(authHeader);
            bool result;
            if (array != null && TryGetPrincipal(userCredentials, array, out principal))
            {
                result = true;
            }
            else
            {
                principal = null;
                result = false;
            }
            return result;
        }

        private static string[] ParseAuthHeader(string authHeader)
        {            
            if (authHeader == null || authHeader.Length == 0 || !authHeader.StartsWith("Basic", StringComparison.CurrentCultureIgnoreCase))
            {
                return null;
            }
            else
            {
                string s = authHeader.Substring(6);
               var array = Encoding.ASCII.GetString(Convert.FromBase64String(s)).Split(':');
                if (array.Length != 2 || string.IsNullOrEmpty(array[0]) || string.IsNullOrEmpty(array[1]))
                {
                    return null;
                }
                else
                {
                    return array;
                }
            }
        }

        private static bool TryGetPrincipal(IEnumerable<string> userCredentials, string[] creds, out IPrincipal principal)
        {
            bool result;
            foreach (string current in userCredentials)
            {
                string[] array = current.Split(';');
                string text = creds[0];
                string a = creds[1];
                if (string.Compare(text, array[0], StringComparison.CurrentCultureIgnoreCase) == 0 && a == array[1])
                {
                    var roles = array[2].Split('|');
                    principal = new GenericPrincipal(new GenericIdentity(array[0]), roles);
                    result = true;
                    return result;
                }
            }
            principal = null;
            result = false;
            return result;
        }
    }
}
