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

        public IISPSession Create(string sessionKey,  bool isNewSessionKey, SessionAppSettings settings)
        {
            if (string.IsNullOrEmpty(sessionKey))
            {
                throw new ArgumentException("sessionKey cannot be null", nameof(sessionKey));
            }
           
            return new ISPSession(sessionKey,  isNewSessionKey, settings);
        }
    }
}
