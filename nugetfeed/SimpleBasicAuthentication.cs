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
            var context = httpApplication.Context;
            if (!BasicAuthenticationProvider.Authenticate(context, false))
            {
                var Response = context.Response;
                Response.Status = "401 Unauthorized";
                Response.StatusCode = 401;
                Response.AppendHeader("WWW-Authenticate", "Basic");
                httpApplication.CompleteRequest();
            }
        }

        public void Dispose()
        {
        }
    }
}
