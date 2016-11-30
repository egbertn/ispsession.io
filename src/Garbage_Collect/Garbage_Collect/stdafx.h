// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once
#define STRICT
// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#define WINVER 0x0502		// Change this to the appropriate value to target other versions of Windows.
#define _WIN32_WINNT 0x0502	// Change this to the appropriate value to target other versions of Windows.
#define _WIN32_WINDOWS 0x0502 // Change this to the appropriate value to target Windows Me or later.
#define _WIN32_IE 0x0800	// Change this to the appropriate value to target other versions of IE.

#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#define _ATL_NO_COM_SUPPORT
#define _ATL_FREE_THREADED
#include "resource.h"
#include "..\..\..\CSession\bstrnocache.h"
#include <atlbase.h>
#include "..\..\..\CSession\CComBSTR2.h"
#include <atlcom.h>
#include "message.h"
#include <atlsecurity.h>

#include <atldbcli.h>
using namespace ATL;
#include "..\..\ADClogModule.h"
extern LoggingModule logModule;