
#pragma once


#include <wtypes.h>
//#ifndef __ATLBASE_H__
//	#error the bstrnocache.h replacement requires to be after <atlbase.h>
//#endif
/* file brnocache.h	
 * Made by e. Nierop, Nierop Webconsultancy, 2005
 * include before atlbase.h!
 */

// macro autocorrects the location of the length prefix on 64 bit systems
// on 32bit systems, a BSTR is 4 bytes prefixed while on 64bit systems, it is 8 byte prefixed
// however, the first 4 bytes, seem unused yet
//#define sizePtrCorrection sizeof(UINT_PTR) == sizeof(unsigned int)  ? 0 : 1
//
//
//#define MAXBSTRLEN unsigned int(~0) 
//#define m_align 16L
//#define MEMALIGN_16(cb) cb % m_align != 0 ? ((cb / m_align) + 1) * m_align : cb

void __stdcall FreeString(wchar_t* *bstr);


/*
 * We use wide to utf8 encoding
 * the buffer and space to create for this conversion should not be created for each
 * string, especially on multi-proc machines, creates CPU locks only to allocate space
 * therefore we use a dynamic buffer that grows as needed and
 * gets freed after the object runs out of the object pool (COM+)
 */

//unsigned int __stdcall SysStringByteLen2(__in_opt wchar_t*);
//int __stdcall SysReAllocStringByteLen2(__in_z_opt wchar_t* * bstr, __in_z_opt const char* input, __in unsigned int cch);

long __stdcall AllocPSTR(char* *lpMulti, SIZE_T lSize, SIZE_T *oldSize);
void __stdcall FreeMulti(char* *lpMulti);


//#ifndef NOSTRINGREPLACE
//
//#define SysReAllocStringByteLen SysReAllocStringByteLen2
//#endif
//
//
