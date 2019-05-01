using ispsession.io.core.Interfaces;
using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Options;
using System;
using System.Diagnostics;
using System.Linq;
#if !Demo
using System.Threading;
using Microsoft.AspNetCore.Builder;
#endif
using System.Threading.Tasks;

namespace ispsession.io.core
{
    public sealed class ISPCacheMiddleWare
    {
        private readonly RequestDelegate _next;
        private readonly CacheAppSettings _options;
        private readonly IISPCacheStore _cacheStore;
        
        private static int _instanceCount;
        private static readonly object locker = new object();
#if !Demo
        private static bool initDone;
        private static bool Checked;
#endif
        public ISPCacheMiddleWare(RequestDelegate next, IISPCacheStore _CacheStore, IOptions<CacheAppSettings> options)
        {
            _next = next;
            this._options = options.Value;
            this._cacheStore = _CacheStore;
         
        }
        public async Task Invoke(HttpContext context)
        {

            // do Application initialisation just once. Otherwise, 
            // when css extension or others are loaded, it will be reloaded again
            Func<bool> initialized = () => false;


#if !Demo
            if (initDone == false)
            {
                lock (locker)
                {
                    initDone = true;
                    string license = _options.License;
                    var lic = _options.LicenseDomains;
                    if (string.IsNullOrEmpty(license))
                    {
                        throw new Exception("LicenseDomains in section ispcache.io is missing in appsettings.json");
                    }
                    if (lic == null || !lic.Any())
                    {
                        throw new Exception(
                            "LicenseDomains in section ispcache.io is missing in appsettings.json");
                    }
                    Checked = StreamManager.LicentieCheck(StreamManager.HexToBytes(string.Join("\r\n", lic)), license);
                }
            }
            if (!Checked)
            {
                await context.Response.WriteAsync(StreamManager.MessageString2);
            }
            if (Interlocked.Increment(ref _instanceCount) > StreamManager.Maxinstances)
            {
                Thread.Sleep(500 * (_instanceCount - StreamManager.Maxinstances));
                Trace.TraceInformation(StreamManager.MessageString3, StreamManager.Maxinstances, _instanceCount);                
            }
#else
            var exp = double.Parse(StreamManager.GetMetaData("at"));
            if (DateTime.Today > DateTime.FromOADate(exp))
            {
                await context.Response.WriteAsync(StreamManager.MessageString);
            }

#endif

            var cacheFeature = new ISPCacheFeature
            {
                Application = this._cacheStore.Create(_options)
            };

            context.Features.Set<ICacheFeature>(cacheFeature);

            await _next(context);
            //remove feature again
            context.Features.Set<ICacheFeature>(null);


            if (cacheFeature.Application != null)
            {
                try
                {
                    await cacheFeature.Application.CommitAsync();

                }
                catch (Exception ex)
                {
                    Diagnostics.TraceError("Application.CommitAsync failed with {0}", ex);
                    //this._logger.ErrorClosingTheSession(var_9_257);
                }
            }
#if !Demo
            Interlocked.Decrement(ref _instanceCount);            
#endif      

        }

    }
    public static partial class Extensions
    {
        /// <summary>
        /// returns the Application feature from Context.
        /// </summary>
        public static IApplicationCache ApplicationCache(this HttpContext context)
        {
            var features = context.Features.Get<ICacheFeature>();
            if (features != null)
            {
                return ((ISPCacheFeature)features).Application;
            }
            return null;
        }
        public static IApplicationBuilder UseISPCache(this IApplicationBuilder app)
        {
            return app.UseMiddleware<ISPCacheMiddleWare>();
        }
    }
}
