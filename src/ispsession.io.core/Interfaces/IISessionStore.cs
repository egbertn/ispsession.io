namespace ispsession.io
{
    public interface IISPSessionStore
    {
        IISPSession Create(string sessionKey, bool isNewSessionKey, SessionAppSettings settings);
    }
}
