using ispsession.io.core.Interfaces;
using System;

namespace ispsession.io
{
    public interface IISPSessionStore
    {
        IISPSession Create(string sessionKey, Func<ISPSession, bool> tryEstablish, bool isNewSessionKey, SessionAppSettings settings);
        IApplicationCache Create(SessionAppSettings settings);
    }
}
