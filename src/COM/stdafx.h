// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once
#include "message.h"
#define Demo
// defines for ISP Session tiny edition
//#define AppKeyNULL
#define EXPIREAT (42765L) // ie: 30/01/2017
#define MAXINSTANCES 1000
#define STRICT
#define UNICODE
#define _UNICODE
#define NOCOMM 
//support Windows Server 2003 with SP1,
#define WINVER _WIN32_WINNT_WIN7	
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#define WIN32_MEAN_AND_LEAN
#define VC_EXTRALEAN 
#define _ATL_FREE_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

// turn on to get full tracing in the Debug window

#include <atlbase.h> 
#include "CComBStr2.h"
#include <atlcom.h>
//#include <atlsync.h>


using namespace ATL;
#define NO_BOOST
#include "include/redis3m/redis3m.hpp"
#include <iostream>
using namespace redis3m;


//#pragma comment(lib, "Ws2_32.lib")
//#ifdef DEBUG
#pragma comment(lib, "netapi32.lib") // for DsGetDcNameW and such


#ifdef WIN64
#if _DEBUG
#pragma comment(lib, "Win32_Interop64D.lib")
#pragma comment(lib, "hiredis64D.lib")
//#pragma comment(lib, "redis3m64D.lib")

#else
#pragma comment(lib, "Win32_Interop64.lib")
#pragma comment(lib, "hiredis64.lib")
//#pragma comment(lib, "redis3m64.lib")
#endif
//#pragma comment(lib, "zlib64.lib")
#else
#if _DEBUG
#pragma comment(lib, "Win32_InteropD.lib")
#pragma comment(lib, "hiredisD.lib")
//#pragma comment(lib, "redis3mD.lib")
#else
#pragma comment(lib, "Win32_Interop.lib")
#pragma comment(lib, "hiredis.lib")
//#pragma comment(lib, "redis3m.lib")
#endif
//	#pragma comment(lib, "zlib.lib")
#endif

#include <asptlb.h>         // Active Server Pages Definitions
//#pragma comment(lib, "Secur32.lib")
//#define SECURITY_WIN32
//#include <security.h>
#include <atlsecurity.h>
#include <atlsafe.h>
#include <lm.h>
#include <Dsgetdc.h>
#define ONEMINUTE  0.00069444444444444F

#define CHECKHR2 if (FAILED(hr)) { ReportComError2(hr, location); goto error; }
#define CHECKHR if (FAILED(hr)) goto error;

#define RELEASE(lpUnk)  if (lpUnk != NULL) { lpUnk->Release(); lpUnk = NULL; } 
//#define ZERO(any) memset(&any, 0, sizeof(any));
#define ERROR_READONLY  L"Session is set readonly"
#define ERROR_INPUT  L"Expecting string input"



#define FreeVariant(var) ::VariantClear(var); ZeroMemory(var, sizeof(VARIANT));
#define CHECK_ERR(err, msg) { \
    if (err != Z_OK) { \
        AtlTrace(L"%s error: %d\n", msg, err); \
    } \
}

#include "ADClogModule.h"

extern LoggingModule logModule;

