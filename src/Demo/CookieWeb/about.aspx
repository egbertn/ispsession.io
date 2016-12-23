<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="about.aspx.cs" Inherits="CookieWeb.about" MasterPageFile="~/MasterPage.master" %>

<asp:Content runat="server" ContentPlaceHolderID="ContentPlaceHolder2">
    <h2>About this ISP Session demo</h2>
<div class="well">
The session is managed using the ADC Cure ASP Session Replacement called ISP Session!<br/>
    This demo also shows some of the supported different types of OLE automation variables which do exist, such as (VB naming) Boolean, Byte, Integer, Long, Currency, Variant, Variant Array, Date, String, Double, Single, and persistable Objects (such as ADODB.Recordset)<br />
    In addition, it supports the 8-byte integer, officially also supported OLE Automation, but not officially by VB Script<br />
</div>
<div class="well">
    <h2>ISP Session supports compatibility among various Microsoft ASP versions and frameworks</h2>
    <p>
        As long as you use interoperable variable types, (ispsession.io mentions which ones) variables can be shared.<br />
        Run this website e.g. with .aspx extension<br />
        default.asp or default.aspx, show that the session is persisted and maintained.<br />
        This also applies to MVC controller views.
    </p>
</div>
 </asp:Content>