<%@enablesessionstate=false codepage="65001"%>
<!--#include file="ispsession.asp"-->
<%'ISP Session can be optimized. If a page does not change Session data, 
    'set the page to readonly 

'Session.Readonly=true
    %><!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8"/>
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <meta name="viewport" content="width=device-width, initial-scale=1"/>
    <title>Welcome at our ISP Session Sample site - Sessions using Cookies</title>

    <!-- Bootstrap -->
    <link href="./lib/bootstrap/dist/css/bootstrap.min.css" rel="stylesheet">
    <link href="./lib/bootstrap/dist/css/bootstrap-theme.min.css" rel="stylesheet">
      <link href="./css/theme.css" rel="stylesheet">

    <!-- HTML5 Shim and Respond.js IE8 support of HTML5 elements and media queries -->
    <!-- WARNING: Respond.js doesn't work if you view the page via file:// -->
    <!--[if lt IE 9]>
      <script src="https://oss.maxcdn.com/libs/html5shiv/3.7.0/html5shiv.js"></script>
      <script src="https://oss.maxcdn.com/libs/respond.js/1.4.2/respond.min.js"></script>
    <![endif]-->
  </head>
  <body role="document">
      <div class="navbar navbar-inverse navbar-fixed-top" role="navigation">
      <div class="container">
        <div class="navbar-header">
          <button type="button" class="navbar-toggle" data-toggle="collapse" data-target=".navbar-collapse">
            <span class="sr-only">Toggle navigation</span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
            
          </button>          
          <a class="navbar-brand" href="#">ISP Session Demo</a>
        </div>
        <div class="navbar-collapse collapse">
          <ul class="nav navbar-nav">
            <li class="active"><a href="./">Home</a></li>
            <li><a href="?page=about">About</a></li>
              <%if Session("LoggedIn") <> True Then %>
            <li><a href="?page=login">Login</a></li><%else                
                 %><li><a href="?page=logout">Logout</a></li><%end if%>
               <li class="dropdown">
              <a class="dropdown-toggle" href="#" data-toggle="dropdown">Power tools<b class="caret"></b></a>
              <ul class="dropdown-menu">
                <li><a href="?page=statistics">Show Session statistics</a></li>                
                <li><a href="?page=resume">Resumable Session</a></li>
                <li><a href="?page=recordset">Store ADODB.Recordset and (XML) DOMDocument in Session</a></li>
                <li><a href="?page=download" target="_blank">Download this demo</a></li>            
              </ul>
            </li>     
          </ul>
        </div><!--/.nav-collapse -->
      </div>
    </div>
<%'ISP Session does not support Server.Execute, but it requires Session.Execute instead! %>
  <div class="container theme-showcase" role="main">
    
    <% dim p
        p = Request.QueryString("page")     
        Select Case p
        Case "aspx"
            Response.Redirect Session.URL("Default.aspx")
        Case "about"
            Session.Execute "about.asp"
        Case "recordset"
            Session.Execute "Recordset.asp"
        Case "logout"
            Session.Execute "logout.asp"
        Case "resume"
            Session.Execute "Resume.asp"
		Case "statistics"
			Session.Execute "statistics.asp"
        Case "securepage"
            Session.Execute "securepage.asp"
        Case "login"
            Session.Execute "Login.asp"
		Case "download"
			Response.Redirect "Cookieweb.zip"
        Case Else
            Session.Execute "Default_Content.asp"
        End Select
        Session.Timeout= 40
        %>
</div> <!-- container -->

    <!-- jQuery (necessary for Bootstrap's JavaScript plugins) -->
    <script src="https://code.jquery.com/jquery.js"></script>
    <!-- Include all compiled plugins (below), or include individual files as needed -->
    <script src="./lib/bootstrap/dist/js/bootstrap.min.js"></script>
	<script>
        function doAjaxPanel(refresh)
        {
         $.get(  "<%=Session.URL("ajaxCallWithSession.asp")%>&ajaxRefresh=" + (refresh ? "1" : "0"),
                function( data ) { $( "div.default_content" ).html( data );}
                   
                ); 
        }
		$( document ).ready(function() 
        {
        
		    $("button.refresh").click(function(){ window.location.reload();});
            // how to pass the session to an Ajax panel.   
            $("button.ajax").click(function(e) { doAjaxPanel(true);e.preventDefault();  });
            
            if ($(".main").length > 0) //Default_Content.asp tag
                doAjaxPanel(false);//default call
		});
        
	</script>
  </body>
</html>