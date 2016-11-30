// CSession.cpp : Implementation of DLL Exports.

#include "stdafx.h"
#include "resource.h"
#include "CSession_i.c"
#include "dllmain.h"

// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void) throw()
{
    return _AtlModule.DllCanUnloadNow();
}

// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) throw()
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void) throw()
{
    // registers object, typelib and all interfaces in typelib
    return _AtlModule.DllRegisterServer();
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void) throw()
{
	return _AtlModule.DllUnregisterServer();
}