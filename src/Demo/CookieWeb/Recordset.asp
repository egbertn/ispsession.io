<%@enablesessionstate="false" codepage="65001"%>
<%Option Explicit%><%
Dim TT
TT = Timer
%>

<!--#include file="ISPSession.asp"-->
<%
' This page shows how to deal with cached ADO recordsets or a COM Component into the session without exhausting threads or other server resources.
' Of course this is just a sample but ideal use is for reports
' that just need to be generated once but the reader would like to filter
' on the number of pages or change the sort order
' In the case of a cached recordset this Session object is ideal to 
' release the tasks of the DBMS.
' Other reasons would be caching non-indexed optimized queries on large tables
' If ADO can store just a small subset, using ADO as filter/sort engine is OK

'in this DEMO we assume you use SQL server or SQL Express.
Dim rs, cx, fldCustID, fldCustName, fldShippedDate, fldOrdID, blnNew, lngDetail
Dim RQ, OrderBy, stD, strDir, TTT, strB
TTT = Timer - TT
TT = Timer
Dim testPersist


If Cache("washere") <> True Then
	Set Rs = CreateObject("ADODB.Recordset")
	' Session.CreateInstance also cares for calling the InitNew method!	
	' you must register CPPPersist.DLL (on 32 bit systems) before you run this!
	' on 64 bit systems, it is CPPPersist64.dll to register.
    Set testPersist =  CreateObject("Msxml2.FreeThreadedDOMDocument.6.0")
    testPersist.loadxml "<blah>Contents</blah>"
	Dim sXMLData
	sXMLData=Server.MapPath(".") + "\thestuff.xml"	
	Cache("washere") = True
	Rs.CursorLocation = 3

	'to get the recordset persisted.
	' it is possible that this provider was not installed on Windows 2008 and higher
	Rs.Open sXMLData, "Provider=MSPersist"

	'Disconnect now	but keep the recordset still open
	Set Rs.ActiveConnection = Nothing
	Rs.MoveFirst
	Set Cache("CachedRs") = Rs
	Set Cache("TestPersist") = testPersist
	
	blnNew = True
Else
	'Activate the ADO rs JIT

	Set Rs = Cache("CachedRs")

	Set testPersist = Cache("TestPersist")
	
	Rs.Delete ' this proves that modified data also is persisted
	Rs.MoveNext ' avoid no current record position error

	blnNew = False
	
End If
'Response.Write "<html></html>"
'REsponse.End
' If Not IsEmpty(Cache("CachedRs")) Then 
	' Set Rs = Cache("CachedRs")
' End If
' Set testPersist = Cache("TestPersist")


Set RQ = Request.QueryString
lngDetail = RQ("d")
OrderBy = RQ("o")

'swap from a to d and from d to a (descending - ascending)
If Right(OrderBy,1) = "d" Then 
	stD = "a" 
ElseIf Right(OrderBy,1) = "a" Then
	stD = "d"
Else
	stD = "a"
End If

%>
<div class="well">
<p>
Click on a column item to sort on it. It will switch between ascending/descending sort<br/>
To get the source code just download the demo.</p>
    <p>
        Note: The data is cached in the Application cache and initialized in global.asa (Application_OnStart)
</p>
<table border="1" >
    <caption><%=Session.SessionID %></caption>
<tr>
<th><a href="?page=recordset&o=C<%=stD%>&d=<%=lngDetail%>">CustomerID</a></th>
<th><a href="?page=recordset&o=O<%=stD%>&d=<%=lngDetail%>">OrderID</a></th>
<th><a href="?page=recordset&o=S<%=stD%>&d=<%=lngDetail%>">ShippedDate</a></th>

</tr>
<%
If Right(OrderBy,1) = "d" Then strDir = "DESC" Else strDir = "ASC"
Select Case Left(OrderBy,1)
	Case "C"
		rs.Sort = "CustomerID " & strDir
	Case "O"
		rs.Sort = "OrderID " & strDir
	Case "S"
		rs.Sort = "ShippedDate " & strDir
End Select
Set fldCustID = Rs("CustomerID")
Set fldOrdID = Rs("OrderID")
Set fldShippedDate = Rs("ShippedDate")

Do Until Rs.Eof
	Response.Write "<tr><td>"
	Response.Write fldCustID
	Response.Write "</td><td>"
	Response.Write fldOrdID
	Response.Write "</td><td>"

	Response.Write fldShippedDate
	Response.Write "</td></tr>"
	Rs.MoveNext
Loop
Dim RecordCount
RecordCount = rs.REcordCount
' optionally force immediate persist instead of waiting up to page garbage collection time
'Session.Contents.Persist("CachedRS")
'rs.close
'set rs = nothing
%>
</table><%
Response.Write "<br/>Time to process the page: "
Response.Write Timer - TT
Response.Write "<br/>Time to fetch the session: "

Response.Write TTT
Response.Write "<br/>record count: "
Response.Write RecordCount
Response.WRite "<br/>Persisted XMLDomDocument.6.0"
' This shows that the XMLDOmDocument was persisted to the Session!
Response.Write testPersist.SelectSingleNode("blah").text
%></div>