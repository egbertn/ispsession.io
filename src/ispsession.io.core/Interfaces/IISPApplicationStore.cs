using System.Threading.Tasks;

namespace ispsession.io.core.Interfaces
{
    public interface IISPCacheStore
    {
       
        IApplicationCache Create(CacheAppSettings setting);
    }
}
