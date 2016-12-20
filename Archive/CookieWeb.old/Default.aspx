<%@ Page Language="C#" Debug="true" AutoEventWireup="true" CodeFile="Default.aspx.cs" Inherits="_Default"  MasterPageFile="MasterPage.master"  %>

<asp:Content runat="server" ContentPlaceHolderID="ContentPlaceHolder2">
<%
    var c = Session["CountRefresh"] ?? 0;
    var countRefresh = Convert.ToInt32(c);
    countRefresh++;
    Session["CountRefresh"] = countRefresh;
if (countRefresh == 10) {
	Session.Abandon();
}
    %>

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
    </div>
        <asp:Label runat="server" ID="test" />
        <asp:Button runat="server" Text="OK" />
        <asp:Label runat="server" id="txtJagged" />
    </form>
</asp:Content>
