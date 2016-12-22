<%@Page language="C#" AutoEventWireup="true" MasterPageFile="MasterPage.master" Inherits="CookieWeb.securepage" %>

<asp:Content runat="Server" id="Content" ContentPlaceHolderID="ContentPlaceHolder2">
<div class="well main">
    <h2>
    Congratulations! You have entered the secure site. 
        </h2>
    Your sesssion ID = <%=Session.SessionID%><br />
</div>

</asp:Content>