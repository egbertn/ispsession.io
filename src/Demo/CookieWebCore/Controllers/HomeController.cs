using Microsoft.AspNetCore.Mvc;
using CookieWebCore.Models;
using Microsoft.Extensions.Options;
using MimeKit;
using System.Threading.Tasks;
#if NETSTANDARD1_6

using MailKit.Security;
using MimeKit.Text;
#endif
using System;

namespace CookieWebCore.Controllers
{
    /// <summary>
    /// December 2016, to demonstrate ISP Session
    /// For real security you should use the Authenticate attribute
    /// Reason I use this nineties style security, to demonstrate compatibility with classic ASP
    /// </summary>
    public class HomeController : BaseController
    {
        private IOptions<MailSettings> _siteSettings;
        public HomeController(IOptions<MailSettings> mailSettings)
        {
            _siteSettings = mailSettings;
        }
        public IActionResult Default([FromQuery] string page)
        {

            var model = new HomeModel();
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
        public IActionResult Logout()
        {
            Session.Remove("LoggedIn");
            ViewData.Remove("LoggedIn");
            return RedirectToAction("Default");
        }
        [HttpGet]
        public IActionResult Resume()
        {
            var model = new ResumeModel()
            {
                Email = (string)(Session["Email"] ?? ""),
                Word = (string)(Session["Word"] ?? ""),
                Message = string.IsNullOrEmpty((string)(Session["Word"] ?? "")) ? null : "Restored from session",
                SessionID = Session.SessionID
            };
            return View(model);
        }
        [HttpGet]
        public IActionResult About()
        {
            return View();
        }
        [HttpPost]
        public async Task<IActionResult> Resume(ResumeModel resume)
        {
            if (!ModelState.IsValid)
            {
                return View(resume);
            }
            Session["Email"] = resume.Email;
            Session["Word"] = resume.Word;
#if NETSTANDARD1_6
            var msg = new MimeMessage();
            msg.To.Add(new MailboxAddress(resume.Email));
            msg.From.Add(new MailboxAddress("NOREPLY@ispsession.io"));
            msg.Subject = "ISP Session resumable session demo";
            var host = Request.Scheme +  "://"+ Request.Host.Host.ToString() + Url.Action("Resume", "Home", new { GUID = Session.SessionID });
            msg.Body = new TextPart(TextFormat.Html) { Text = $@"<html><head></head><body>Resume your session with 

    <a href=""{host}"">Click here</a><br/>
    Please close your browser to see that the session is resumed when you start a new browser using the URL inside the email!
    </body></html>"};




            var cl = new MailKit.Net.Smtp.SmtpClient();
            var arr = this._siteSettings.Value.SmtpServer.Split('.');
            cl.LocalDomain = string.Join(".",arr,1,arr.Length-1);
            await cl.ConnectAsync(this._siteSettings.Value.SmtpServer, 25, SecureSocketOptions.StartTlsWhenAvailable);
            await cl.AuthenticateAsync(this._siteSettings.Value.UserName, this._siteSettings.Value.Password);
            await cl.SendAsync(msg);
#endif
            return View(resume);
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
      
        public IActionResult Ajax(string ajaxRefresh)
        {
            var model = new HomeModel();
            model.CountRefresh = (int)(Session["CountRefresh"] ?? 0);
            model.CountRefresh++;
            Session["CountRefresh"] = model.CountRefresh;
            // again, this is not how it should be done, but to keep the code samples a little bit the same            
            model.Session = this.Session;
            model.Refresh = ajaxRefresh;
            return PartialView(model);
        }
        public IActionResult Error()
        {
            return View();
        }
    }
}
