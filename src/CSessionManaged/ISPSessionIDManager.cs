﻿using System;
using System.Globalization;
using System.Security.Cryptography;
using System.Text;
using System.Web;
using System.Web.SessionState;

namespace ispsession.io
{

    // no support for cookieless, too much hassle and also bad for SEO.

    /// <summary>
    /// required to replace the compatible session cookie for ISP Session both managed and unmanaged
    /// </summary>
    public sealed class ISPSessionIDManager: ISessionIDManager 
    {
        private readonly SessionAppSettings _settings;
        private static readonly RandomNumberGenerator CryptoRandom = RandomNumberGenerator.Create();
        public ISPSessionIDManager(SessionAppSettings settings)
        {
            _settings = settings;
        }
       
        public string GetSessionID(HttpContext context)
        {
            string cookieText = null;
            bool foundGuidinURL = false;
            if (_settings.SnifQueryStringFirst)
            {
                var urlCookie = context.Request.QueryString[_settings.CookieName];
                if (urlCookie != null)
                {
                    cookieText = urlCookie;
                    foundGuidinURL = true;
                }
            }
            string cookieValue = null;
            var httpCookie = context.Request.Cookies[_settings.CookieName];
            if (httpCookie != null && httpCookie.Value != null)
            {
                cookieValue = httpCookie.Value;
                if (!Validate(cookieValue))
                {
                    cookieValue = null;
                    if (_settings.SnifQueryStringFirst == false)
                    {
                        var urlCookie = context.Request.QueryString[_settings.CookieName];
                        if (urlCookie != null && Validate(urlCookie))
                        {
                            Helpers.TraceInformation("GetSessionID found url guid {0}", urlCookie);
                            cookieText = urlCookie;
                            foundGuidinURL = true;
                        }
                    }
                }
                else
                {
                    Helpers.TraceInformation("GetSessionID found cookie guid {0}", cookieValue);
                }
            }
            
            var db = CSessionDL.SafeConn.GetDatabase(_settings.DataBase);
            bool foundSession = false;
            if (foundGuidinURL)
            {

                foundSession = db.RedundantExists(cookieText, _settings);
                if (foundSession)
                {
                    return cookieText;
                }
                else if (_settings.ReEntrance == true && !string.IsNullOrEmpty(cookieText)) //allow to ressurrect a session id, however, the data is gone
                {
                    Helpers.TraceInformation("GetSessionID revived session url guid {0}", cookieText);
                    return cookieText;
                }             
                return null;
            }
            if (cookieValue == null)
            {
                return null;
            }

            foundSession = db.RedundantExists(cookieValue, _settings);
            if (foundSession)
            {
                return cookieValue;
            }
            else if (_settings.ReEntrance && !string.IsNullOrEmpty(cookieValue))
            {
                Helpers.TraceInformation("GetSessionID revived session cookie guid {0}", cookieValue);
                return cookieValue;
            }
            return null;
        }

        public string CreateSessionID(HttpContext context)
        {
            var bt = new byte[16];
            CryptoRandom.GetBytes(bt);
            return GuidToHex(bt);
        }

        private static string GuidToHex(byte[] bytes)
        {
            var sb = new StringBuilder(32);
            for (int x = 0; x <= 15; x++)
            {
                sb.Append(bytes[x].ToString("X2"));
            }
            return sb.ToString();
        }

        public bool Validate(string id)
        {
            if (string.IsNullOrEmpty(id) || id.Length != 32)
            {
                return false;
            }
            for (int x = 0; x <= 30; x += 2)
            {
                byte bogus;
                if (byte.TryParse(id.Substring(x, 2), NumberStyles.AllowHexSpecifier, null, out bogus) == false)
                {
                    return false;
                }
            }
            return true;
        }

        internal HttpCookie CreateSessionCookie(string id, bool isHttps)
        {
            //TODO: config domain etc...
            return new HttpCookie(_settings.CookieName, id)
            {
                Path = !string.IsNullOrEmpty(_settings.Path) ? _settings.Path : "/",
                Expires = _settings.CookieExpires > 0 ? DateTime.UtcNow.AddMinutes(_settings.CookieExpires) : DateTime.MinValue,
                Domain = !string.IsNullOrEmpty(_settings.Domain) ? _settings.Domain : null,
                Secure = isHttps ? (_settings.CookieNoSSL == false ? true : false) : false
            };
        }

        public void SaveSessionID(HttpContext context, string id, out bool redirected, out bool cookieAdded)
        {
            redirected = false; //because we do not do HttpCookieMode.UseUri            
            cookieAdded = true;//always added because HttpCookieMode.UseCookies 
            var response = context.Response;
            Helpers.TraceInformation("SaveSessionID {0}", id);
            if (response.HeadersWritten)
            {
                throw new HttpException("Response was Flushed, cannot save Session Cookie");
            }
            if (!Validate(id))
            {
                throw new HttpException("Invalid SessionID");
            }

            var isHttps = context.Request.IsSecureConnection;
            var cookies = response.Cookies;
            if (Array.FindIndex(cookies.AllKeys, x => x == _settings.CookieName) >= 0)
            {
                cookies.Remove(_settings.CookieName);
            }
            cookies.Add(CreateSessionCookie(id, isHttps));
            //Session implies, that caching is disabled
            var cache = response.Cache;
            response.Headers["pragma"] = "no-cache";            
            cache.SetCacheability(HttpCacheability.NoCache);
            cache.SetExpires(DateTime.UtcNow.AddMinutes(-1));
            cache.SetRevalidation(HttpCacheRevalidation.AllCaches);
            cache.SetNoStore();
        }
       
        public void RemoveSessionID(HttpContext context)
        {
            context.Response.Cookies.Remove(_settings.CookieName);
        }
        //bogus
        void ISessionIDManager.Initialize()
        {
        
        }
        //bogus
        bool ISessionIDManager.InitializeRequest(HttpContext context, bool suppressAutoDetectRedirect, out bool supportSessionIDReissue)
        {
            suppressAutoDetectRedirect = true;
            supportSessionIDReissue = true;
            return true;
        }

    }
}