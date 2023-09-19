// Tools Copyright ADC Cure 2017-2018
#include "stdafx.h"
#include <string>
#include "tools.h"
#include <LM.h>
#include "CStream.h"
#include "ConfigurationManager.h"
//#include <sys/stat.h>
#include <filesystem>
using namespace std::experimental;
std::wstring& __stdcall ltrim(std::wstring &s) 
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) {return !std::isspace(c); }));
	return s;
}
std::string& __stdcall ltrim(std::string &s) 
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) {return !std::isspace(c); }));
    return s;
}
// trim from end
std::wstring& __stdcall rtrim(std::wstring &s) 
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int c) {return !std::isspace(c); }).base(), s.end());
	return s;
}
std::string& __stdcall rtrim(std::string &s) 
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int c) {return !std::isspace(c); }).base(), s.end());
	return s;
}

// trim from both ends
std::wstring& __stdcall trim(std::wstring &s) 
{
	return ltrim(rtrim(s));
}
std::string& __stdcall trim(std::string &s) 
{
	return ltrim(rtrim(s));
}

std::string str_toupper(std::string s) 
{

	std::transform(s.begin(), s.end(), s.begin(),
		// [](char c){ return std::toupper(c); }          // wrong
		[](unsigned char c){ return (unsigned char)std::toupper(c); } // correct
	);
	return s;

}
bool is_number(const std::wstring& s) {
	return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}
std::wstring str_tolower(std::wstring s)
{
	std::transform(s.begin(), s.end(), s.begin(),
		// [](char c){ return std::toupper(c); }          // wrong
		[](wchar_t c) { return (wchar_t)std::tolower(c); } // correct
	);
	return s;
}

bool is_bool(const std::wstring& s) {
	if (s.empty())
		return false;

	auto lower = str_tolower(s);
	return lower.compare(L"true") == 0 || lower.compare(L"false") == 0;
}
//void split(const std::string &s, char delim, std::vector<std::string> &elems, int maxCount s= 0)
//{
// std::stringstream ss;
// ss.str(s);
// std::string item;
// int maxC = 0;
// while ((std::getline(ss, item, delim) && (maxCount == 0 || (maxC++<maxCount)))) {
//	 elems.push_back(trim(item));
// }
//}
void split(const std::wstring &s, wchar_t delim, std::vector<std::wstring> &elems, int maxCount)
{
	if (s.empty())
	{
		return;
	}
	auto n = std::count(s.begin(), s.end(), delim);
	if (maxCount > 0 && maxCount > n)
	{
		n = maxCount;
	}
	//elems.resize(n);
	if (n == 0)
	{
		elems.push_back(s);
		return;
	}
	size_t pos = 0;
	for (int x = 0; x <= n; x++)
	{
		auto newPos = s.find(delim, pos);
		elems.push_back(trim(newPos != std::string::npos && (x < maxCount || maxCount == 0) ? s.substr(pos, newPos - pos) : s.substr(pos)));
		pos = newPos + 1;
	}

}

