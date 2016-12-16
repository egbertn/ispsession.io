using Microsoft.AspNetCore.Builder;
//using Microsoft.AspNetCore.DataProtection;
using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Options;
using System;
using System.Threading.Tasks;


namespace ispsession.io
{
    internal sealed class ISPSessionStateItemCollection
    {
        internal PersistMetaData Meta;
        internal ISPSessionStateItemCollection2 Items;
    }
    //internal static class CookieProtection
    //{
    //    internal static string Protect(IDataProtector protector, string data)
    //    {
    //        if (protector == null)
    //        {
    //            throw new ArgumentNullException("protector");
    //        }
    //        if (string.IsNullOrEmpty(data))
    //        {
    //            return data;
    //        }
    //        byte[] bytes = Encoding.UTF8.GetBytes(data);
    //        return Convert.ToBase64String(protector.Protect(bytes)).TrimEnd('=');
    //    }

    //    internal static string Unprotect(IDataProtector protector, string protectedText)
    //    {
    //        string result;
    //        try
    //        {
    //            if (string.IsNullOrEmpty(protectedText))
    //            {
    //                result = string.Empty;
    //            }
    //            else
    //            {
    //                byte[] array = Convert.FromBase64String(Pad(protectedText));
    //                if (array == null)
    //                {
    //                    result = string.Empty;
    //                }
    //                else
    //                {
    //                    byte[] array2 = protector.Unprotect(array);
    //                    if (array2 == null)
    //                    {
    //                        result = string.Empty;
    //                    }
    //                    else
    //                    {
    //                        result = Encoding.UTF8.GetString(array2);
    //                    }
    //                }
    //            }
    //        }
    //        catch (Exception exception)
    //        {
    //           // logger.ErrorUnprotectingSessionCookie(exception);
    //            result = string.Empty;
    //        }
    //        return result;
    //    }

    //    private static string Pad(string text)
    //    {
    //        int num = 3 - (text.Length + 3) % 4;
    //        if (num == 0)
    //        {
    //            return text;
    //        }
    //        return text + new string('=', num);
    //    }
    //}
   
    public class ISPSessionMiddleWare
    {    
        private readonly ISPSessionIDManager _manager;
        private readonly RequestDelegate _next;
        private readonly SessionAppSettings _options;
        private bool Checked;
        private readonly IISPSessionStore _sessionStore;
        private static readonly Func<ISPSession, bool> ReturnTrue = (i) => true;

        //  private readonly IDataProtector _dataProtector;
        public ISPSessionMiddleWare(RequestDelegate next, IISPSessionStore sessionStore, IOptions<SessionAppSettings> options)
        {
            _next = next;
            this._options = options.Value;
            this._sessionStore = sessionStore;
            _manager = new ISPSessionIDManager(_options);
        }
        public async Task Invoke(HttpContext context)
        {

            var isNewSessionKey = false;
             
            var text2 = _manager.GetSessionID(context);
            if (text2 == null || _options.Liquid)
            {
                text2 = _manager.CreateSessionID(context);
                isNewSessionKey = true;              
            }
            Func<ISPSession, bool> tryEstablishSession = (i) => (new ISPSessionIDManager(context, text2, _options)).TryEstablishSession(i);

#if !Demo
            string license = _appSettings.Lic;
            var lic = _appSettings.LicKey;
            if (string.IsNullOrEmpty(license))
            {
                throw new Exception("Ccession.LIC appsetting is missing in web.config");
            }
            if (string.IsNullOrEmpty(lic))
            {
                throw new Exception(
                    "ISP Session requires a Appsetting such as <add key=\"License\" value=\"0245456556560418A91B161F23534007\" />");
            }
                    Checked = Helpers.LicentieCheck(Helpers.HexToBytes(lic), license);
            if (!Checked)
            {
                await context.Response.WriteAsync("The ispsession.io Module should be licensed. Please contact ADC Cure for an updated license at information@adccure.nl");
            }
#else
            var exp = double.Parse(Helpers.GetMetaData("at"));
            if (DateTime.Today > NativeMethods.FromOADate(exp))
            {
                await context.Response.WriteAsync("The ispsession.io Module expired! We would welcome your order at <a href=\"http://ispsession.io/?page=order\">order here</a>");
            }

#endif
            var sessionFeature = new ISPSessionFeature();
            
            sessionFeature.Session = this._sessionStore.Create(text2, tryEstablishSession, isNewSessionKey, _options);
            context.Features.Set<IISPSEssionFeature>(sessionFeature);
            context.Features.Set<Microsoft.AspNetCore.Http.Features.ISessionFeature>(sessionFeature);//make HttpContext.Session happy
            await _next(context);
            //remove feature again
            context.Features.Set<IISPSEssionFeature>(null);
            context.Features.Set<Microsoft.AspNetCore.Http.Features.ISessionFeature>(null);
  
            if (sessionFeature.Session != null)
            {
                try
                {
                   await sessionFeature.Session.CommitAsync();
                }
                catch (Exception ex)
                {
                    //this._logger.ErrorClosingTheSession(var_9_257);
                }
            }
                    
        }

    }
    public static class BuilderExtensions
    {
        public static IApplicationBuilder UseISPSession(this IApplicationBuilder app)
        {
            return app.UseMiddleware<ISPSessionMiddleWare>();
        }
    }

}
