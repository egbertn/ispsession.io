// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once
#include "message.h"
//#define Demo
// defines for ISP Session tiny edition

//#define EXPIREAT ( 43342) // ie: 30/08/2018
#define MAXINSTANCES 1000
#define STRICT
#define UNICODE
#define _UNICODE
#define NOCOMM 
//support Windows Server 2003 with SP1,
#define WINVER _WIN32_WINNT_WIN8	
#define _WIN32_WINNT _WIN32_WINNT_WIN8
 #define INCL_WINSOCK_API_TYPEDEFS 0
#define WIN32_MEAN_AND_LEAN
#define VC_EXTRALEAN 
#define _ATL_FREE_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _CRT_RAND_S
// turn on to get full tracing in the Debug window

#include <atlbase.h> 
#include "CComBStr2.h"
#include <atlcom.h>
//#include <atlsync.h>


using namespace ATL;
#define NO_BOOST
#include "include/redis3m/redis3m.hpp"
#include <iostream> 
#include <cctype>
#include <clocale>
using namespace redis3m;



//#ifdef DEBUG
#pragma comment(lib, "netapi32.lib") // for DsGetDcNameW and such


#include <asptlb.h>         // Active Server Pages Definitions
#include <atlsecurity.h>
#include <atlsafe.h>

#define ONEMINUTE  0.00069444444444444F

#define CHECKHR2 if (FAILED(hr)) {/* ReportComError2(hr, location);*/ goto error; }
#define CHECKHR if (FAILED(hr)) goto error;

#define RELEASE(lpUnk)  if (lpUnk != nullptr) { lpUnk->Release(); lpUnk = nullptr; } 
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
struct ARRAY_DESCRIPTOR
{
	VARTYPE type;
	LONG ElemSize;
	LONG Dims;
};
