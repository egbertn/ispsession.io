using System;
using System.Web.UI;

namespace CookieWeb
{
    public partial class securepage: Page
    {
        protected override void OnLoad(EventArgs e)
        {
            if ((bool)(Session["LoggedIn"] ?? false) != true)
            {
                Session["ErrMsg"] = "Your session has expired or your login was not successfull!";

                Response.Redirect("login.aspx", false);
            }
        }
    }
}