using Microsoft.AspNetCore.Http;
using System.Text;
using System.Threading.Tasks;

namespace ispsession.io.core
{
    /// <summary>
    /// required to replace the compatible session cookie for ISP Session both managed and unmanaged
    /// </summary>
    public sealed class ISPCacheManager
    {

        private readonly HttpContext _context;
        private readonly string _id;
        
        private readonly CacheAppSettings _settings;
        
        public ISPCacheManager(CacheAppSettings settings)
        {
            _settings = settings;
        }
       
        /// <summary>
        /// some magic, required to lazy initialize a session from Redis
        /// the problem exists that MVC has no page extensions
        /// So we don't know when or not 'to Application'. The only reliable detection would be at Session set/get item/delete/clear etc
        /// which means, somebody needs us now. Other IIS requests, such as .js files or css thus will ignore the state.
        /// </summary>
        internal async Task< bool> TryEstablishCache(ApplicationCache i)
        {
           await i.LoadAsync();            
        
           return true;
        }
        public ISPCacheManager(HttpContext context, CacheAppSettings settings)
        {
            _settings = settings;
            _context = context;
        }
    }
}