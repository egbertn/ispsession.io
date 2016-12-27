<%@ Page Language="C#" Debug="true" AutoEventWireup="true" CodeBehind="Default.aspx.cs" Inherits="_Default"  MasterPageFile="MasterPage.master"  %>

<asp:Content runat="server" ContentPlaceHolderID="ContentPlaceHolder2">

    <form id="form1" runat="server">
    <div>        
        
            <h2>Compatibility and interoperability</h2>
            <p>  
                Your sesssion ID = <%=Session.SessionID%><br />
                <%if (Session["a3"] != null)
                  {
                      object[,] b = (object[,])Session["a3"];%><br />
                Dimensional Array (0,0) =<%=b[0,0]%><br/>
Dimensional Array (0,1) =	<%=b[0,1]%><br/>
Dimensional Array (1,1) =	<%=b[1,1]%><br/>
                      <%}%><br />
                Session("CountRefresh") = <%=Session["CountRefresh"]%>
             <%--   <%if (Session["myobjects"] != null){%>
                      <br/>Object Array Test: <%=((SomeData[])Session["myobjects"])[0].When%>
                     <%}%>
					                 <%if (Session["myintArray"] != null){%>
                      <br/>Int Array Test: <%=((int[])Session["myIntArray"])[3]%>
                     <%}%>--%>
                </p>
        <asp:Label runat="server" ID="realbig"/>
        <div class="panel panel-info">
    <div class="panel-heading">
        <h3 class="panel-title">An ajax panel which uses the ASP Session replacement</h3>
    </div>
    <div class="panel-body default_content">
        Ajax panel. If you see this, something obviously did not work
    </div>
</div>

<p>
    <button class="btn btn-lg ajax btn-primary" data-path='@Url.Action("Ajax", "Home", new  {GUID = Model.Session.SessionID})'>Ajaxpanel refresh</button>
</p>
<div class="well">
    We invite you to login at our website to see the secret section by clicking login.<br />

    Since we use an Ajax panel, iframes are supported as well. Even if they are from a different domain, as long as they run ISP Session on the same database server.
</div>
    </div>
        <asp:Label runat="server" ID="test" />
        <asp:Button runat="server" Text="OK" />
        <asp:Label runat="server" id="txtJagged" />
    </form>
</asp:Content>
