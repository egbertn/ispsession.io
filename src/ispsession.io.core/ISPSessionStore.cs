using Microsoft.Extensions.DependencyInjection;
using System;

namespace ispsession.io.Store
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

        public IISPSession Create(string sessionKey, Func<ISPSession, bool> tryEstablish, bool isNewSessionKey, SessionAppSettings settings)
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
            return new ISPSession(sessionKey, tryEstablish,  isNewSessionKey, settings);
        }
    }
}
