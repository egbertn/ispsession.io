using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;
namespace CookieWeb
{
    /// <summary>
    /// Summary description for HomeController
    /// </summary>
    public class HomeController : Controller
    {
        [HttpGet]
        public ActionResult Default()
        {
       
            var model = new DefaultModel() { SessionID = Session.SessionID };
            return View(model);
        }
    }
}