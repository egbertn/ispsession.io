<%@enablesessionstate=false%>
<!--#include file="ispsession.asp"-->
<%
Dim RF 
Set RF = Request.Form
'Stop
If RF("ACTION") = "OK" Then
	If RF("UserID") = "demo" And RF("Password") = "demo" Then
		Session("LoggedIn") = True

		Response.Redirect "?page=securepage"
	End If
	Session("ErrMsg") = "Please try again, your user id / password  was wrong"
	
End If

%><div class="well">
Please login before you get to the secret section for this site...<br/>
user: demo <br/>
password demo<br/>
<form action="?page=login" method="post">
<table>
<tr>
<td colspan="2"><%
Response.Write Session("ErrMsg") 

Set SC = Session.Contents
SC.Remove "ErrMsg"
Set SC = Nothing
%></td>
</tr>
<tr>
<td>UserID:</td>
<td><input type="text" name="UserID" maxlength="10"/>
</td>
</tr><tr>
<td>Password:</td><td><input type="password" name="Password" maxlength="4"/></td>
</tr>
<tr>
<td>&nbsp;</td><td>
<input type="submit" class="btn btn-lg btn-default" value="OK" name="ACTION"/></td>
</tr>
</table>
</form>

  </div>
