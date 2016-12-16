using Microsoft.AspNetCore.Mvc;
using CookieWebCore.Models;

namespace CookieWebCore.Controllers
{
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
            if (model.CountRefresh > 10)
            {
                Session.Abandon();
            }
            return View(model);
        }
        [HttpGet]
        public IActionResult Logout()
        {
            Session.Remove("Loggedin");
            return RedirectToAction("Default");
        }
        [HttpGet]
        public IActionResult Login()
        {
            /*
             * Dim RF
Set RF = Request.Form
'Stop
If RF("ACTION") = "OK" Then
	If RF("UserID") = "demo" And RF("Password") = "demo" Then
		Session("LoggedIn") = True

		Response.Redirect "?page=securepage"
	End If
	Session("ErrMsg") = "Please try again, your user id / password  was wrong"

End If
             * */
            var login = new LoginModel();

            return View(login);
        }
        public IActionResult Secure()
        {
            return View();
        }
        [HttpPost]
        public IActionResult Login(LoginModel login)
        {
            /*
             * Dim RF
Set RF = Request.Form
'Stop
If RF("ACTION") = "OK" Then
	If RF("UserID") = "demo" And RF("Password") = "demo" Then
		Session("LoggedIn") = True

		Response.Redirect "?page=securepage"
	End If
	Session("ErrMsg") = "Please try again, your user id / password  was wrong"

End If
             * */
            if (!ModelState.IsValid)
            {
                return BadRequest(ModelState);
            }
            return RedirectToAction("Secure");
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
