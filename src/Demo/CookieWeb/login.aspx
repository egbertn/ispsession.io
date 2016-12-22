<%@ Language="C#" AutoEventWireup="true" CodeBehind="Login.aspx.cs" Inherits="CookieWeb.login"  MasterPageFile="MasterPage.master" %>

<asp:Content runat="server" ContentPlaceHolderID="ContentPlaceHolder2">

        <div class="well">
        Please login before you get to the secret section for this site...<br/>
        user: demo <br/>
        password demo<br/>
        <form runat="Server" action="login.aspx" method="post">
            <fieldset class="form-fields">
            <table>
                <tr>
                    <td colspan="2">
                        <asp:Label runat="server" id="ErrMsg" />


                        </td>
                 </tr>
                <tr>
                    <td>UserID:</td>
                    <td><asp:TextBox runat="server" ID="UserID" maxlength="10"/>
                </td>
                </tr>
                <tr>
                    <td>Password:</td>
                    <td><asp:TextBox TextMode="password" ID="Password" runat="Server" maxlength="4"/></td>
                </tr>
                <tr>
                    <td>&nbsp;</td>
                    <td>
                        <asp:Button type="submit" runat="Server" Text="OK" CssClass="btn btn-lg btn-default" ID="ACTION"/></td>
                </tr>
            </table>
                </fieldset>
        </form>

  </div>

</asp:Content>