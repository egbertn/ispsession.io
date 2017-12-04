#include "stdafx.h"
#include <string>
#include "tools.h"

#include "CStream.h"
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
		[](unsigned char c){ return (unsigned char)std::toupper(c); } // correct
	);
	return s;

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
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return std::move( converterX.from_bytes(str));
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
void __stdcall FreeString(__out BSTR * theString) throw()
{
	if (theString != NULL && *theString != NULL)
	{
		SysFreeString(*theString);
		*theString = NULL;
	}
}

STDMETHODIMP ISequentialStream_Copy(_In_ ISequentialStream* iface, _In_ ISequentialStream* pstm, _In_ ULARGE_INTEGER cb, _Inout_opt_ ULARGE_INTEGER* pcbRead, _Inout_opt_ ULARGE_INTEGER* pcbWritten) throw()
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
STDMETHODIMP SerializeKeys(const std::vector<string> &keys, __in IKeySerializer* pDictionary, command& cmd, const string& appkeyPrefix) throw()
{

	string k(appkeyPrefix);
	auto prefixSize = appkeyPrefix.size();
	string baseString;
	CComObject<CStream>* cseqs;
	CComObject<CStream>::CreateInstance(&cseqs);
	CComPtr<IStream> stream;

	stream = cseqs;
	HRESULT hr = S_OK;
	CComBSTR bstrKey;
	LARGE_INTEGER set = { 0 };
	ULARGE_INTEGER newpos = { 0 };
	for (auto saddKey = 0; saddKey < keys.size(); ++saddKey)
	{
		ULARGE_INTEGER sze;
		sze.QuadPart = 128;
		stream->SetSize(sze);

		stream->Seek(set, STREAM_SEEK_SET, nullptr);


		bstrKey = keys[saddKey].c_str();
		hr = pDictionary->SerializeKey(bstrKey, stream);
		stream->Commit(STATFLAG_DEFAULT);//must be IStream thus 
		hr = stream->Seek(set, STREAM_SEEK_CUR, &newpos);
		auto cBytes = newpos.LowPart;
		logModule.Write(L"Serialize key %s %x len %d", bstrKey, hr, cBytes);
		BYTE buf[512] = { 0 };
		stream->SetSize(newpos);
		stream->Seek(set, STREAM_SEEK_SET, nullptr); //cut off to correct length
		ULONG read = 0;
		baseString.resize(0);
		while (hr == S_OK)
		{
			hr = stream->Read(buf, sizeof(buf), &read);
			if (read > 0)
			{
				baseString.append((char*)buf, read);
			}
		}
		if (SUCCEEDED(hr))
		{
			//only set redis keys to upper, not the serialized one
			k.resize(prefixSize);
			k.append( str_toupper(keys[saddKey]));
			cmd << k << baseString;
			hr = S_OK;
		}
	}	
	return hr;

}
// LicentieCheck
// do not modify these lines. It is illegal.
// position - 
// meaning
// position 1: License
// position 2-5 calculated license
// position 6-9 calculated hash on license (if license type = 4)
bool __stdcall ::LicentieCheck(GUID *license, BSTR strLicensedFor) throw()
{
	bool retVal = false;
	char licenseType = 0;
	//most left byte is license type.
	memcpy(&licenseType, license, sizeof(licenseType));
	// the domain name or PC name
	// prefer dns name, then NT4 Domain Name, then PC name.
	DWORD compName = MAX_COMPUTERNAME_LENGTH + 1;
	CComBSTR cwName, NT4NETBIOSNAME(compName), WORKGROUPNAME(L"");
	NT4NETBIOSNAME.Attach(GetNetBIOSName(false));

	// try to get the computer name through another API
	if (NT4NETBIOSNAME.Length() == 0)
	{
		logModule.Write(L"NO NETBIOS Name (%d)", GetLastError());
		NT4NETBIOSNAME.Attach(GetNetBIOSName(true));

	}
	//mostly ERROR_NO_SUCH_DOMAIN		
	PDOMAIN_CONTROLLER_INFOW pdomInfo;
	DWORD result1 = DsGetDcNameW(NT4NETBIOSNAME, nullptr, nullptr, nullptr, DS_DIRECTORY_SERVICE_PREFERRED | DS_RETURN_DNS_NAME, &pdomInfo);
	if (result1 != ERROR_SUCCESS)
	{
		logModule.Write(L"NO AD DOMAIN (%d)", result1);		//mostly ERROR_NO_SUCH_DOMAIN
		NETSETUP_JOIN_STATUS status;
		PWSTR wgname = nullptr;
		NET_API_STATUS apistatus = ::NetGetJoinInformation(nullptr, &wgname, &status);
		if (apistatus == NERR_Success)
		{
			if (status == NetSetupUnjoined || status == NetSetupUnknownStatus)
			{
				cwName.Attach(NT4NETBIOSNAME.Detach());
			}
			else if (status == NetSetupWorkgroupName)
			{
				cwName = wgname;
			}
			::NetApiBufferFree(wgname);
		}
		else
		{
			cwName.Attach(NT4NETBIOSNAME.Detach());
		}
	}
	else
	{
		cwName = pdomInfo->DomainName;
		::NetApiBufferFree(pdomInfo);
	}
	logModule.Write(L"Names %s, %s", NT4NETBIOSNAME.m_str, cwName);

	CComBSTR buf(strLicensedFor);


	// split since this is a CR LF line separated text file (notepad format)
	// our Control Number is and should be the last line!
	// should be a hidden string!

	CComSafeArray<BSTR> lines;
	//backward compat for line separated licenses
	if (buf.IndexOf(L"\r\n") > 0)
	{
		buf.Insert(0, L"\r\n");
		lines.Attach(buf.Split(L"\r\n"));
	}
	else
	{
		buf.Insert(0, L" ");
		lines.Attach(buf.Split(L" "));
	}
	lines.SetAt(0, ::SysAllocString(L"ISP Session Version 8.0"), false);

	ULONG arraySize = lines.GetCount();
	bool foundLicensedItem = false;
	while (arraySize-- != 1) // line 0 contains 
	{
		if (cwName.CompareTo(lines.GetAt(arraySize), true) == 0
			|| NT4NETBIOSNAME.CompareTo(lines.GetAt(arraySize), true) == 0
			|| WORKGROUPNAME.CompareTo(lines.GetAt(arraySize), true) == 0)
		{
			foundLicensedItem = true;
			break;
		}
	}
	if (foundLicensedItem == false && licenseType != 4)
	{
		logModule.Write(L"Could not find licensedItem %s in allowed licensee %s", cwName.m_str, buf);
		return false;
	}
	CComBSTR space(L" "); // reuse variable
	buf.Attach(CComBSTR::Join(lines.m_psa, space));
	ULONG hashcode = buf.GetHashCode(),
		checkCode, checkCode2;
	//offset 4
	memcpy(&checkCode, (PBYTE)license + sizeof(unsigned char),
		sizeof(checkCode));
	//offset 5
	memcpy(&checkCode2,
		(PBYTE)license + sizeof(unsigned char) + sizeof(checkCode),
		sizeof(checkCode2));
	logModule.Write(L"given license %d calculated license %d license hash %d",
		checkCode, hashcode, checkCode2);
	switch (licenseType)
	{
	case 1: case 2: case 3://isp session simple / advanced & ent
		if (hashcode == checkCode)
			retVal = true;

		break;
	case 4://isp session blk this just checks the GUID, not the given domain name

		buf = (LONG)licenseType;
		space = checkCode; //reuse variable
		buf += space;
		if (buf.GetHashCode() == checkCode2)
			retVal = true;
		break;
	default:
		retVal = false;
	}

	return retVal;
}