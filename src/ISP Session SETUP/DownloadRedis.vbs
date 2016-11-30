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

WScript.Echo "We will use " + curDir + "\Redis to install the Redis instance"


If isW64 Then
    version = GetVCRVersion(oFs.BuildPath(oFs.BuildPath( curDir, "Redis"), "redis-server.exe"))
    'vc 12.0.40660
    If version(2) < 40660  Then  
        DownloadToTempAndRun 64
    End If

    version = GetVCRVersion(oFs.BuildPath( oFs.BuildPath( windir, "SysWOW64"), "msvcr120.dll"))    
    If version(2) < 40660  Then  
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
    version = GetVCRVersion(oFs.BuildPath( oFs.BuildPath( windir, "system32"), "msvcr120.dll")) 'vcruntime140
    If version(2) < 40660  Then  
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
