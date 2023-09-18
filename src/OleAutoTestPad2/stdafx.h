// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED

#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#define _CRT_SECURE_NO_WARNINGS

#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW

#include "resource.h"
#include <atlbase.h>
HRESULT __stdcall HashData2(const unsigned char* lpSrc, DWORD nSrcLen, unsigned char *lpDest, DWORD nDestLen);

#include "..\..\src\COM\bstrnocache.h"
#include <atlcom.h>
#include <atlctl.h>
#include <adoint.h>

#include "..\..\src\COM\CCOmBSTR2.h"
#include <atlsafe.h>

#include <atlfile.h>
using namespace ATL;

#define SECURITY_WIN32
#include <security.h>
#include <lm.h>
#include <Dsgetdc.h>

#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "Rpcrt4.lib")
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "ADSIid.Lib")
#pragma comment (lib, "Ntdsapi.lib")
