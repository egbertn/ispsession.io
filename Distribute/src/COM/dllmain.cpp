#include "stdafx.h"
#include "resource.h"
#include "CSession_i.c"
#include "dllmain.h"

ISPCSessionModule _AtlModule;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;
	return _AtlModule.DllMain(dwReason, lpReserved); 
}
