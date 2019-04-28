using ispsession.io.Interfaces;
using System.Web;
using System;
namespace ispsession.io
{
    public static class Extensions
    {
        public static IApplicationCache ApplicationCache (this HttpContextBase context)
        {
            if (!context.Items.Contains(ISPApplicationModule.ItemContextKey))
            {
                throw new InvalidOperationException("ISP Cache has not correctly been registered make sure our 'ISPApplication' handler exists at web.Config/configuration/system.webServer/modules");
            }
            return (IApplicationCache)context.Items[ISPApplicationModule.ItemContextKey];
        }
        public static IApplicationCache ApplicationCache(this System.Web.HttpContext context)
        {
            if (!context.Items.Contains(ISPApplicationModule.ItemContextKey))
            {
                throw new InvalidOperationException("ISP Cache has not correctly been registered make sure our 'ISPApplication' handler exists at web.Config/configuration/system.webServer/modules");
            }
            return (IApplicationCache)context.Items[ISPApplicationModule.ItemContextKey];
        }
    }
}
