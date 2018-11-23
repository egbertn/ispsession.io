<%@enablesessionstate=false codepage=65001%>
<!--#include file="ispsession.asp"-->

<%
'Session.Contents.CaseSensitive = True
Response.Write TypeName(Session("CountRefresh"))
Session("CountRefresh") = Session("CountRefresh") + 1
If Clng(Session("CountRefresh")) > 10 Then 	
	Session.Abandon
End If
%>
<div class="well main">
    <h2>Compatibility and interoperability</h2>
    <p>  
Your sesssion ID = <%=Session.SessionID%><br />

<button class="btn btn-lg refresh btn-primary">Refresh this page</button>
<%

if Not IsEmpty(Session("a3")) then
Dim b,driedimg    
	b = Session("a3")
    driedimg = Session("driedim")
    %><br />
Dimensional Array (0,0) =<%=b(0,0)%><br/>
Dimensional Array (0,1) =	<%=b(0,1)%><br/>
Dimensional Array (1,1) =	<%=b(1,1)%><br/>
<%if not isempty(driedimg) Then%>
Driedim Array(0,1,0) = <%=driedimg(0,1,0) %>        
<%
    End If
    End If%>
Session("a0") =<%=Session("a0")%><br/>
Session("a2") = <%=Session("a2")%><br/>
Session("CountRefresh") = <%=Session("CountRefresh")%><br/>
 Cache("Count") = <%=Cache("count")%>
<br/><%

    Cache.ExpireKeyAt "Count", 2000
dim a()
redim driedim(1,1,1)
    driedim(0,0,0) = "aap"
    driedim(0,0,1) = "noot"
    driedim(0,1,0) = "mies"
Session("driedim") = driedim
session("aaa") = a
redim a(1,1)
a(0,0) = "start"
a(0,1) = Now()
a(1,1) = "Bye"
Session("a3") = a
Session("a0") = "Smith"
Session("a2") = Now()


%> 
</p>
</div>

<div class="panel panel-info">
            <div class="panel-heading">
              <h3 class="panel-title">An ajax panel which uses the ASP Session replacement</h3>
            </div>
            <div class="panel-body default_content">
              Ajax panel. If you see this, something obviously did not work
            </div>
</div>

<p>
<button class="btn btn-lg ajax btn-primary">Ajaxpanel refresh</button></p>
<div class="well">
We invite you to login at our website to see the secret section by clicking login.<br/>

    Since we use an Ajax panel, iframes are supported as well. Even if they are from a different domain, as long as they run ISP Session on the same database server.
</div>
<pre>
    <%
		Response.Write "<br/>"
	    Response.Write Cache("category_array")(3, 4)
		Response.Write "<br/>"
        Response.Write "Empty ? " & IsEmpty(Cache("EmptyVal")) & Cache("EmptyVal")
        Response.Write "<br/>"
        Response.Write  Cache("LngVal")
        Response.Write "<br/>"
        Response.Write "longstring ends with" + Right( Cache("BigString"), 10) & " " & Len(Cache("BigString"))
        Response.Write "<br/>"
        Response.WRite "ByteString=" & Chr(AscB(Cache("ByteString"))) & "  " & LenB(Cache("ByteString"))
        Response.Write "<br/>"
        Response.Write "SingleDim=" & Cache("SingleDim")(1)
        Response.Write "<br/>"
        Response.Write Cache("NullVal") & "Null=" & IsNull(Cache("NullVal"))
        Response.Write "<br/>"
        Response.Write "Session Count Variables = "
        Response.Write Session.Contents.Count

        REsponse.Write "<br/>"
        For Each k in Session.Contents
            Response.Write k 
            Response.Write "<br/> type="
            dim t
            t = Session.Contents.VarType(k)
            Response.Write Session.Contents.VarType(k)
            Response.Write "<br/> value="
            if (t And vbArray) = vbArray  Then            
                'Response.Write Session.Contents.Item(k)
            Else
                Response.Write Session.Contents.Item(k)
            End If
            Response.Write "<br/>"
        Next
        REsponse.Write "GetKey="
        Response.Write Session.Contents.Key(1)
        Response.Write "<br/>"
        Response.Write Session.Contents.Exists("A0")       
        if Cache("Count") > 30 Then
            Cache("Muke") = 10
            Cache.Remove "Muke"
            cache.removeall
        End If
    %>
</pre>