bool __stdcall FileExists(const wchar_t * FileName) noexcept
{
	CComBSTR bstrFileName(FileName);
	auto ansi = bstrFileName.ToString();
	return std::filesystem::exists(ansi);
}
//attach to it
// e.g. searching on web.Config will return .Config
BSTR __stdcall FileExtension(const CComBSTR& FileName) noexcept
{
	int lastPos = FileName.LastIndexOf('.');
	if (lastPos >= 0)
		return FileName.Substring(lastPos);
	return NULL;
}
//strips path and extension e.g. 'c:\\dir\\file.ext' becomes 'file'
// returns null if an error ocurred
BSTR __stdcall FileStripPath(const wchar_t* pathname) noexcept
{	
	wchar_t drive[_MAX_DRIVE];
	wchar_t dir[_MAX_DIR ];
	wchar_t fname[_MAX_FNAME];
	wchar_t ext[_MAX_EXT];
	errno_t err =
		_wsplitpath_s(pathname, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
	if (err != 0)
	{
		return NULL;
	}
	CComBSTR fileName(fname);
	return fileName.Detach();
}
//strips file and extension e.g. 'c:\\dir\\file.ext' becomes 'c:\\dir'
BSTR __stdcall FileStripFile(const wchar_t* pathname) noexcept
{
	wchar_t drive[_MAX_DRIVE];
	wchar_t dir[_MAX_DIR];
	wchar_t fname[_MAX_FNAME];
	wchar_t ext[_MAX_EXT];
	errno_t err =
		_wsplitpath_s(pathname, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
	if (err != 0)
	{
		return NULL;
	}
	CComBSTR path(drive);	
	path.Append(dir);
	if (path.EndsWith(L"\\"))
	{
		path.TrimEnd(L"\\");
	}
	//	fileName.Append(ext);
	return path.Detach();
}
BSTR __stdcall GetModulePath() noexcept
{
	CComBSTR licenseFile(MAX_PATH);
	
	HINSTANCE hinst = _AtlBaseModule.GetModuleInstance();
	DWORD bufLen = ::GetModuleFileNameW(hinst, licenseFile, MAX_PATH);
	if (bufLen > 0)
		//strip right length
		licenseFile.SetLength(bufLen);
	if (bufLen > MAX_PATH)
		::GetModuleFileNameW(hinst, licenseFile, bufLen);
	CComBSTR bExt;
	bExt.Attach(FileExtension(licenseFile));
	CComBSTR ext(bExt);
	//remove eventual 64.dll or 64.exe to .dll or .exe
	bExt.Insert(0, L"64");
	licenseFile.Replace(bExt, ext);

	return licenseFile.Detach();
}
BSTR __stdcall GetDCDomain() noexcept
{
	DSROLE_PRIMARY_DOMAIN_INFO_BASIC* info;
	DWORD dw;

	dw = DsRoleGetPrimaryDomainInformation(NULL,
		DsRolePrimaryDomainInfoBasic,
		(PBYTE*)&info);
	if (dw == ERROR_SUCCESS)
	{
		BSTR retVal = SysAllocString(info->DomainNameDns);
		DsRoleFreeMemory(info);
		return retVal;
	}
	return nullptr;
}
//
BSTR __stdcall GetNetBIOSName(bool GiveDnsName = false) noexcept
{
	DWORD realSize = MAX_COMPUTERNAME_LENGTH + 1;
	CComBSTR retVal(realSize);
	if (::GetComputerName(retVal.m_str, &realSize) != ERROR_SUCCESS)
	{
		retVal.SetLength(realSize);
		return retVal.Detach();
	}
	return nullptr;
}

void __stdcall LogMessage(const DWORD messtype, PCWSTR msg[] = NULL, int els = 0) noexcept
{
	
	HANDLE report = ::RegisterEventSourceW(NULL, L"ISPSession");	
	if (report != NULL)
	{		
		if (::ReportEventW(report, EVENTLOG_WARNING_TYPE, 0, messtype, NULL, (WORD)els, 0, msg, NULL) == FALSE)
		{			
			logModule.Write(L"ReportEvent failed with: %d", GetLastError());		
		}		
	}
	else
	{
		logModule.Write(L"RegisterEventSource failed %d", GetLastError());
	}

	::DeregisterEventSource(report);		
}
//void __stdcall ReportComError(const DWORD win32err, PCWSTR msg) throw()
//{	
//	PWSTR berr = NULL;
//	DWORD mlen = ::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, win32err,0, (LPWSTR)&berr, 0, NULL);
//	if (mlen ==0)
//	{
//		CComBSTR num; 
//		CComBSTR err(L"An unknown error occurred code: ");
//		num.AssignLongHex(win32err);
//		err.Append(num);		
//		PCWSTR amsg[] = {msg, err};
//		LogMessage(MSG_ERROR, amsg, 2);
//	}
//	else
//	{
//		PCWSTR amsg[] = {msg, berr};
//		CComBSTR temp(berr);		
//		temp.Append(':');
//		temp.Append(msg);
//		
//		LogMessage(MSG_ERROR, amsg); 
//		::GlobalFree(berr);
//	}
//	
//}
//
//void __stdcall ReportComError2(const HRESULT hr, PCWSTR msg) throw()
//{
//	
//	PCWSTR amsg[] = {msg};
//	int nr = 1;
//	LogMessage(MSG_ERROR, amsg, 1);
//}

DATE __stdcall Now() noexcept
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	DATE nu =0;
	SystemTimeToVariantTime(&st, &nu);	
	return nu;
}
//select convert(varchar(23), getdate(), 126) = ISO8601
// returns: eg '2009-04-22T13:11:36.377'
BSTR __stdcall FormatDBTimeStamp(const DATE ts) noexcept
{
	if (ts == NULL) return NULL;
	SYSTEMTIME st = { 0 };
	::VariantTimeToSystemTime(ts, &st);
	CComBSTR sTS, fix;
	fix.Format(L"%.3d", st.wMilliseconds);
	if (st.wMilliseconds > 999)
	{
		fix.SetLength(3);
	}	
	sTS.Format(L"'%.4d-%.2d-%.2dT%.2d:%.2d:%.2d.%s'", st.wYear, 
											st.wMonth, 
											st.wDay,
											st.wHour, 
											st.wMinute, 
											st.wSecond, 
											fix);
	return sTS.Detach();

}
STDMETHODIMP OleSaveToStream2(IPersistStreamInit *pPersistStmInit, IStream *pStm) noexcept
{
	if (pPersistStmInit == nullptr || pStm == nullptr)
		return E_INVALIDARG;
	CLSID clsd;

	HRESULT hr = pPersistStmInit->GetClassID(&clsd);
	
	if (hr == S_OK)
		pStm->Write(&clsd, sizeof(GUID), nullptr);
	if (hr == S_OK)
		hr = pPersistStmInit->Save(pStm, TRUE);
	return hr;
}

STDMETHODIMP OleLoadFromStream2(IStream *pStm, REFIID iidInterface, void** ppvObj) noexcept
{
	if (pStm == nullptr || ppvObj == nullptr) return E_INVALIDARG;

	CLSID clsd;
	CComPtr<IPersistStreamInit> pPersist;
	HRESULT hr = pStm->Read( &clsd, sizeof(GUID), nullptr);
	if (hr == S_OK)
	{
		hr = pPersist.CoCreateInstance(clsd, nullptr, CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER | CLSCTX_NO_CODE_DOWNLOAD);
		if (hr == S_OK)
		{
			hr = pPersist->Load(pStm);
			if (hr == S_OK)
			{
				hr = pPersist->QueryInterface(iidInterface, ppvObj);
			}

		}
	}
	return hr;
}

//// optional m_lngTimeOutSetting and 
//// strConstruct [out]
//STDMETHODIMP ReadDllConfig(BSTR * strConstruct, LONG *lTimeOutSetting, /*BOOL *hashSessionID,*/ int *enableLogging, GUID * license, BSTR* licenseContents) throw()
//{
//
//	ConfigurationManager config;
//	
//	CComBSTR key, val;
//	
//	key = L"ispsession_io:DataSource";
//	*strConstruct = config.AppSettings(key);
//	if (*strConstruct == NULL)
//	{
//		OutputDebugStringA("CSession.dll.Config not found, therefore exiting\r\n");
//		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
//	}
//	key.SetLength(512);
//	auto stored = ::GetEnvironmentVariableW(*strConstruct, key, 512);
//	if (stored > 0)
//	{
//		SysReAllocStringLen(strConstruct, key, stored);
//	}
//	if (lTimeOutSetting != nullptr)
//	{
//		key = L"ispsession_io:SessionTimeout";
//		val.Attach(config.AppSettings(key, L"20"));
//		if (val.Length() > 0 && val.IsNumeric())
//			*lTimeOutSetting = val.ToLong();
//	}
//	/*if (hashSessionID!= NULL)
//	{
//		key = L"HASH_SESSIONID";
//		val.Attach(config.AppSettings(key, L"false"));
//		if (val.Length() > 0 && val.IsBool())
//			*hashSessionID = val.ToBool()  == VARIANT_TRUE ? TRUE : FALSE;
//	}*/
//	
//	/*if (EnableCompression != NULL)
//	{
//		key = L"UseCompression";
//		val.Attach(config.AppSettings(key, L"1"));
//		*EnableCompression = val.ToLong();
//	}*/
//
//	if (enableLogging != NULL)
//	{
//		key = L"ispsession_io:EnableLogging";
//		val.Attach(config.AppSettings(key, L"0"));
//		if (val.Length() > 0 && val.IsNumeric())
//			*enableLogging= val.ToLong();
//#ifdef DEBUG
//		AtlTrace(L"Logging enabled %d\r\n", *enableLogging);
//#endif
//	}
//	if (license != NULL)
//	{
//		key = L"ispsession_io:License";
//
//		val.Attach(config.AppSettings(key, L""));
//		if (val.Length() != 0)
//		{
//			setstring((const PUCHAR)license, val.m_str);
//		}   
//	}
//	if (licenseContents != NULL)
//	{
//		key = L"ispsession_io:Csession.LIC";
//		val.Attach(config.AppSettings(key, L""));
//		if (val.Length() != 0)
//		{
//			*licenseContents = val.Detach();
//		}
//	}
//	return S_OK;
//}
wstring __stdcall s2ws(const std::string& str)
{
    auto count = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), nullptr, 0);
    wstring ret;
    ret.resize(count);
    ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), ret.data(), count);
    return std::move(ret);

}

