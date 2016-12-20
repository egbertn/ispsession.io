<%@enablesessionstate=false%>
<!--#include file="ispsession.asp"-->
<div class="well">
<p>
In order to get statistics working,
you should make sure that you have configured
the Garbage Collector through Setup to run always.<br />
The ISP Session Garbage Collector runs a query at a minute interval, and
calculates statistics that ISP Session will fetch.
</p>
<%
Function GetExplanation(v)
    Select Case v   
    Case "AvgCompressionRate"
        GetExplanation = "Average Compression, normally equals to 1 but goes up as soon as you have session state size above 8K"
    Case "AvgLongStateLength"
        GetExplanation = "Average length in bytes of session states above 8K."
    Case "AvgSessionDuration"
        GetExplanation = "Average length of current Sessions."
    Case "CountExpired"
        GetExplanation = "Sessions that were expired just before they got garbage collected."
  
    Case "MinLongStateLength"
        GetExplanation = "Minimal size of sessions above 8K."
    Case "MinShortStateLength"
        GetExplanation = "Minimum size of short Session."
    Case "SessionCount"
        GetExplanation = "Number of sessions currently active."
    Case Else
        GetExplanation = v
    End Select
End Function

' Create a copy of the Statistics Collection object
Set Stats = Session.Statistics( "F01FB5D0BCA6CE4C8CC773916F281C32", Session.SessionID)


Response.Write "<table border=""0"" cellspacing=""0""  width=""450"">"
Response.Write "<colgroup align=""left"" size=""150""/>"
Response.Write "<colgroup size=""50"" align=""right""/>"
Response.Write "<colgroup size=""40""/>"
Response.Write "<colgroup size=""250"" align=""left""/>"
Response.Write "<tr style=""background-color:green""><th>Name</th><th>Value</th><th></th><th>Explanation</th></tr>"
For Each V In Stats
	Response.Write "<tr valign=""top""><td>&nbsp;"
	Response.Write v
	Response.Write "</td><td>&nbsp;"
	Response.Write Stats(v)
	Response.Write "</td><td>&nbsp;</td><td>"	
	Response.Write GetExplanation(v)
	Response.Write "</td></tr>"
Next
Response.Write "</table>"

%><br/>

</div>