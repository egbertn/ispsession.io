using System;
namespace CookieWeb
{
    public partial class login : System.Web.UI.Page
    {
        protected override void OnLoad(EventArgs e)
        {
            if ( Session["ErrMsg"] != null)
            {
                ErrMsg.Text = (string)Session["ErrMsg"];
                Session["ErrMsg"] = null;
            }
            if (IsPostBack)
            {
                if (UserID.Text == "demo" && Password.Text == "demo")
                {

                    Session["LoggedIn"] = true;
                    //MAJOR WARNING. leaving out 'EndResponse', defaults to True
                    // if 'endresponse'==true the session will not be saved.
                    Response.Redirect ("SecurePage.aspx", false);
                }

                ErrMsg.Text = "Please try again, your user id / password  was wrong";

            }
        }
    }
}