using CookieWeb.Models;
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
            var model = new HomeModel() { SessionID = Session.SessionID };
            
            model.CountRefresh = (int)(Session["CountRefresh"] ?? 0);
            model.CountRefresh++;
            // again, this is not how it should be done, but to keep the code sample
            // consistent!
            model.Session = this.Session;
            Session["CountRefresh"] = model.CountRefresh;
            ViewData["LoggedIn"] = Session["LoggedIn"];
            Session["mydecimal"] = -1234.567M;
            if (model.CountRefresh > 10)
            {
                Session.Abandon();
            }
            return View(model);
        }

        [HttpGet]
        public ActionResult Ajax(string ajaxRefresh)
        {
            var model = new HomeModel() { SessionID = Session.SessionID };

            model.CountRefresh = (int)(Session["CountRefresh"] ?? 0);
            model.CountRefresh++;
            model.Refresh = ajaxRefresh;
            // again, this is not how it should be done, but to keep the code sample
            // consistent!
            model.Session = this.Session;
            Session["CountRefresh"] = model.CountRefresh;
            return PartialView(model);
        }
    }
}