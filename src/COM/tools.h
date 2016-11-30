// tools.h
// April, 26 2009 added because of a code clean up

#pragma once

BSTR __stdcall GetNetBIOSName(bool GiveDnsName);
void __stdcall LogMessage(const DWORD messtype, PCWSTR msg[], int els);
void __stdcall ReportComError(const DWORD win32err, PCWSTR msg=NULL);
void __stdcall ReportComError2(const HRESULT hr, PCWSTR msg = NULL);
BSTR __stdcall GetModulePath();
BSTR __stdcall FileStripFile(const wchar_t* pathname);
BSTR __stdcall FileStripPath(const wchar_t* pathname);
// reads registry for settings
// optional lngTimeOutSetting 
STDMETHODIMP ReadDllConfig(/*out*/ BSTR * strConstruct, LONG *lTimeOutSetting,
							  /*BOOL *EnableCompression,  */
							/*  BOOL *hashSessionID =0,*/ int *EnableLogging=0, GUID * license=NULL, BSTR* licensedFor=NULL);

BOOL __stdcall setstring(const PUCHAR addrGUID,const BSTR strCookiePtr);
BOOL __stdcall IsValidHex(const BSTR Cookie);
BSTR __stdcall sHexFromBt(const PUCHAR btBytes, LONG cb = sizeof(GUID), bool prePend=true);
// set byte length string to hex, note to set the correct length!
void __stdcall sHexFromBt2(const PUCHAR btBytes, LONG cb, PBYTE memBuf) ;
void __stdcall sHexFromBt(GUID* psa, BSTR *sRet);
// creates a multibyte string from any point in memory
// assumes you own the memory. If bytes == NULL returns zerood string
std::string __stdcall HexStringFromMemory(PBYTE bytes, int len);
std::wstring __stdcall s2ws(const std::string& str);
std::string __stdcall ws2s(const std::wstring& wstr);
DATE __stdcall Now();
BSTR __stdcall FormatDBTimeStamp(const DATE  ts);
bool __stdcall FileExists(const wchar_t* file);
BSTR __stdcall FileExtension(const wchar_t *file);