using ispsession.io.core.Interfaces;
using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Options;
using System;
using System.Threading.Tasks;

namespace ispsession.io.core
{
    public sealed class ISPCacheMiddleWare
    {
        private readonly RequestDelegate _next;
        private readonly CacheAppSettings _options;
        private readonly IISPCacheStore _CacheStore;
        private ISPCacheManager _manager;
        public ISPCacheMiddleWare(RequestDelegate next, IISPCacheStore _CacheStore, IOptions<CacheAppSettings> options)
        {
            _next = next;
            this._options = options.Value;
            this._CacheStore = _CacheStore;
            _manager = new ISPCacheManager(_options);
        }
        public async Task Invoke(HttpContext context)
        {

            // do Application initialisation just once. Otherwise, 
            // when css extension or others are loaded, it will be reloaded again
            Func<bool> initialized = () => false;
            Task<bool> tryEstablishApplication(ApplicationCache i) => (new ISPCacheManager(context, _options)).TryEstablishSession(i);


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
                    Checked = StreamManager.LicentieCheck(StreamManager.HexToBytes(lic), license);
                }
            }
            if (!Checked)
            {
                await context.Response.WriteAsync(StreamManager.MessageString2);
            }
            if (Interlocked.Increment(ref _instanceCount) > StreamManager.Maxinstances)
            {
                Thread.Sleep(500 * (_instanceCount - StreamManager.Maxinstances));
                NativeMethods.OutputDebugStringW(string.Format(StreamManager.MessageString3, StreamManager.Maxinstances, _instanceCount));                
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
                Application = this._CacheStore.Create(_options, tryEstablishApplication)
            };
            var database = CSessionDL.GetDatabase(_options);
            context.Features.Set<ICacheFeature>(cacheFeature);
            
            await _next(context);
            //remove feature again
            context.Features.Set<ICacheFeature>(null);
          

            if (cacheFeature.Application != null)
            {
                try
                {
                    await cacheFeature.Application.CommitAsync();
                    var keyMan = (IKeySerializer)cacheFeature.Application;
                    await CSessionDL.ApplicationSave(database, _options.AppKey, keyMan, TimeSpan.FromMilliseconds(0));

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
            return app.UseMiddleware<ISPCacheMiddleWare>();
        }
    }
}
