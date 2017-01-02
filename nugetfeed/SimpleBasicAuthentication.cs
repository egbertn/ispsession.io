using System;
using System.Web;

namespace SimpleBasicAuthentication
{
    public class BasicAuthenticationModule : IHttpModule
    {
        public void Init(HttpApplication context)
        {
            context.AuthenticateRequest += this.context_AuthenticateRequest;
        }

        private void context_AuthenticateRequest(object sender, EventArgs e)
        {
            HttpApplication httpApplication = (HttpApplication)sender;
            if (!BasicAuthenticationProvider.Authenticate(httpApplication.Context, false))
            {
                httpApplication.Context.Response.Status = "401 Unauthorized";
                httpApplication.Context.Response.StatusCode = 401;
                httpApplication.Context.Response.AppendHeader("WWW-Authenticate", "Basic");
                httpApplication.CompleteRequest();
            }
        }

        public void Dispose()
        {
        }
    }
}
