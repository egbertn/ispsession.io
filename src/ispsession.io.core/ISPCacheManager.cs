using Microsoft.AspNetCore.Http;
using System.Text;
using System.Threading.Tasks;

namespace ispsession.io
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
        private bool _shouldEstablishSession;
        /// <summary>
        /// some magic, required to lazy initialize a session from Redis
        /// the problem exists that MVC has no page extensions
        /// So we don't know when or not to Session. The only reliable detection would be at Session set/get item/delete/clear etc
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
          
            context.Response.OnStarting(OnStartingCallback, this);
        }
        private static Task OnStartingCallback(object state)
        {
           
            return Task.FromResult(0);
        }
     
        

       

        private static string GuidToHex(byte[] bytes)
        {
            var sb = new StringBuilder(32);
            
            for (int x = 0; x <= 15; x++)
            {
                sb.Append(bytes[x].ToString("X2"));
            }
            return sb.ToString();
        }                
        public static bool Validate(string id)
        {
            int l;
            if (string.IsNullOrEmpty(id) || (l = id.Length) != 32)
            {
                return false;
            }
            while (l-- != 0)
            {
                var ch = id[l] | 32; //lowercase
                if (!((ch >= 'a' && ch <= 'f') || (ch >= '0' && ch <= '9')))
                {
                    return false;
                }
            }
            return true;
        }       

        
    }
}