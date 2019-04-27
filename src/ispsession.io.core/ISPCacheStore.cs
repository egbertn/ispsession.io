using ispsession.io.core.Interfaces;
using System;
using System.Threading.Tasks;

namespace ispsession.io.core
{
    public class ISPCacheStore: IISPCacheStore
    {
        public IApplicationCache Create(CacheAppSettings settings, Func<ApplicationCache, Task<bool>> tryEstablishSession)
        {
            var appCache = new ApplicationCache(settings, tryEstablishSession);

            return appCache;
        }

       
    }
}
