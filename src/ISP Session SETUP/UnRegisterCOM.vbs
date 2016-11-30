Dim oEnv, oWs, isW64, winDir, oFs, version, oShellApp, _
    owinHttp, tempPath, curDir, regsvr32
Set oWs = CreateObject("WScript.Shell")
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

    'unregister COM components
    regsvr32 = oFs.BuildPath( oFs.BuildPath( windir, "SysWOW64"), "regsvr32.exe")
    oWs.Run regsvr32 + " /u /s " + oFs.BuildPath( curDir, "CSession.dll")

    regsvr32 = oFs.BuildPath( oFs.BuildPath( windir, "system32"), "regsvr32.exe")
    oWs.Run regsvr32 + " /u /s " + oFs.BuildPath( curDir, "CSession64.dll")
Else 'just in case
    
    regsvr32 = oFs.BuildPath( oFs.BuildPath( windir, "system32"), "regsvr32.exe")
    oWs.Run regsvr32 + " /u /s " + oFs.BuildPath( curDir, "CSession.dll")
End If

'===============



