using System;
using System.Collections.Specialized;
using System.Configuration;
using System.Web;

namespace ispsession.io
{
    internal static class ConfigHelper
    {
        internal static T GetAppValue<T>(this NameValueCollection settings, string key)
        {
            var temp = settings[key];
            try
            {
                if (typeof(T) == typeof(Guid))
                {
                    Guid g;
                    object obj;
                    if (Guid.TryParse(temp, out g))
                    {
                        //box the muke
                        obj = g;
                        return (T)obj;
                    }
                    obj = Guid.Empty;
                    return (T)obj;
                }
                else
                {
                    return (T)(Convert.ChangeType(temp, typeof(T)));
                }

            }
            catch
            {
                throw new HttpException(String.Format("{0} must be specified as true or false", key));
            }

        }
        internal static T GetAppValue<T>(this NameValueCollection settings, string key, T defaultvalue)
        {
            var temp = settings[key];
            if (string.IsNullOrWhiteSpace(temp))
            {
                return defaultvalue;
            }

            return GetAppValue<T>(settings, key);

        }
    }
}
