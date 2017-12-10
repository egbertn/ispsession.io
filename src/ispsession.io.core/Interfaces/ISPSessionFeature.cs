using ispsession.io.core.Interfaces;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Http.Features;

namespace ispsession.io
{
    public interface IISPSEssionFeature: ISessionFeature
    {
        new IISPSession Session { get; set; }
        /// <summary>
        /// global application cache
        /// </summary>
        IApplicationCache Application { get; set; }
    }
    public class ISPSessionFeature: IISPSEssionFeature
    {
        public IISPSession Session { get; set; }
        public IApplicationCache Application { get; set; }
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