string __stdcall ws2s(const std::wstring& wstr)
{

    auto count = ::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.length()), nullptr, 0, nullptr, nullptr);
    string ret;
    ret.resize(count);
    ::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.length()), ret.data(), count, nullptr, nullptr);
    return std::move(ret);

}

//assumes that PBYTE is valid memory! 
//ensures that a return is given, e.g. NULL bytes will return an empty hexadecimal range of 'len' length
std::string __stdcall HexStringFromMemory(PBYTE bytes, int len) noexcept
{
	if (bytes == NULL || len <= 0)
	{
		return NULL;
	}
	if (bytes == NULL)
	{
		return std::move(std::string((size_t)len * 2, '0'));
	}
	std::string retVal;
	retVal.reserve(len * 2);	
	for (INT cx = 0; cx < len; cx++)
	{
		auto btByte = bytes[cx];
		auto btByte2 = btByte & 15;
		btByte = btByte / 16;
		if (btByte > 9)
			btByte += 55;
		else
			btByte |= 48;

		if (btByte2 > 9)
			btByte2 += 55;
		else
			btByte2 |= 48;
		retVal.push_back((char)btByte);
		retVal.push_back((char)btByte2);
	};
	return std::move(retVal);
}
wstring sHexFromBt(GUID* psa) noexcept
{
	auto pvdata = (PBYTE)psa;
	wstring sRet;
	sRet.reserve(sizeof(GUID) * 2);

	for (int cx = 0, bx = 0; cx < sizeof(GUID) * 2; bx++,cx+=2)
	{
		wchar_t btByte = pvdata[bx];
		wchar_t btByte2 = btByte & 15;
		btByte = btByte / 16;
		if (btByte > 9)
			btByte += 55;
		else
			btByte |= 48;

		if (btByte2 > 9)
			btByte2 += 55;
		else
			btByte2 |= 48;
		//ZeroMemory(*sRet, sizeof(GUID) *2);
		sRet.push_back((char)btByte);
		sRet.push_back((char)btByte2);

	}
	return std::move(sRet);
}


