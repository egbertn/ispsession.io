using Microsoft.AspNetCore.Builder;
//using Microsoft.AspNetCore.DataProtection;
using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Options;
using System;
using System.Threading;
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
        private static bool Checked;
        private readonly IISPSessionStore _sessionStore;
        private static int _instanceCount;
        private static readonly object locker = new object();
        private static bool initDone;

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
            // do the cookie stuff just once
            Func<bool> initialized =() => false;
            Func<ISPSession, bool> tryEstablishSession = (i) => (new ISPSessionIDManager(context, text2, _options)).TryEstablishSession(i);

#if !Demo
            if (initDone == false)
            {
                lock (locker)
                {
                    initDone = true;
                    string license = _options.Lic;
                    var lic = _options.LicKeyCore;
                    if (string.IsNullOrEmpty(license))
                    {
                        throw new Exception("LicKeyCore in section ispsession.io is missing in appsettings.json");
                    }
                    if (string.IsNullOrEmpty(lic))
                    {
                        throw new Exception(
                            "Lic in section ispsession.io is missing in appsettings.json");
                    }
                    Checked = Helpers.LicentieCheck(Helpers.HexToBytes(lic), license);
                }
            }
            if (!Checked)
            {
                await context.Response.WriteAsync("The ispsession.io Module should be licensed. Please contact ADC Cure for an updated license at information@adccure.nl");
            }
            if (Interlocked.Increment(ref _instanceCount) > Helpers.Maxinstances)
            {
                Thread.Sleep(500 * (_instanceCount - Helpers.Maxinstances));
                NativeMethods.OutputDebugStringW(string.Format("LICENSE ERROR max = {0} requested ={1} \r\n", Helpers.Maxinstances, _instanceCount));                
            }
#else
            var exp = double.Parse(Helpers.GetMetaData("at"));
            if (DateTime.Today > NativeMethods.FromOADate(exp))
            {
                await context.Response.WriteAsync(Helpers.MessageString);
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
#if !Demo
            Interlocked.Decrement(ref _instanceCount);            
#endif      

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
