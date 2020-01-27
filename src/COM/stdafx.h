// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once
#include "message.h"
//#define Demo
// defines for ISP Session tiny edition
//TODO: GLOG implementation
//#define EXPIREAT ( 43889) // ie: 28/feb/2020
#pragma warning( disable : 4840 )

#define MAXINSTANCES 1000
#define UNICODE
#define _UNICODE

#include <winsdkver.h>
#define _ATL_FREE_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _CRT_RAND_S
// turn on to get full tracing in the Debug window

#include <atlbase.h> 
#include "tools.h"
#include "CComBStr2.h"
#include <atlcom.h>

//#include <atlsync.h>


using namespace ATL;
#define NO_BOOST
#include "redis3m.hpp"
//#include <iostream> 
#include <cctype>
#include <clocale>
using namespace redis3m;



//#ifdef DEBUG
//#pragma comment(lib, "netapi32.lib") // for DsGetDcNameW and such


#include <asptlb.h>         // Active Server Pages Definitions
#include <atlsecurity.h>
#include <atlsafe.h>

constexpr auto ONEMINUTE = 0.00069444444444444F;

#define CHECKHR if (FAILED(hr)) goto error;

#define RELEASE(lpUnk)  if (lpUnk != nullptr) { lpUnk->Release(); lpUnk = nullptr; } 
#define ERROR_READONLY  L"Session is set readonly"


#define _OFF_T_DEFINED

#include "ADClogModule.h"

extern LoggingModule logModule;
struct ARRAY_DESCRIPTOR
{
	VARTYPE type;
	LONG ElemSize;
	LONG Dims;
};
