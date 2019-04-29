using ispsession.io.core.Interfaces;
using System;
using System.Threading.Tasks;

namespace ispsession.io.core
{
    public class ISPCacheStore: IISPCacheStore
    {
        public IApplicationCache Create(CacheAppSettings settings)
        {
            var appCache = new ApplicationCache(settings);

            return appCache;
        }

       
    }
}
