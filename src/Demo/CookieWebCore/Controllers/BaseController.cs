using ispsession.io;
using Microsoft.AspNetCore.Mvc;
using System;

namespace CookieWebCore.Controllers
{
    public class BaseController: Controller
    {
        protected IISPSession Session
        {
            get
            {
                var features = HttpContext.Features.Get<IISPSEssionFeature>();
                if (features != null)
                {
                    return ((ISPSessionFeature)features).Session;
                }
                throw new Exception("ISP SEssion Feature not found");
            }
        }
    }
}
