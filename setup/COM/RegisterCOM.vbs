Dim oEnv, oWs, isW64, winDir, oFs, version, oShellApp, _
    tempPath, curDir, regsvr32, fileTemp
Set oWs = CreateObject("WScript.Shell")

If UserPerms("Admin") = False Then
    WScript.Echo "You must run this script in Adminstrative mode, otherwise, the setup cannot download the MS Visual C++ runtime and install it on your Windows." + vbCrLf + "The VC 14 Runtime is required by the ISP SEssion COM file, CSession.DLL and CSession64.dll"
    WScript.Quit(1)
End If

'We are admin, go on.
Set oFs = CreateObject("Scripting.FileSystemObject")



'============= some environment muke
Set oEnv = oWs.Environment("Process")
curDir = oWs.CurrentDirectory
winDir = oEnv("windir")
tempPath = oEnv("TEMP")
isW64 = (oEnv("PROCESSOR_ARCHITECTURE") = "AMD64")
'=============

'=========== check bitness and download & run VC runtime setup silently
If isW64 Then
    version = GetVCRVersion(oFs.BuildPath( oFs.BuildPath( windir, "system32"), "msvcp140.dll"))
    'vc 14.24.28127.4 = Visual Studio 2019   
    If version(2) < 28127  Then  
        DownloadToTempAndRun 64
    End If  
    version = GetVCRVersion(oFs.BuildPath( oFs.BuildPath( windir, "SysWOW64"), "msvcp140.dll"))    
    If version(2) < 28127  Then  
        DownloadToTempAndRun 32
    End If
    'register COM components
    regsvr32 = oFs.BuildPath( oFs.BuildPath( windir, "SysWOW64"), "regsvr32.exe")
    fileTemp = oFs.BuildPath( curDir, "CSession.dll")
    If Not oFs.FileExists(fileTemp) Then
        WScript.Echo "trying to register ISP SEssion COM DLL, but DLL not found: " + fileTemp
        WScript.Quit(1)
    End If
    oWs.Run regsvr32 + " /s " + fileTemp

    regsvr32 = oFs.BuildPath( oFs.BuildPath( windir, "system32"), "regsvr32.exe")
    fileTemp = oFs.BuildPath( curDir, "CSession64.dll")
    If Not oFs.FileExists(fileTemp) Then
        WScript.Echo "trying to register ISP SEssion COM DLL, but DLL not found: " + fileTemp
        WScript.Quit(1)
    End If
    oWs.Run regsvr32 + " /s " + fileTemp
Else 'just in case
    version = GetVCRVersion(oFs.BuildPath( oFs.BuildPath( windir, "system32"), "msvcp140.dll")) 'vcruntime140
    If version(2) < 28127  Then  
        DownloadToTempAndRun 32
    End If

    regsvr32 = oFs.BuildPath( oFs.BuildPath( windir, "system32"), "regsvr32.exe")
    fileTemp = oFs.BuildPath( curDir, "CSession.dll")
    If Not oFs.FileExists(fileTemp) Then
        WScript.Echo "trying to register ISP SEssion COM DLL, but DLL not found: " + fileTemp
        WScript.Quit(1)
    End If
    oWs.Run regsvr32 + " /s " + fileTemp
End If

WScript.Echo "ISP Session 64 bit and 32 bit should be OK now!"
'===============


Sub DownloadToTempAndRun(bits)
    Dim vArray , pos, fName, stream, cmd, owinHttp
    Set owinHttp = CreateObject("WinHttp.WinHttpRequest.5.1")
    Set stream = CreateObject("ADODB.Stream")
    stream.Type = 1 'binary
    'checked at June 24, 2018: these files exist
    ' english runtime

    vArray = Array("https://aka.ms/vs/16/release/vc_redist.x64.exe", _
        "https://aka.ms/vs/16/release/vc_redist.x86.exe")
    
    If Bits = 32 Then
        pos = 1       
    Else
        pos = 0
    End If
    fName = oFs.GetFileName(vArray(pos))
    owinHttp.Open "GET", vArray(pos), False 
    WScript.Echo "downloading VC runtime, will take a a minute" + vArray(pos)
    owinHttp.Send()
    stream.Open
    stream.Write  owinHttp.ResponseBody    
    cmd = oFs.BuildPath(tempPath, fName)
    stream.SaveToFile cmd,2 'adSaveCreateOverWrite
    WScript.Echo "running VC runtime setup, will take a minute: " & cmd
    oWs.Run cmd + " /quiet /norestart" ,0,true
    'delete used muke
    On Error Resume Next
    oFs.DeleteFile cmd
    On Error Goto 0
End Sub
Function GetVCRVersion(path)
    If Not oFs.FileExists(path) Then
        GetVCRVersion = Array(0,0,0,0)
	Exit Function
    End If
    Dim vArray , v
    v=oFs.GetFileVersion(path)    
    vArray= Split(v, ".",-1,vbBinaryCompare)
    Dim cx, ct 
    ct = UBound(vArray)
    For cx = 0 to ct 
        vArray(cx)=CLng(vArray(cx))
    Next
    GetVCRVersion = vArray
End Function

Function UserPerms (PermissionQuery)          
 UserPerms = False  ' False unless proven otherwise           
 Dim CheckFor, CmdToRun         

 Select Case Ucase(PermissionQuery)           
 'Setup aliases here           
 Case "ELEVATED"           
   CheckFor =  "S-1-16-12288"           
 Case "ADMIN", "ADMINISTRATOR"
   CheckFor =  "S-1-5-32-544"         
 Case Else                  
   CheckFor = PermissionQuery                  
 End Select           

 CmdToRun = "%comspec% /c whoami /all | findstr /I /C:""" & CheckFor & """"  

 Dim  returnValue        
 
 returnValue = oWs.Run(CmdToRun, 0, true)     
 If returnValue = 0 Then UserPerms = True                   
End Function

Function UACTurnedOn ()
 On Error Resume Next

    UACTurnedOn = (oWs.RegRead("HKLM\Software\Microsoft\Windows\CurrentVersion\Policies\System\EnableLUA") = 0)
 
End Function

