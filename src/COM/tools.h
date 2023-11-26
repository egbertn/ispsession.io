// tools.h
// April, 26 2009 added because of a code clean up

#pragma once
#include <sstream>
#include <vector>
#include <string>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <DSRole.h>

std::wstring ltrim(const std::wstring &s);
std::wstring rtrim(const std::wstring& s);
std::wstring trim(const std::wstring& s);

std::string& __stdcall ltrim(std::string &s);
std::string& __stdcall rtrim(std::string &s);
std::string& __stdcall trim(std::string &s);

void split(const std::wstring &s, wchar_t delim, std::vector<std::wstring> &elems, int maxCount = 0);
std::string str_toupper(const std::string& s);
bool is_bool(const std::wstring& s);
bool is_number(const std::wstring& s);
std::wstring get_environment_variable(const std::wstring& envName);
BSTR __stdcall GetNetBIOSName(bool GiveDnsName) noexcept;
BSTR __stdcall GetDCDomain()  noexcept;
void __stdcall LogMessage(const DWORD messtype, PCWSTR msg[], int els) noexcept;
//void __stdcall ReportComError(const DWORD win32err, PCWSTR msg=NULL);
//void __stdcall ReportComError2(const HRESULT hr, PCWSTR msg = NULL);
BSTR __stdcall GetModulePath() noexcept;
// reads registry for settings
// optional lngTimeOutSetting 
//STDMETHODIMP ReadDllConfig(/*out*/ BSTR * strConstruct, LONG *lTimeOutSetting,
//							  /*BOOL *EnableCompression,  */
//							/*  BOOL *hashSessionID =0,*/ int *EnableLogging=0, GUID * license=NULL, BSTR* licensedFor=NULL);

bool is_hex_notation(std::wstring const& s);
// set byte length string to hex, note to set the correct length!
bool setstring(GUID* addrGUID,const std::wstring& strCookiePtr) noexcept;

std::wstring sHexFromBt(GUID* psa) noexcept;
// creates a multibyte string from any point in memory
// assumes you own the memory. If bytes == NULL returns zerood string
std::string __stdcall HexStringFromMemory(PBYTE bytes, int len) noexcept;
std::wstring __stdcall s2ws(const std::string& str);
std::string __stdcall ws2s(const std::wstring& wstr);
DATE __stdcall Now() noexcept;

STDMETHODIMP OleSaveToStream2(IPersistStreamInit *pPersistStmInit, IStream *pStm) noexcept;
STDMETHODIMP OleLoadFromStream2(IStream *pStm, REFIID iidInterface, void** ppvObj) noexcept;
bool __stdcall LicentieCheck(GUID *licence, BSTR licenseContents) noexcept;
STDMETHODIMP ISequentialStream_Copy( ISequentialStream* iface, ISequentialStream* pstm, ULARGE_INTEGER cb, ULARGE_INTEGER* pcbRead, ULARGE_INTEGER* pcbWritten) noexcept;
HRESULT __stdcall HashData2(const unsigned char* lpSrc, DWORD nSrcLen, unsigned char *lpDest, DWORD nDestLen) noexcept;
unsigned long get_hashcode(const std::wstring& s);
