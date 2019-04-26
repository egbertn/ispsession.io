<%@enablesessionstate=false%>
<% 'otherwise, ajax GET calls, will receive 304 and thus not update
    Response.CacheControl="No-Cache"
    Response.Expires=-1 %>
<!--#include file="ispsession.asp"-->
<%'ISP Session can be optimized. If a page does not change Session data, 
    'set the page to readonly 
    Dim refresh, countRefresh 
    refresh = Request.QueryString("ajaxRefresh")
    If refresh = "1" Then
    Session("CountRefresh") = Session("CountRefresh") + 1
	
End If  %>

<p>
    This demonstrates that ajax calls ALSO can have a Session state!<br />
Session("CountRefresh") = <%=Session("CountRefresh") %><br />
This panel ran at: <%=Now() %>
    </p>
<p>
    <%If refresh = "1" Then%>
    Thanks. As you can see when install the demo, and look at the source code, there is no magic or unusual classic ASP code inside the page itself, it just works!
    <%else%>
    Hit the Ajax panel refresh button!
    <%end if%>
</p>


