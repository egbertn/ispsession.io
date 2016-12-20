<%@enablesessionstate=false codepage=65001%>
<!--#include file="ispSession.asp"-->
<%Dim sMethod, RF, sWord, sHost, findslash, cdoConfig, omail
set omail = CreateObject("CDO.Message")
set cdoConfig = CreateObject("CDO.Configuration")
sMethod = Request.ServerVariables("HTTP_METHOD")
'do not forget to set the reentrance flag to True
'otherwise this session is not resumable (security!)
Session.ReEntrance = True
Session.Timeout = 3600 '24 hours. Don't forget to set the timeout to a sufficient time where you expect the user to come back!

sHost = "http://" + Request.ServerVariables("SERVER_NAME") + Request.ServerVariables("HTTP_URL") + Request.QueryString("HTTP_QUERYSTRING")

If sMethod = "POST" Then
	Set RF = Request.Form
	sEmail = RF("EMAIL")
	sWord = RF("txtWord")
	
	with cdoConfig.fields
		.item("http://schemas.microsoft.com/cdo/configuration/sendusing").value = 2 'cdoSendUsingPort 
    	.item("http://schemas.microsoft.com/cdo/configuration/smtpserver").value = "[YOURSEVERIP]"
    	.item("http://schemas.microsoft.com/cdo/configuration/sendusername").value ="yoursmtpaccount" 'TODO		
    	.item("http://schemas.microsoft.com/cdo/configuration/sendpassword").value = "*********" 'TODO
		.item("http://schemas.microsoft.com/cdo/configuration/smtpauthenticate").value = 1 ' clear text
    	.update
	End With    			
	Set omail.Configuration = cdoConfig 
	oMail.To = sEmail
	oMail.From = "ResumeDemo@nieropwebconsult.nl"
	oMail.Subject = "Resume Session Demo"
	oMail.TextBody = "Resume your session with " + Session.URL(sHost) 
	oMail.HtmlBody = "<HTML><BODY>Resume your session with " + _
	"<a href=""" + Session.URL(sHost) + """>" + _
	"Click here</A><BR>" + _
	"Please close your browser to see that the session is resumed when you start a new browser using the URL inside the email!" + _
	"</BODY></HTML>"
	With Session.Contents
		.Item("sWord") = sWord
		.Item("EMAIL") = sEmail
	End With

	oMail.Send
	'Response.End
End If
%>
<div class="well">
<h2>Resume Session from URL Demo</h2>
<p>
This page shows that a session can be resumed using a URL inside an email. In fact, the URL can be kept anywhere. While your cookie has expired when you close the browser, it is still resumeable by having a browser click on a URL.<BR>
Note that this feature introduces security issues. A resumable session can be resumed by anyone who has access to the readable URL containing a sessionid.<BR>Programmers should take special care that such URL's are protected by passwords for instance.
</p>
<h1>You must modify the password and smtp username before running this demo</h1>
Your session ID: <%=Session.SessionID%><br/>

<form action="?page=resume" method="post">
What food do you like most?<br/>
<input type="text" maxlength="32" size="32" name="txtword" value="<%=Session("sWord")%>"/><br/>
To which email address we can sent your resumable Session? <br/><sub>Note, this email address is <b>not</b> monitored nor kept on file!</sub>
<br/>
<input type="text" maxlength="64" size="32" name="email" value="<%=Session("EMAIL")%>"/><br/>
<input type="submit" name="action" value="Send">
</form>

</div>