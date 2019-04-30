using Microsoft.AspNetCore.Http;
using System;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

namespace ispsession.io.core
{
    /// <summary>
    /// required to replace the compatible session cookie for ISP Session both managed and unmanaged
    /// </summary>
    public sealed class ISPSessionIDManager
    {

        private readonly HttpContext _context;
        private  string _id;
        private readonly SessionAppSettings _settings;
        private static readonly RandomNumberGenerator CryptoRandom = RandomNumberGenerator.Create();
        public ISPSessionIDManager(SessionAppSettings settings)
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
        internal async Task<bool> TryEstablishSession(ISPSession i)
        {
            var ret = this._shouldEstablishSession |= !this._context.Response.HasStarted;
            if (ret && i.IsNewSession == false)
            {
              
                await i.LoadAsync();
                if (i.Liquid)
                {
                    i.OldSessionID = i.SessionID;
                    var newSessionID = CreateSessionID(_context);
                    this._id =  i.SessionID = newSessionID;                     
                }
                
            }

            return true;
        }
        public ISPSessionIDManager(HttpContext context, string id, SessionAppSettings settings)
        {
            _settings = settings;
            _context = context;
            _id = id;
          
            context.Response.OnStarting(OnStartingCallback, this);
        }
        private static Task OnStartingCallback(object state)
        {
            var sessionIdManager = (ISPSessionIDManager)state;
            if (sessionIdManager._shouldEstablishSession )
            {
                sessionIdManager.SetCookie();
                
            } 
            return Task.CompletedTask;
        }
        private void SetCookie()
        {
            if (Validate(_id) == false)
            {
                return;
            }
            var request = _context.Request;

            StreamManager.TraceInformation("SetCookie {0}, {1}", _id, request.Path);
            var isHttps = request.IsHttps;
            var resp = this._context.Response;
            var opts = new CookieOptions()
            {
                Domain = string.IsNullOrEmpty(this._settings.Domain) ? null : this._settings.Domain,
                Path = this._settings.Path ?? request.Path,
                Secure = isHttps && _settings.CookieNoSSL == false ? true : false,
                Expires = _settings.CookieExpires == 0 ? default(DateTimeOffset?) : DateTime.UtcNow.AddMinutes(_settings.CookieExpires)
            };
            //unfortunately, workaround, otherwise cookies get duplicated
            //resp.Cookies.Delete(this._settings.CookieName, opts);

            resp.Cookies.Append(this._settings.CookieName, this._id, opts);

            var headers = resp.Headers;
            headers["Cache-Control"] = "no-cache";
            headers["Pragma"] = "no-cache";
            headers["Expires"] = "-1";
        }
        public async Task< string> GetSessionIDAsync(HttpContext context)
        {
            string cookieText = null;
            bool foundGuidinURL = false;
            var request = context.Request;
            if (_settings.SnifQueryStringFirst)
            {
                var cookie = request.Query[_settings.CookieName];

                if (cookie.Count > 0)
                {
                    cookieText = cookie[0];
                    if (Validate(cookieText))
                    {
                        foundGuidinURL = true;
                    }

                }
            }
            string cookieValue = null;
            var httpCookie = request.Cookies[_settings.CookieName];

            if (httpCookie != null && !foundGuidinURL)
            {
                cookieValue = httpCookie;
                if (!Validate(cookieValue))
                {
                    cookieValue = null;
                    if (_settings.SnifQueryStringFirst == false)
                    {
                        var urlCookie = request.Query[_settings.CookieName];

                        if (urlCookie.Count > 0 && Validate(urlCookie[0]))
                        {
                            StreamManager.TraceInformation("GetSessionID found url guid {0}, {1}", urlCookie, request.Path);
                            cookieText = urlCookie;
                            foundGuidinURL = true;
                        }
                    }
                }
                else
                {
                    StreamManager.TraceInformation("GetSessionID found cookie guid {0}, {1}", cookieValue, request.Path);
                }
            }
            var db = CSessionDL.GetDatabase(_settings);
            bool foundSession = false;
            if (foundGuidinURL)
            {

                foundSession = await db.RedundantExistsAsync(cookieText, _settings);
                if (foundSession)
                {
                    return cookieText;
                }
                else if (_settings.ReEntrance == true && !string.IsNullOrEmpty(cookieText)) //allow to ressurrect a session id, however, the data is gone
                {
                    StreamManager.TraceInformation("GetSessionID revived session url guid {0}, {1}", cookieText, request.Path);
                    return cookieText;
                }
                return null;
            }
            if (cookieValue == null)
            {
                return null;
            }

            foundSession = await db.RedundantExistsAsync(cookieValue, _settings);
            if (foundSession)
            {
                return cookieValue;
            }
            else if (_settings.ReEntrance ==true && !string.IsNullOrEmpty(cookieValue))
            {
                StreamManager.TraceInformation("GetSessionID revived session cookie guid {0}", cookieValue);
                return cookieValue;
            }
            return null;
        }

        internal string CreateSessionID(HttpContext context)
        {
            var bt = new byte[16];
            CryptoRandom.GetBytes(bt);
            return GuidToHex(bt);
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