
<%

on error resume next

Set SessionTest = Server.CreateObject("NWCTools.CSession")

if Err.Number = 0 then
	Set Session = Server.CreateObject("NWCTools.CSession")
	Response.Write "Using CSession<br /><br />"
else
	Response.Write "Using IIS Native Session<br /><br />"
end if

on error goto 0

pageName = "http://" & Request.ServerVariables("SERVER_NAME") & Request.ServerVariables("URL") 

if (Request.QueryString("step") = "-1") then

	Session.Abandon
	'Response.Write "<meta http-equiv=""refresh"" content=""0; url=" & pageName & """>"
	Response.Redirect pageName
	
elseif (Session("HasRequester") = true and Request.QueryString("step") = 2) then
	
	Set upRequester = Session("g_spRequester")
	
	Response.Write "Session State Test<br /><br />"
	
	Response.Write "AppId should be 1,  is " &  upRequester.AppId & "<br />"
	Response.Write "GroupId should be 2,  is " &  upRequester.GroupId & "<br />"
	Response.Write "ID(-1) should be -1,  is " &  upRequester.ID(-1) & "<br />"
	Response.Write "IdStack should be -1,  is " &  upRequester.IdStack & "<br />"
	Response.Write "Level should be 3,  is " &  upRequester.Level & "<br />"
	Response.Write "Lid should be -1,  is " &  upRequester.Lid & "<br />"
	Response.Write "ParentId should be -1,  is " &  upRequester.ParentId & "<br />"
	Response.Write "QueryString should be 'step=2&lIdStack=',  is '" &  upRequester.QueryString & "'<br />"
	Response.Write "SafeDateLCID should be 1031,  is " &  upRequester.SafeDateLCID & "<br />"
	Response.Write "TemplateId should be 4,  is " &  upRequester.TemplateId & "<br />"
	
	Response.Write "<br /><a href=""" & pageName & "?step=3&rq_strIdStack=-1,12,111"">Proceed to Request State Test</a><br />"

elseif (Session("HasRequester") = true and Request.QueryString("step") = 3) then
	
	Set upRequester = Session("g_spRequester")
	
	Response.Write "Request State Test<br /><br />"
	
	Response.Write "AppId should be 1,  is " &  upRequester.AppId & "<br />"
	Response.Write "GroupId should be 2,  is " &  upRequester.GroupId & "<br />"
	Response.Write "ID(-1) should be 12,  is " &  upRequester.ID(-1) & "<br />"
	Response.Write "IdStack should be -1,12,111,  is " &  upRequester.IdStack & "<br />"
	Response.Write "Level should be 3,  is " &  upRequester.Level & "<br />"
	Response.Write "Lid should be 111,  is " &  upRequester.Lid & "<br />"
	Response.Write "ParentId should be 12,  is " &  upRequester.ParentId & "<br />"
	Response.Write "QueryString should be 'rq_strIdStack=111&step=3&lIdStack=,-1,12,111',  is '" &  upRequester.QueryString & "'<br />"
	Response.Write "SafeDateLCID should be 1031,  is " &  upRequester.SafeDateLCID & "<br />"
	Response.Write "TemplateId should be 4,  is " &  upRequester.TemplateId & "<br />"
	
	Err.Clear
	on error resume next
	
	stepNum = upRequester.Get("step")
	
	if Err.Number <> 0 then
		Response.Write "Get(""step"") should be 3, but it threw an error<br />"
	else
		Response.Write "Get(""step"") should be 3, is " &  stepNum & "<br />"
	end if
	
	upRequester.Push 4000
	Response.Write "IdStack should be -1,12,111,4000,  is " &  upRequester.IdStack & "<br />"
	
	pop = upRequester.Pop
	Response.Write "IdStack should be -1,12,111,  is " &  upRequester.IdStack & "<br />"
	
	upRequester.AddRequestVariable "test", "33"
	Response.Write "Request Variable 'test' should be 33,  is " &  upRequester.Get("test") & "<br />"
	
	Response.Write "<br /><a href=""" & pageName & "?step=4"">Proceed to Final State Test</a><br />"
	
elseif (Session("HasRequester") = true and Request.QueryString("step") = 4) then
	
	Set upRequester = Session("g_spRequester")
	
	Response.Write "Final State Test<br /><br />"
	
	Response.Write "AppId should be 1,  is " &  upRequester.AppId & "<br />"
	Response.Write "GroupId should be 2,  is " &  upRequester.GroupId & "<br />"
	Response.Write "ID(-1) should be 12,  is " &  upRequester.ID(-1) & "<br />"
	Response.Write "IdStack should be -1,12,111,  is " &  upRequester.IdStack & "<br />"
	Response.Write "Level should be 3,  is " &  upRequester.Level & "<br />"
	Response.Write "Lid should be 111,  is " &  upRequester.Lid & "<br />"
	Response.Write "ParentId should be 12,  is " &  upRequester.ParentId & "<br />"
	Response.Write "QueryString should be 'step=4&lIdStack=,-1,12,111',  is '" &  upRequester.QueryString & "'<br />"
	Response.Write "SafeDateLCID should be 1031,  is " &  upRequester.SafeDateLCID & "<br />"
	Response.Write "TemplateId should be 4,  is " &  upRequester.TemplateId & "<br />"
	Response.Write "Request Variable 'test' should be 33,  is " &  upRequester.Get("test") & "<br />"
	
else
	Set upRequester = Server.CreateObject("UpRequester.Requester")
	Set Session("g_spRequester") = upRequester
	Session("HasRequester") = true
	
	Response.Write "Instantiation Test<br /><br />"
	
	Response.Write "AppId should be 0,  is " &  upRequester.AppId & "<br />"
	Response.Write "GroupId should be -1,  is " &  upRequester.GroupId & "<br />"
	Response.Write "ID(-1) should be -1,  is " &  upRequester.ID(-1) & "<br />"
	Response.Write "IdStack should be -1,  is " &  upRequester.IdStack & "<br />"
	Response.Write "Level should be -1,  is " &  upRequester.Level & "<br />"
	Response.Write "Lid should be -1,  is " &  upRequester.Lid & "<br />"
	Response.Write "ParentId should be -1,  is " &  upRequester.ParentId & "<br />"
	Response.Write "QueryString should be 'lIdStack=-1',  is '" &  upRequester.QueryString & "'<br />"
	Response.Write "SafeDateLCID should be 1031,  is " &  upRequester.SafeDateLCID & "<br />"
	Response.Write "TemplateId should be -1,  is " &  upRequester.TemplateId & "<br />"
	
	upRequester.AppId = 1
	upRequester.GroupId = 2
	upRequester.Level = 3
	upRequester.TemplateId = 4
	
	Response.Write "<br /><a href=""" & pageName & "?step=2"">Proceed to Session State Test</a><br />"
	
end if

Response.Write "<br /><br /><a href=""" & pageName & "?step=-1"">Restart Test</a><br />"

%>