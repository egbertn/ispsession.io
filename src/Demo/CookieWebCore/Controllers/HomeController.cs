using Microsoft.AspNetCore.Mvc;
using CookieWebCore.Models;

namespace CookieWebCore.Controllers
{
    public class HomeController : BaseController
    {
        public IActionResult Default()
        {
            var model = new HomeModel();
            model.CountRefresh =  (int)(Session["CountRefresh"] ?? 0);
            model.CountRefresh++;
            // again, this is not how it should be done, but to keep the code sample
            // consistent!
            model.Session = this.Session;
            Session["CountRefresh"] = model.CountRefresh;
            if (model.CountRefresh > 10)
            {
                Session.Abandon();
            }
            return View(model);
        }
    
        public IActionResult Ajax()
        {
            return View();
        }
        public IActionResult Error()
        {
            return View();
        }
    }
}