// Converts Hex GUID in Parameter strCookiePtr to addrGUID
bool is_hex_notation(std::wstring const& s)
{
	return  s.size() == 32
		&& s.find_first_not_of(L"0123456789abcdefABCDEF", 0) == std::wstring::npos;
}

bool setstring(GUID* addrGUID,wstring& strCookiePtr) noexcept
{
	if (strCookiePtr.empty())
	{
		return false;
	}
	LONG cx = 0;
	UCHAR btByte = 0, btByte2 = 0;
	auto sdata = (PUCHAR)strCookiePtr.c_str();
	UINT sLen = strCookiePtr.length();
	BOOL retval = TRUE;
	if (sLen == (sizeof(GUID) * 2))
	{
		int bx = sizeof(GUID) - 1;
		for (cx = (sLen * 2) - 4; cx >= 0; cx -= 4, bx--)
		{
			btByte = sdata[cx];
			btByte2 = sdata[cx + 2];

			if ((btByte >= 65) && (btByte <= 70)) //& correct
				btByte -= 55;
			else if ((btByte >= 48) && (btByte <= 57))
				btByte ^= 48;
			//'needless test but you never now if somebody converts to lowercase
			else if ((btByte >= 97) && (btByte <= 102))
				btByte -= 87;
			else{ retval = FALSE; break; }

			if ((btByte2 >= 65) && (btByte2 <= 70))
				btByte2 -= 55;
			else if ((btByte2 >= 48) && (btByte2 <= 57))
				btByte2 ^= 48;  //' cut the two leftmost bits 110000
			//'needless test but you never now if somebody converts to lowercase
			else if ((btByte2 >= 97) && (btByte2 <= 102))
				btByte2 -= 87;
			else
			{
				retval = FALSE; break;
			}
			((PUCHAR)addrGUID)[bx] = (btByte * 16) | btByte2;
		}

	}
	else
		retval = FALSE;

	return  retval;
	
}

