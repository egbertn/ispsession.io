using Microsoft.AspNetCore.Builder;
//using Microsoft.AspNetCore.DataProtection;
using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Options;
using System;
using System.Threading.Tasks;
using ispsession.io.core.Interfaces;
using System.Threading;
using System.Diagnostics;
namespace ispsession.io.core
{
    internal sealed class ISPSessionStateItemCollection
    {
        internal PersistMetaData Meta;
        internal ISPSessionStateItemCollection2 Items;
    }


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

            var text2 = await _manager.GetSessionIDAsync(context);

            if (text2 == null )
            {
                text2 = _manager.CreateSessionID(context);
                isNewSessionKey = (text2 == null);  //when liquid, it is not new
            }

            // do the cookie stuff just once
            Func<bool> initialized =() => false;
            Task<bool> tryEstablishSession(ISPSession i) => (new ISPSessionIDManager(context, text2,
                _options)).TryEstablishSession(i);

            Interlocked.Increment(ref _instanceCount);

            var sessionFeature = new ISPSessionFeature
            {
                Session = this._sessionStore.Create(text2, tryEstablishSession, isNewSessionKey, _options),

            };
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
            Interlocked.Decrement(ref _instanceCount);


        }

    }
    public static partial class Extensions
    {

        /// <summary>
        /// returns the Session feature from Context.
        /// </summary>
        public static IISPSession Session(this HttpContext context)
        {
            var features = context.Features.Get<IISPSEssionFeature>();
            if (features != null)
            {
                return ((ISPSessionFeature)features).Session;
            }
            return null;
        }
        public static IApplicationBuilder UseISPSession(this IApplicationBuilder app)
        {
            return app.UseMiddleware<ISPSessionMiddleWare>();
        }
    }

}
