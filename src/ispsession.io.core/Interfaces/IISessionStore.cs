using System;
using System.Threading.Tasks;

namespace ispsession.io
{
    public interface IISPSessionStore
    {
        IISPSession Create(string sessionKey, Func<ISPSession, Task<bool>> tryEstablish, bool isNewSessionKey, SessionAppSettings settings);
    }
}
