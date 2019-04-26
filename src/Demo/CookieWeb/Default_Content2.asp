<%@enablesessionstate=false codepage=65001%>
<!--#include file="ispsession.asp"-->

<%
'Session.Contents.CaseSensitive = True
Response.Write TypeName(Session("CountRefresh"))
Session("CountRefresh") = Session("CountRefresh") + 1
If Clng(Session("CountRefresh")) = 4 Then
	Session.LiquidCookie = True
Else	
	Session.LiquidCookie = False
End If
If Clng(Session("CountRefresh")) > 10 Then 	
	Session.Abandon
End If
%>
<div class="well main">
    <h2>Compatibility and interoperability</h2>
    <p>  
Your sesssion ID = <%=Session.SessionID%><br />

<%=Session("CountRefresh")%><br/>