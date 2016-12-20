using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

public partial class ajaxCallWithSession : System.Web.UI.Page
{
    protected override void OnLoad(EventArgs e)
    {
         //otherwise, ajax GET calls, will receive 304 and thus not update
        Response.CacheControl="No-Cache";
        Response.Expires=-1;


        Refresh = Request.QueryString["ajaxRefresh"];
        if (Refresh == "1") 
        {
            Session["CountRefresh"] =(int)Session["CountRefresh"] + 1;
        }

    }

    protected string Refresh { get; private set; }
}