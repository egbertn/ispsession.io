<%
Public Session
' you might disable this line if you do 
' not wish to use the application replacement object
On Error Resume Next
' Note, we require Server.CreateObject.
Set Session = Server.CreateObject("NWCTools.CSession")

'Session.Initialize Request, Response, Server, Application
If Err.Number <> 0 Then
	Response.Write "During ISP Session initializiation an error occurred: 0x" 
	Response.Write Hex(Err.Number)
	'too vague
	'Response.Write "<br>COM+ error: "
	Response.Write Err.Description	
	Response.Write "<BR>"
	
	Response.Write "Please check the Application Event Log to see the description for this error"
	Response.End
End If
On Error GoTo 0

Private Sub Session_OnStart()
	'When a user session is new for the webfarm
	'Session("someval") = "some value should be kept" & Timer
	'REsponse.Write("START")

	'You should lock the application normally if you write to it
	' 
	'Session.LiquidCookie = True
	'Session.LiquidCookie = True
	Session.ReEntrance=True
	'otherwise the webapplication will lock too often
	

	
	
End Sub

Private Sub Session_OnExpired()
	'when a user session was expired but
	'the user retries a page after the session already was phased out. See Session.TimeOut

End Sub

If Session.IsExpired Then Session_OnExpired

If Session.IsNew Then Session_OnStart

%>