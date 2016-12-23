using CookieWeb.Models;
using System;
using System.Net.Mail;
using System.Threading.Tasks;
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
        public ActionResult About()
        {
            return View();
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
        [HttpPost]
        public ActionResult Login(LoginModel login)
        {

            if (!ModelState.IsValid)
            {
                Session["LoggedIn"] = false;
                return View(login);
            }

            Session["LoggedIn"] = true;
            return RedirectToAction("Secure");
        }
        [HttpGet]
        public ActionResult Logout()
        {
            Session.Remove("LoggedIn");
            ViewData.Remove("LoggedIn");
            return RedirectToAction("Default");
        }
        [HttpGet]
        public ActionResult Login()
        {

            var login = new LoginModel();

            return View(login);
        }
        public ActionResult Secure()
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

        public ActionResult Resume()
        {
            var model = new ResumeModel()
            {
                //tha magic
                Word = (string)(Session["Word"] ?? ""),
                Message = string.IsNullOrEmpty((string)(Session["Word"] ??"")) ? null : "Restored from session",
                SessionID = Session.SessionID,
                Email = (string)(Session["Email"] ?? "")
            };
            model.SessionID = Session.SessionID;
            return View(model);
        }
        public ActionResult Recordset()
        {
            return View();
        }
        [HttpPost]
        public async Task<ActionResult> Resume(ResumeModel resume)
        {
            if (!ModelState.IsValid)
            {
                return View(resume);
            }

            var msg = new MailMessage();
            msg.To.Add(new MailAddress(resume.Email));
            msg.From =new MailAddress("NOREPLY@ispsession.io");
            msg.Subject = "ISP Session resumable session demo";
            var host = Request.Url.Scheme +  "://"+ Request.Url.Host.ToString() +(Request.Url.IsDefaultPort ? "" : ":"+ Request.Url.Port.ToString()) + Url.Action("Resume", "Home", new { GUID = Session.SessionID });
            Session["Word"] = resume.Word;
            Session["Email"] = resume.Email;
            msg.Body = string.Format(@"<html><head></head><body>Resume your session with 

    <a href=""{0}"">Click here</a><br/>
    Please close your browser to see that the session is resumed when you start a new browser using the URL inside the email!
    </body></html>", host);
            msg.IsBodyHtml = true;



            var cl = new SmtpClient();
            
            //await cl.AuthenticateAsync(this._siteSettings.Value.UserName, this._siteSettings.Value.Password);
            cl.Send(msg);
            resume.Message = "mail is sent. You can close your browser";
            return View(resume);
        }
    }
}