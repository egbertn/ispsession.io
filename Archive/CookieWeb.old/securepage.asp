<%@enablesessionstate=false%>
<!--#include file="ispsession.asp"-->
<%
If Session("LoggedIn") <> True Then
	Session("ErrMsg") = "Your session has expired or your login was not successfull!"
	Response.Redirect "?page=login"
End If
%><div class="well">
    <h2>
    Congratulations! You have entered the secure site. 
        </h2>
    Your sesssion ID = <%=Session.SessionID%><br />
</div>


