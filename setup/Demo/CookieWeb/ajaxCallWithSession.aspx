<%@ Language="C#" CodeFile="ajaxCallWithSession.aspx.cs" EnableSessionState="ReadOnly" Inherits="ajaxCallWithSession"  %>

<p>
    This demonstrates that ajax calls ALSO can have a Session state!<br />
Session["CountRefresh"] = <%=Session["CountRefresh"]%><br />
This panel ran at: <%=DateTime.Now %>
    </p>
<p>
    <%if (this.Refresh == "1"){%>
    Thanks. As you can see when install the demo, and look at the source code, there is no magic or unusual classic ASP code inside the page itself, it just works!
    <%}else{%>
    Hit the Ajax panel refresh button!
    <%}%>
</p>


