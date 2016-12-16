using Microsoft.AspNetCore.Mvc;
using CookieWebCore.Models;

namespace CookieWebCore.Controllers
{
    /// <summary>
    /// December 2016, to demonstrate ISP Session
    /// For real security you should use the Authenticate attribute
    /// Reason I use this nineties style security, to demonstrate compatibility with classic ASP
    /// </summary>
    public class HomeController : BaseController
    {
        public IActionResult Default([FromQuery] string page)
        {
          
            var model = new HomeModel();
            model.CountRefresh =  (int)(Session["CountRefresh"] ?? 0);
            model.CountRefresh++;
            // again, this is not how it should be done, but to keep the code sample
            // consistent!
            model.Session = this.Session;
            Session["CountRefresh"] = model.CountRefresh;
            ViewData["LoggedIn"] = Session["LoggedIn"];
            if (model.CountRefresh > 10)
            {
                Session.Abandon();
            }
            return View(model);
        }
        [HttpGet]
        public IActionResult Logout()
        {
            Session.Remove("LoggedIn");
            ViewData.Remove("LoggedIn");
            return RedirectToAction("Default");
        }
        [HttpPost]
        public IActionResult Login(LoginModel login)
        {

            if (!ModelState.IsValid)
            {
                Session["LoggedIn"] = false;
                return BadRequest(ModelState);
            }
         
            Session["LoggedIn"] = true;            
            return RedirectToAction("Secure");
        }
        [HttpGet]
        public IActionResult Login()
        {

            var login = new LoginModel();

            return View(login);
        }
        public IActionResult Secure()
        {
            var loggedin = Session["LoggedIn"] ?? false;
            if ((bool)loggedin != true)
            {
                return RedirectToAction("Login");
            }
            ViewData["LoggedIn"] = true;
            ViewData["SessionID"] = Session.SessionID;
            return View();
        }
      
        public IActionResult Ajax()
        {
            var model = new HomeModel();
            model.CountRefresh = (int)(Session["CountRefresh"] ?? 0);
            model.CountRefresh++;
            Session["CountRefresh"] = model.CountRefresh;
            // again, this is not how it should be done, but to keep the code samples a little bit the same            
            model.Session = this.Session;
            return PartialView(model);
        }
        public IActionResult Error()
        {
            return View();
        }
    }
}
