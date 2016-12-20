using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
namespace CookieWeb.Models
{
    /// <summary>
    /// Summary description for Class2
    /// </summary>
    public class HomeModel
    {
        public string SessionID { get; set; }

        public int CountRefresh { get; set; }
        //JUST TO DEMONSTRATE compatibility, not howto use the Session within MVC!
        public HttpSessionStateBase Session { get; set; }
    }
}