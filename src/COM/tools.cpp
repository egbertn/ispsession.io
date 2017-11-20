#include "stdafx.h"
#include <string>
#include "tools.h"
#include "ConfigurationManager.h"

std::wstring& __stdcall ltrim(std::wstring &s) 
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
		std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}
std::string& __stdcall ltrim(std::string &s) 
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
		std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}
// trim from end
std::wstring& __stdcall rtrim(std::wstring &s) 
{
	s.erase(std::find_if(s.rbegin(), s.rend(),
		std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}
std::string& __stdcall rtrim(std::string &s) 
{
	s.erase(std::find_if(s.rbegin(), s.rend(),
		std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
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
		[](unsigned char c){ return std::toupper(c); } // correct
	);
	return s;
}
//void split(const std::string &s, char delim, std::vector<std::string> &elems, int maxCount = 0)
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

bool __stdcall FileExists(const wchar_t * FileName) throw()
{
	FILE *file;
	bool result = false;
	if ((_wfopen_s(&file , FileName, L"r") == 0)) {
		fclose(file);
		result = true;
	}
	return result;
}
//attach to it
// e.g. searching on web.Config will return .Config
BSTR __stdcall FileExtension(const CComBSTR& FileName) throw()
{
	int lastPos = FileName.LastIndexOf('.');
	if (lastPos >= 0)
		return FileName.Substring(lastPos);
	return NULL;
}
//strips path and extension e.g. 'c:\\dir\\file.ext' becomes 'file'
// returns null if an error ocurred
BSTR __stdcall FileStripPath(const wchar_t* pathname) throw()
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
BSTR __stdcall FileStripFile(const wchar_t* pathname) throw()
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
BSTR __stdcall GetModulePath() throw()
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

//
BSTR __stdcall GetNetBIOSName(bool GiveDnsName = false) throw()
{
	DWORD compNameLength = GiveDnsName ? 255 : MAX_COMPUTERNAME_LENGTH;
	CComBSTR NT4NETBIOSNAME(compNameLength);
	compNameLength++;
	if (GetComputerNameExW(
		GiveDnsName ? ComputerNameDnsHostname : ComputerNameNetBIOS, NT4NETBIOSNAME, &compNameLength) == TRUE)
		// success
		NT4NETBIOSNAME.SetLength(compNameLength);
	else
	{
		return NULL;
	}
	return NT4NETBIOSNAME.Detach();
}

void __stdcall LogMessage(const DWORD messtype, PCWSTR msg[] = NULL, int els = 0) throw()
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
void __stdcall ReportComError(const DWORD win32err, PCWSTR msg) throw()
{	
	PWSTR berr = NULL;
	DWORD mlen = ::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, win32err,0, (LPWSTR)&berr, 0, NULL);
	if (mlen ==0)
	{
		CComBSTR num; 
		CComBSTR err(L"An unknown error occurred code: ");
		num.AssignLongHex(win32err);
		err.Append(num);		
		PCWSTR amsg[] = {msg, err};
		LogMessage(MSG_ERROR, amsg, 2);
	}
	else
	{
		PCWSTR amsg[] = {msg, berr};
		CComBSTR temp(berr);		
		temp.Append(':');
		temp.Append(msg);
		
		LogMessage(MSG_ERROR, amsg); 
		::GlobalFree(berr);
	}
	
}

void __stdcall ReportComError2(const HRESULT hr, PCWSTR msg) throw()
{
	
	PCWSTR amsg[] = {msg};
	int nr = 1;
	LogMessage(MSG_ERROR, amsg, 1);
}

DATE __stdcall Now() throw()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	DATE nu =0;
	SystemTimeToVariantTime(&st, &nu);	
	return nu;
}
//select convert(varchar(23), getdate(), 126) = ISO8601
// returns: eg '2009-04-22T13:11:36.377'
BSTR __stdcall FormatDBTimeStamp(const DATE ts) throw()
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
STDMETHODIMP OleSaveToStream2(IPersistStreamInit *pPersistStmInit, IStream *pStm) throw()
{
	if (pPersistStmInit == nullptr || pStm == nullptr)
		return E_INVALIDARG;
	CLSID clsd;

	HRESULT hr = pPersistStmInit->GetClassID(&clsd);
	if (hr == S_OK)
		hr = WriteClassStm(pStm, clsd);
	if (hr == S_OK)
		hr = pPersistStmInit->Save(pStm, TRUE);
	return hr;
}

STDMETHODIMP OleLoadFromStream2(IStream *pStm, REFIID iidInterface, void** ppvObj)
{
	if (pStm == nullptr || ppvObj == nullptr) return E_INVALIDARG;

	CLSID clsd;
	CComPtr<IPersistStreamInit> pPersist;
	HRESULT hr = ReadClassStm(pStm, &clsd);
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

// optional m_lngTimeOutSetting and 
// strConstruct [out]
STDMETHODIMP ReadDllConfig(BSTR * strConstruct, LONG *lTimeOutSetting, /*BOOL *hashSessionID,*/ int *enableLogging, GUID * license, BSTR* licenseContents) throw()
{

	ConfigurationManager config;
	
	CComBSTR key, val;
	
	key = L"ispsession_io:DataSource";
	*strConstruct = config.AppSettings(key);
	if (*strConstruct == NULL)
	{
		OutputDebugStringA("CSession.dll.Config not found, therefore exiting\r\n");
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
	}
	key.SetLength(512);
	auto stored = ::GetEnvironmentVariableW(*strConstruct, key, 512);
	if (stored > 0)
	{
		SysReAllocStringLen(strConstruct, key, stored);
	}
	if (lTimeOutSetting != nullptr)
	{
		key = L"ispsession_io:SessionTimeout";
		val.Attach(config.AppSettings(key, L"20"));
		if (val.Length() > 0 && val.IsNumeric())
			*lTimeOutSetting = val.ToLong();
	}
	/*if (hashSessionID!= NULL)
	{
		key = L"HASH_SESSIONID";
		val.Attach(config.AppSettings(key, L"false"));
		if (val.Length() > 0 && val.IsBool())
			*hashSessionID = val.ToBool()  == VARIANT_TRUE ? TRUE : FALSE;
	}*/
	
	/*if (EnableCompression != NULL)
	{
		key = L"UseCompression";
		val.Attach(config.AppSettings(key, L"1"));
		*EnableCompression = val.ToLong();
	}*/

	if (enableLogging != NULL)
	{
		key = L"ispsession_io:EnableLogging";
		val.Attach(config.AppSettings(key, L"0"));
		if (val.Length() > 0 && val.IsNumeric())
			*enableLogging= val.ToLong();
#ifdef DEBUG
		AtlTrace(L"Logging enabled %d\r\n", *enableLogging);
#endif
	}
	if (license != NULL)
	{
		key = L"ispsession_io:License";

		val.Attach(config.AppSettings(key, L""));
		if (val.Length() != 0)
		{
			setstring((const PUCHAR)license, val.m_str);
		}   
	}
	if (licenseContents != NULL)
	{
		key = L"ispsession_io:Csession.LIC";
		val.Attach(config.AppSettings(key, L""));
		if (val.Length() != 0)
		{
			*licenseContents = val.Detach();
		}
	}
	return S_OK;
}
wstring __stdcall s2ws(const std::string& str)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.from_bytes(str);
}

string __stdcall ws2s(const std::wstring& wstr)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return std::move( converterX.to_bytes(wstr));
}