STDMETHODIMP ISequentialStream_Copy(_In_ ISequentialStream* iface, _In_ ISequentialStream* pstm, _In_ ULARGE_INTEGER cb, _Inout_opt_ ULARGE_INTEGER* pcbRead, _Inout_opt_ ULARGE_INTEGER* pcbWritten) noexcept
{
	HRESULT        hr = S_OK;
	BYTE           tmpBuffer[128];
	ULONG          bytesRead, bytesWritten, copySize;
	ULARGE_INTEGER totalBytesRead;
	ULARGE_INTEGER totalBytesWritten;

	AtlTrace(L"(%p, %p, %d, %p, %p)\n", iface, pstm,
		cb.u.LowPart, pcbRead, pcbWritten);

	if (iface == nullptr || pstm == nullptr)
		return STG_E_INVALIDPOINTER;

	totalBytesRead.QuadPart = 0;
	totalBytesWritten.QuadPart = 0;

	while (cb.QuadPart > 0 && hr == S_OK)
	{
		if (cb.QuadPart >= sizeof(tmpBuffer))
			copySize = sizeof(tmpBuffer);
		else
			copySize = cb.u.LowPart;

		hr = iface->Read( tmpBuffer, copySize, &bytesRead);
		if (FAILED(hr))
			break;

		totalBytesRead.QuadPart += bytesRead;

		if (bytesRead)
		{
			hr = pstm->Write( tmpBuffer, bytesRead, &bytesWritten);
			if (FAILED(hr))
				break;

			totalBytesWritten.QuadPart += bytesWritten;
		}

		if (bytesRead != copySize)
			cb.QuadPart = 0;
		else
			cb.QuadPart -= bytesRead;
	}

	if (pcbRead) pcbRead->QuadPart = totalBytesRead.QuadPart;
	if (pcbWritten) pcbWritten->QuadPart = totalBytesWritten.QuadPart;

	return hr;

}



