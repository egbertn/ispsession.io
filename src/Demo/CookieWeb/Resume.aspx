<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Resume.aspx.cs" Inherits="CookieWeb.Resume" MasterPageFile="~/MasterPage.master" %>

<asp:Content runat="server" ContentPlaceHolderID="ContentPlaceHolder2">
    <div class="well">
<h2>Resume Session from URL Demo (NOT functional yet)</h2>
<p>
This page shows that a session can be resumed using a URL inside an email. In fact, the URL can be kept anywhere. While your cookie has expired when you close the browser, it is still resumeable by having a browser click on a URL.<BR>
Note that this feature introduces security issues. A resumable session can be resumed by anyone who has access to the readable URL containing a sessionid.<BR>Programmers should take special care that such URL's are protected by passwords for instance.
</p>
<h1>You must modify the password and smtp username before running this demo</h1>
Your session ID: <%=Session.SessionID%><br/>

<form action="?page=resume" method="post">
What food do you like most?<br/>
<input type="text" maxlength="32" size="32" name="txtword" value=""/><br/>
To which email address we can sent your resumable Session? <br/><sub>Note, this email address is <b>not</b> monitored nor kept on file!</sub>
<br/>
<input type="text" maxlength="64" size="32" name="email" value=""/><br/>
<input type="submit" name="action" value="Send">
</form>

</div>
</asp:Content>