//assumes that PBYTE is valid memory! 
//ensures that a return is given, e.g. NULL bytes will return an empty hexadecimal range of 'len' length
std::string __stdcall HexStringFromMemory(PBYTE bytes, int len) throw()
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
void __stdcall sHexFromBt(GUID* psa, BSTR *sRet) throw()
{
	auto pvdata = (PBYTE)psa;
	if (::SysReAllocStringLen(sRet, psa == NULL ? L"00000000000000000000000000000000" : NULL, sizeof(GUID) * 2) == TRUE)
	{

		//ZeroMemory(*sRet, sizeof(GUID) *2);
		BSTR sdata = *sRet;
		for (int cx = 0, bx = 0; cx < sizeof(GUID) * 2; bx++)
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
			sdata[cx++] = btByte;
			sdata[cx++] = btByte2;
		}
	}
}

// converts binary array to hexadecimal string prepended by 0x
// so {0xFF, 0xFF} would become L"0xFFFF" 
BSTR __stdcall sHexFromBt(const PUCHAR btBytes, LONG cb, bool prepend) throw()
{
    int cx; 
	BYTE btBlah;
	BYTE btByte;
    BYTE btByte2;
    LONG lUbound = cb;
	PBYTE btfetch;

	PULONG pulHex;
	CComBSTR myGUID(cb * 2 + 2); 
	if (btBytes == NULL) 
		myGUID = L"00000000000000000000000000000000";
	else
	{	
		if (prepend)
		{
			pulHex = (PULONG)myGUID.m_str + 1;
			myGUID.m_str[0] = L'0';
			myGUID.m_str[1] = L'x';
		}
		else
		{
			pulHex = (PULONG)myGUID.m_str;
		}

		btfetch = (PBYTE) btBytes;

		for (cx =lUbound  - 1 ; cx>=0; cx--) 
		{
			btBlah = btfetch[cx];
			btByte2 =btBlah & 15;
			btByte = btBlah >>4;

			if (btByte > 9)  
				btByte += 55;
			else 
				btByte |=  48;

			if (btByte2 > 9) 
				btByte2 +=  55;
			else 
				btByte2 |= 48;
			
			pulHex[cx]= (ULONG)btByte2 <<16 | btByte;
		}
	}
	return  myGUID.Detach();
	
}
BOOL __stdcall IsValidHex(const BSTR Cookie) throw()
{
	UINT sLen = ::SysStringLen(Cookie);

	if (sLen == (sizeof(GUID) * 2))
	{		
		for (INT cx = (sizeof(GUID) / sizeof(int)) - 1; cx >= 0; cx--)
		{
			auto converted = wcstoul(&Cookie[cx*sizeof(__int64)], L'\0', 16);
			if (converted == 0)
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}
// Converts Hex GUID in Parameter strCookiePtr to addrGUID
BOOL __stdcall setstring(const PUCHAR addrGUID,const BSTR strCookiePtr) throw()
{
	
	LONG cx = 0;
	UCHAR btByte = 0, btByte2 = 0;
	PUCHAR sdata = reinterpret_cast<PUCHAR>(strCookiePtr);
	UINT sLen = ::SysStringLen(strCookiePtr);
	BOOL retval = TRUE;
	if (sLen == (sizeof(GUID) * 2))
	{
		int bx = sizeof(GUID) - 1;
		for (cx = (sLen * 2) - 4; cx >= 0; cx -= 4, bx--)
		{
			btByte = sdata[cx];
			btByte2 = sdata[cx + 2];

			if ((btByte >= 65) & (btByte <= 70)) //& correct
				btByte -= 55;
			else if ((btByte >= 48) & (btByte <= 57))
				btByte ^= 48;
			//'needless test but you never now if somebody converts to lowercase
			else if ((btByte >= 97) & (btByte <= 102))
				btByte -= 87;
			else{ retval = FALSE; break; }

			if ((btByte2 >= 65) & (btByte2 <= 70))
				btByte2 -= 55;
			else if ((btByte2 >= 48) & (btByte2 <= 57))
				btByte2 ^= 48;  //' cut the two leftmost bits 110000
			//'needless test but you never now if somebody converts to lowercase
			else if ((btByte2 >= 97) & (btByte2 <= 102))
				btByte2 -= 87;
			else
			{
				retval = FALSE; break;
			}
			addrGUID[bx] = (btByte * 16) | btByte2;
		}

	}
	else
		retval = FALSE;

	return  retval;
	
}
void __stdcall FreeString(BSTR * theString) throw()
{
	if (theString != NULL && *theString != NULL)
	{
		SysFreeString(*theString);
		*theString = NULL;
	}
}