static const unsigned char HashDataLookup[256] = {
				  0x01, 0x0E, 0x6E, 0x19, 0x61, 0xAE, 0x84, 0x77, 0x8A, 0xAA, 0x7D, 0x76, 0x1B,
				  0xE9, 0x8C, 0x33, 0x57, 0xC5, 0xB1, 0x6B, 0xEA, 0xA9, 0x38, 0x44, 0x1E, 0x07,
				  0xAD, 0x49, 0xBC, 0x28, 0x24, 0x41, 0x31, 0xD5, 0x68, 0xBE, 0x39, 0xD3, 0x94,
				  0xDF, 0x30, 0x73, 0x0F, 0x02, 0x43, 0xBA, 0xD2, 0x1C, 0x0C, 0xB5, 0x67, 0x46,
				  0x16, 0x3A, 0x4B, 0x4E, 0xB7, 0xA7, 0xEE, 0x9D, 0x7C, 0x93, 0xAC, 0x90, 0xB0,
				  0xA1, 0x8D, 0x56, 0x3C, 0x42, 0x80, 0x53, 0x9C, 0xF1, 0x4F, 0x2E, 0xA8, 0xC6,
				  0x29, 0xFE, 0xB2, 0x55, 0xFD, 0xED, 0xFA, 0x9A, 0x85, 0x58, 0x23, 0xCE, 0x5F,
				  0x74, 0xFC, 0xC0, 0x36, 0xDD, 0x66, 0xDA, 0xFF, 0xF0, 0x52, 0x6A, 0x9E, 0xC9,
				  0x3D, 0x03, 0x59, 0x09, 0x2A, 0x9B, 0x9F, 0x5D, 0xA6, 0x50, 0x32, 0x22, 0xAF,
				  0xC3, 0x64, 0x63, 0x1A, 0x96, 0x10, 0x91, 0x04, 0x21, 0x08, 0xBD, 0x79, 0x40,
				  0x4D, 0x48, 0xD0, 0xF5, 0x82, 0x7A, 0x8F, 0x37, 0x69, 0x86, 0x1D, 0xA4, 0xB9,
				  0xC2, 0xC1, 0xEF, 0x65, 0xF2, 0x05, 0xAB, 0x7E, 0x0B, 0x4A, 0x3B, 0x89, 0xE4,
				  0x6C, 0xBF, 0xE8, 0x8B, 0x06, 0x18, 0x51, 0x14, 0x7F, 0x11, 0x5B, 0x5C, 0xFB,
				  0x97, 0xE1, 0xCF, 0x15, 0x62, 0x71, 0x70, 0x54, 0xE2, 0x12, 0xD6, 0xC7, 0xBB,
				  0x0D, 0x20, 0x5E, 0xDC, 0xE0, 0xD4, 0xF7, 0xCC, 0xC4, 0x2B, 0xF9, 0xEC, 0x2D,
				  0xF4, 0x6F, 0xB6, 0x99, 0x88, 0x81, 0x5A, 0xD9, 0xCA, 0x13, 0xA5, 0xE7, 0x47,
				  0xE6, 0x8E, 0x60, 0xE3, 0x3E, 0xB3, 0xF6, 0x72, 0xA2, 0x35, 0xA0, 0xD7, 0xCD,
				  0xB4, 0x2F, 0x6D, 0x2C, 0x26, 0x1F, 0x95, 0x87, 0x00, 0xD8, 0x34, 0x3F, 0x17,
				  0x25, 0x45, 0x27, 0x75, 0x92, 0xB8, 0xA3, 0xC8, 0xDE, 0xEB, 0xF8, 0xF3, 0xDB,
				  0x0A, 0x98, 0x83, 0x7B, 0xE5, 0xCB, 0x4C, 0x78, 0xD1 };
// same as ShlWapi.HashData
HRESULT __stdcall HashData2(const unsigned char* lpSrc, DWORD nSrcLen, unsigned char *lpDest, DWORD nDestLen) noexcept
{
	INT srcCount = nSrcLen - 1, destCount = nDestLen - 1;
	if (!lpSrc || !lpDest)
		return E_INVALIDARG;
	while (destCount >= 0)
	{
		lpDest[destCount] = (destCount & 0xff);
		destCount--;
	}

	while (srcCount >= 0)
	{
		destCount = nDestLen - 1;
		while (destCount >= 0)
		{
			lpDest[destCount] = HashDataLookup[lpSrc[srcCount] ^ lpDest[destCount]];
			destCount--;
		}
		srcCount--;
	}
	return S_OK;
}
std::wstring getenv(wstring& envName)
{
	std::wstring env_var;
	size_t required_size;
	::_wgetenv_s(&required_size, nullptr, 0, envName.c_str());
	if (required_size == 0) return std::move(std::wstring());


	env_var.resize(required_size);
	::_wgetenv_s(&required_size, (wchar_t*)env_var.data(), required_size, envName.c_str());

	return std::move(env_var);
}
unsigned long get_hashcode(std::wstring& s)
{
	unsigned long hash = 0;

	if (!s.empty())
	{
		//compress unicode before hashing
		const auto lpza = ws2s(s);
		HRESULT	hr = HashData2((const unsigned char*)lpza.c_str(), lpza.length(), (LPBYTE)&hash, sizeof(unsigned long));
		if (hr != S_OK) hash = 0xFFFFFFFF;
	}

	return hash;
}