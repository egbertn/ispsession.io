<%@enablesessionstate=false%>
<!--#include file="ispsession.asp"-->
<%
Session.Contents.Remove("LoggedIn")

%><div class="well">
<h1>You are logged out now</h1>
    
Click here to try to enter the secured site<a href="?page=securepage">Enter secure page(if it's OK you are not allowed to...)</a><br/>
Click here to start again at the root<a href="./">Enter start of website again</a><br/>
</div>