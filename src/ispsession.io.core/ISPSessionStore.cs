using System;
using System.Threading.Tasks;
using ispsession.io.core.Interfaces;
using Microsoft.AspNetCore.Http;

namespace ispsession.io.core
{
    public class ISPSessionStore : IISPSessionStore
    {
        //private readonly ILoggerFactory _loggerFactory;

        public ISPSessionStore()
        {
          
            //if (loggerFactory == null)
            //{
            //    throw new ArgumentNullException("loggerFactory");
            //}
            //this._cache = cache;
            //this._loggerFactory = loggerFactory;
        }

        public IISPSession Create(string sessionKey, Func<ISPSession, Task<bool>> tryEstablish, bool isNewSession, SessionAppSettings settings)
        {
            if (string.IsNullOrEmpty(sessionKey))
            {
                throw new ArgumentException("sessionKey cannot be null", nameof(sessionKey));
            }
            if (settings == null)
            {
                throw new ArgumentNullException(nameof(settings), "settings cannot be null");
            }
            if (settings.SessionTimeout <=0 )
            {
                throw new ArgumentOutOfRangeException(nameof(settings.SessionTimeout), "SessionTimeout is zero or negative, which is not supported");
            }
            return new ISPSession(sessionKey,  tryEstablish, isNewSession, settings);
        }

     
    }
}
