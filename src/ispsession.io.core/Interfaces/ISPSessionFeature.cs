using ispsession.io.core.Interfaces;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Http.Features;

namespace ispsession.io
{
    public interface IISPSEssionFeature: ISessionFeature
    {
        new IISPSession Session { get; set; }
       
    }
    public class ISPSessionFeature: IISPSEssionFeature
    {
        public IISPSession Session { get; set; }
     
        ISession ISessionFeature.Session
        {
            get
            {
               return Session;
            }

            set
            {
                
            }
        }
    }
}