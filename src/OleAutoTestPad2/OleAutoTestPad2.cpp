// OleAutoTestPad2.cpp : Implementation of WinMain

#include <map>

#include "stdafx.h"
#include <iostream>
#include <atlcoll.h>
#include "resource.h"
//#include "OleAutoTestPad2_i.h"
#include <atlcoll.h>
#include <filesystem>
#include <string>
#include <stdlib.h>

#include <stdio.h>

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

HRESULT __stdcall HashData2(const unsigned char* lpSrc, DWORD nSrcLen, unsigned char *lpDest, DWORD nDestLen)
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
// Converts Hex GUID in Parameter strCookiePtr to addrGUID
BOOL __stdcall setstring(const PUCHAR addrGUID,const BSTR strCookiePtr) throw()
{
	LONG cx =0;
    UCHAR btByte =0, btByte2 = 0;
	PUCHAR sdata = reinterpret_cast<PUCHAR>(strCookiePtr);
	UINT sLen = ::SysStringLen(strCookiePtr);
	BOOL retval = TRUE;
    if (sLen == (sizeof(GUID) * 2)) 
	{
		int bx = sizeof(GUID) - 1;
        for (cx = (sLen * 2) - 4; cx >= 0; cx -= 4, bx--)
		{
            btByte = sdata[cx ];
            btByte2 = sdata[cx + 2];

			if ((btByte >= 65) & (btByte <= 70)) //& correct
                btByte -= 55;
            else if ((btByte >= 48) & (btByte <= 57)) 
                btByte ^= 48;
            //'needless test but you never now if somebody converts to lowercase
            else if ((btByte >= 97) & (btByte <= 102))
                btByte -= 87;
			else{retval = FALSE; break;}

            if ((btByte2 >= 65) & (btByte2 <= 70))
                btByte2 -= 55;
            else if ((btByte2 >= 48) & (btByte2 <= 57))
                btByte2 ^= 48;  //' cut the two leftmost bits 110000
            //'needless test but you never now if somebody converts to lowercase
            else if ((btByte2 >= 97) & (btByte2 <= 102))
                btByte2 -= 87;
			else
               {retval = FALSE; break;}
            addrGUID[bx] = (btByte * 16) | btByte2;
		}
        
	}
	else
		retval = FALSE;

	return  retval;
}


class COleAutoTestPad2Module : public ATL::CAtlExeModuleT< COleAutoTestPad2Module >
{
public :
	DECLARE_NO_REGISTRY()
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_OLEAUTOTESTPAD2, "{E83463A0-3359-4FEC-86A0-E7464635C1A8}")
	};

COleAutoTestPad2Module _AtlModule;

enum  DECLSPEC_UUID("CCB79238-5A61-49e2-A740-77B4782E7926") ADC_PRODUCT
    {	ADC_PHOTO	= 1,
	//simple
	ADC_CACHE	= 2,
	ADC_CACHE_ADV	= 4,
	ADC_SECURE_PORT	= 8,
	ADC_CHECKUP	= 16,
	ADC_ISPSESSION	= 32,
	ADC_ISPSESSION_ADV	= 64,
	ADC_ISPSESSION_ENT	= 128,
	ADC_ISPSESSION_BULK	= 256,
	ADC_CACHE_ENT = 512,
	ADC_DICTIONARY_BIN = 1024,
	ADC_DICTIONARY_BULK = 2048, //2^11

	ADC_ISPSTATE_PROVIDER_SIMPLE = 4096, //2^12
	ADC_ISPSTATE_PROVIDER_ADV = 8192,   //2^13
	ADC_ISPSTATE_PROVIDER_ENT = 16384,  //2^14
	ADC_ISPSTATE_PROVIDER_BULK = 32768,	//2^15
	ADC_ISPSTATE_CORE_SIMPLE = 65536,	//2^16
	ADC_ISPSTATE_CORE_ENT = 131072,		//2^17
	ADC_ISPSTATE_CORE_ADV = 262144,		//2^18
	ADC_ISPSTATE_CORE_BULK = 524288,	//2^19
	ADC_CACHE_BULK = 2 ^ 20,
	ADC_ALL	= 0xffffffff
    } ;
// Converts Hex GUID in Parameter strCookiePtr to addrGUID
bool __stdcall setString(const GUID* guid,const BSTR strCookiePtr) throw()
{
	LONG cx =0;
    wchar_t btByte =0, btByte2 = 0;
	PUCHAR addrGUID = (PUCHAR)guid;
	PUCHAR sdata = (PUCHAR)strCookiePtr;
	UINT sLen = ::SysStringLen(strCookiePtr);
	bool retval = true;
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
			else{retval = false; break;}

            if ((btByte2 >= 65) & (btByte2 <= 70))
                btByte2 -= 55;
            else if ((btByte2 >= 48) & (btByte2 <= 57))
                btByte2 ^= 48;  //' cut the two leftmost bits 110000
            //'needless test but you never now if somebody converts to lowercase
            else if ((btByte2 >= 97) & (btByte2 <= 102))
                btByte2 -= 87;
			else
               {retval = false; break;}
            addrGUID[bx] = (btByte * 16) | btByte2;
		}
        
	}
	else
		retval = false;

	return  retval;
}
bool __stdcall LicentieCheck(GUID *license, BSTR strLicensedFor) throw()
{
	bool retVal = false;
	char licenseType = 0;
	//most left byte is license type.
	memcpy(&licenseType, license, sizeof(licenseType));
	// the domain name or PC name
	// prefer dns name, then NT4 Domain Name, then PC name.

    CComBSTR NT4NETBIOSNAME;
    CComBSTR dnsDomain;
    CComBSTR buf(strLicensedFor);
    dnsDomain.Attach(CComBSTR(L"blah"));
    NT4NETBIOSNAME.Attach(CComBSTR(L"adf"));


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
	lines.SetAt(0, ::SysAllocString(licenseType >= 5 ? L"ISP Session Cache 8.0" : L"ISP Session Version 8.0"), false);

	ULONG arraySize = lines.GetCount();
	bool foundLicensedItem = false;
	while (arraySize-- != 1) // line 0 contains 
	{
		if (NT4NETBIOSNAME.CompareTo(lines.GetAt(arraySize), true) == 0
			|| dnsDomain.CompareTo(lines.GetAt(arraySize), true) == 0)
		{
			foundLicensedItem = true;
			break;
		}
	}
	if (foundLicensedItem == false && (licenseType != 4 && licenseType != 8))
	{
		//logModule.Write(L"Could not find licensedItem %s in allowed licensee %s", NT4NETBIOSNAME.m_str, buf);
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
	//logModule.Write(L"given license %d calculated license %d license hash %d",
		//checkCode, hashcode, checkCode2);
	switch (licenseType)
	{
	case 5: case 6: case 7:

	case 1: case 2: case 3://isp session simple / advanced & ent
		if (hashcode == checkCode)
			retVal = true;

		break;
	case 8:
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
//bool LicentieCheck(PCWSTR product) throw()
//{
//	bool retVal = false;
//	// the domain name or PC name
//	// prefer dns name, then NT4 Domain Name, then PC name.
//	DWORD compName = MAX_COMPUTERNAME_LENGTH+1;
//	CComBSTR cwName,NT4NETBIOSNAME(compName);
//
//	// try to get the computer name through another API
//	if (GetComputerNameExW(ComputerNameNetBIOS, NT4NETBIOSNAME, &compName) == TRUE)
//		// success
//		NT4NETBIOSNAME.SetLength(compName);
//	else
//	{
//		//logModule.Write(L"NO NETBIOS Name (%d)", GetLastError());		
//	}
//	//mostly ERROR_NO_SUCH_DOMAIN		
//	PDOMAIN_CONTROLLER_INFOW pdomInfo;
//	DWORD result1 = DsGetDcNameW(NT4NETBIOSNAME, NULL, NULL, NULL, DS_DIRECTORY_SERVICE_PREFERRED | DS_RETURN_DNS_NAME, &pdomInfo);
//	if (result1 != ERROR_SUCCESS)		
//	{
//		//logModule.Write(L"NO AD DOMAIN (%d)", result1);		//mostly ERROR_NO_SUCH_DOMAIN
//		NETSETUP_JOIN_STATUS status;
//		PWSTR wgname = NULL;
//		NET_API_STATUS apistatus = NetGetJoinInformation(NULL, &wgname, &status);
//		if (apistatus == NERR_Success)
//		{
//			if (status == NetSetupUnjoined || status == NetSetupUnknownStatus)
//			{		
//				cwName.Attach(NT4NETBIOSNAME.Detach());
//			}
//			else if(status == NetSetupWorkgroupName)
//			{
//				cwName.Attach(CComBSTR(wgname).Detach());
//			}
//			NetApiBufferFree(wgname);
//		}	
//		else
//		{
//			cwName.Attach(NT4NETBIOSNAME.Detach());
//		}
//	}
//	else
//	{
//		cwName = pdomInfo->DomainName;
//		NetApiBufferFree(pdomInfo);		
//	}	
//	//logModule.Write(L"Names %s, %s", NT4NETBIOSNAME, cwName);
//	
//	CComBSTR licenseFile(MAX_PATH);
//	HINSTANCE hinst = _AtlBaseModule.GetModuleInstance();
//	DWORD bufLen = GetModuleFileNameW(hinst, licenseFile, MAX_PATH);
//	if (bufLen > 0)	
//		//strip right length
//		licenseFile.SetLength(bufLen);
//	if (bufLen > MAX_PATH)	
//		GetModuleFileNameW(hinst, licenseFile, bufLen);
//	if (licenseFile.EndsWith(L"64.dll"))
//		licenseFile.Remove(licenseFile.Length() - 6, 2);
//	PathRemoveFileSpecW(licenseFile);
//	licenseFile.SetLength((unsigned int)wcslen(licenseFile));
//	CComBSTR path(licenseFile);
//	
//	licenseFile += L"\\*.LIC";
//
//	WIN32_FIND_DATAW findData = {0};
//	HANDLE h = FindFirstFileW(licenseFile, &findData);
//	GUID license = {0};
//	BOOL result = TRUE;
//	bool validGUID = false;
//	while (h != INVALID_HANDLE_VALUE  && ::GetLastError() != ERROR_FILE_NOT_FOUND && result == TRUE)
//	{
//		licenseFile = findData.cFileName;
//		//check this license!
//		CComBSTR bstrLicense;
//		//bstrLicense = PathFindFileNameW(licenseFile);
//		PathRemoveExtensionW(licenseFile);
//		licenseFile.SetLength((unsigned int)wcslen(licenseFile));
//		int checkLen = licenseFile.Length();
//		if (checkLen == 32)
//		{
//			
//			validGUID = setString(&license, licenseFile);
//			if (validGUID) break;
//		}
//		
//		result = FindNextFileW(h, &findData);
//		
//	}
//	if (INVALID_HANDLE_VALUE != h)
//		FindClose(h);
//	else
//	{
////		logModule.Write(L"%s could not be read because of %d", licenseFile, GetLastError());
//		return false;
//	}
//	if (validGUID == false)
//	{
////		logModule.Write(L"invalid licence code %s", licenseFile);
//		return false;
//	}
//	char licenseType = 0;
//	//most left byte is license type.
//	memcpy(&licenseType, &license, sizeof(licenseType));
//	licenseFile.Insert(0, L"\\");
//	licenseFile.Insert(0, path);
//	licenseFile.Append(L".LIC");
//	CAtlFile lFile;
//	// makes createfile use lower level instead of win32
//	licenseFile.Insert(0, L"\\\\?\\");
//	//logModule.Write(L"opening %s", licenseFile);
//	HRESULT hr = lFile.Create(licenseFile, FILE_READ_DATA, FILE_SHARE_READ, OPEN_EXISTING);
//	if (FAILED(hr))
//	{
//		//logModule.Write(L"Cannot open %s %x", licenseFile, hr);
//		return false;
//	}
//	else 
//	{
//		ULONGLONG nLen;
//		
//		lFile.GetSize(nLen);
//		LONG lLen = (LONG)nLen; //cutoff __int64  to int
//		// assume Utf16 and no content messing
//		CComBSTR buf(lLen / sizeof(wchar_t));
//		
//		hr = lFile.Read((void*)buf.m_str, lLen);
//		if (hr != S_OK || (lLen > 1 && buf.m_str[0] != 0xFEFF)) // test for correct utf16 preample
//		{
//			//Error(L"invalid fileformat", CLSID_NWCSession, hr);
////			logModule.Write(L"Invalid fileformat %s, should be utf16(unicode2), %x", licenseFile, hr);
//			return false;
//		}
//		buf.Remove(0, 1); //remove the Utf16-PreAmple
//		// split since this is a CR LF line separated text file (notepad format)
//		// our Control Number is and should be the last line!
//		// should be a hidden string!
//		buf.Insert(0, product);
//		ATL::CComSafeArray<BSTR> lines;
//		lines.Attach(buf.Split(L"\r\n"));
//		ULONG arraySize = lines.GetCount();
//		bool foundDomain = false;
//		while(arraySize-- != 1) // line 0 contains 
//		{
//			if (cwName.CompareTo(lines.GetAt(arraySize), true) == 0)
//			{
//				foundDomain = true;
//				break;
//			}
//		}
//		if (foundDomain == false && (licenseType != 4 && licenseType != 21)) //ADC_ISPSESSION_BULK, ADC_DICTIONARY_BULK
//		{
//			//logModule.Write(L"Could not find domain %s in licfile %s", cwName, licenseFile);
//			return false;
//		}
//		licenseFile = L" "; // reuse variable
//		buf.Attach(CComBSTR::Join(lines.m_psa, licenseFile));
//		ULONG hashcode = buf.GetHashCode(),
//			checkCode, checkCode2;
//		//offset 4
//		memcpy_s(&checkCode, sizeof(ULONG), &((PBYTE)&license)[sizeof(unsigned char)], 
//			sizeof(checkCode));
//		//offset 5
//		memcpy_s(&checkCode2, sizeof(ULONG),
//			&((PBYTE)&license)[sizeof(unsigned char) + sizeof(checkCode)], 
//			sizeof(checkCode2));
//		//logModule.Write(L"given license %d calculated license %d license hash %d", 
//			//checkCode, hashcode, checkCode2); 
//		switch(licenseType)
//		{
//		case 1: case 2: case 3: case 20: case 22: //isp session simple / advanced & ent GUIDGen
//			if (hashcode == checkCode) 
//				retVal = true;
//
//			break;		
//		case 21: //ADC Dictionary
//		case 4://isp session blk
//			
//			buf = (LONG)licenseType;
//			licenseFile = checkCode; //reuse variable
//			buf += licenseFile;
//			if (buf.GetHashCode() == checkCode2) 
//				retVal = true;
//		
//			break;
//		default:
//			retVal = false;
//		}		
//	}	
//	return retVal;
//}

BSTR __stdcall sHexFromBt(const PUCHAR btBytes, LONG cb) throw()
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
		myGUID.Empty();
	else
	{	
		pulHex = (PULONG)myGUID.m_str + 1;
		myGUID.m_str[0] = L'0';
		myGUID.m_str[1] = L'x';

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

DATE STDMETHODCALLTYPE Now(void) throw()
{
	SYSTEMTIME st;
	::GetLocalTime(&st);
	double nu =0;
	::SystemTimeToVariantTime(&st, &nu);
	return nu;
}

GUID __stdcall IspSessionGen(BSTR sites, unsigned char licenseType) throw()
{
	
	//most left byte is license type.
	GUID retVal = {0};
	
	//copy the addressof retVal to a handy pointer
	memcpy(&retVal, &licenseType, sizeof(licenseType));

	CComBSTR strSites, extraLicCheck, num;
	strSites.AssignBSTR(sites);
	PWSTR prodString = NULL;
	switch (licenseType)
    {
        case 1:
        case 2:
        case 3:
        case 4:
            prodString = L"ISP Session Version 8.0 ";
            break;
		case 5:
		case 6:
		case 7:
		case 8:
			prodString = L"ISP Session Cache 8.0 ";
			break;
		case 22:
		case 23:		
		case 24:
		case 25:
			prodString = L"ISP Session 8.5 State Module ";
			break;
		case 26:
		case 27:
		case 28:
		case 29:
			prodString = L"ISP Session 9.0 State Provider ";
			break;
        default:
            prodString = L"ADC Cure reserved ";
            break;
    }
	strSites.Insert(0, prodString);
	int countLines = CComBSTR::CountChar(strSites, ' ');
	if (countLines < 4) return retVal;
	ULONG checkCode = strSites.GetHashCode();
	memcpy( ((PBYTE)&retVal) + 1, &checkCode, sizeof(checkCode));

	extraLicCheck = (LONG)licenseType;
	num = checkCode;
	extraLicCheck += num;
	ULONG checkCode2 = extraLicCheck.GetHashCode();
	memcpy(  ((PBYTE)&retVal) + 5, &checkCode2, sizeof(checkCode));
	srand((int)GetTickCount());
	//fill with 'usage' data
	for (int cx=7 ; cx!= 0; cx--)
	{
		retVal.Data4[cx] = rand() / (RAND_MAX / 128);
	}
	return retVal;
}
STDMETHODIMP GenerateLicense_(BSTR domain, SAFEARRAY* serversList, 
										   ADC_PRODUCT adcProduct,
										   BSTR * license) throw()
{

	unsigned char ispSessionLicensType = 0;
	switch(adcProduct )
	{
		case ADC_ISPSESSION:
			ispSessionLicensType = 1;
			break;
		case ADC_ISPSESSION_ADV:
			ispSessionLicensType = 2;
			break;
		case ADC_ISPSESSION_ENT:
			ispSessionLicensType = 3;
			break;
		case ADC_ISPSESSION_BULK:
			ispSessionLicensType = 4;
			break;
		case ADC_CACHE:
			ispSessionLicensType = 5;
			break;
		case ADC_CACHE_ADV:
			ispSessionLicensType = 6;
			break;
		case ADC_CACHE_ENT:
			ispSessionLicensType = 7;
			break;
		case ADC_CACHE_BULK:
			ispSessionLicensType = 8;
			break;
	/*	case ADC_FILERENAME:
			ispSessionLicensType = 10;*/
			//break;
		//case ADC_DICTIONARY_BIN:
		//	ispSessionLicensType = 20;
		//	break;
		//case ADC_DICTIONARY_BULK:
		//	ispSessionLicensType = 21;
		//	break;
		case ADC_ISPSTATE_PROVIDER_SIMPLE:
			ispSessionLicensType = 22;
			break;
		case ADC_ISPSTATE_PROVIDER_ADV:
			ispSessionLicensType = 23;
			break;
		case ADC_ISPSTATE_PROVIDER_ENT:
			ispSessionLicensType = 24;
			break;
		case ADC_ISPSTATE_PROVIDER_BULK:
			ispSessionLicensType = 25;
			break;
		case ADC_ISPSTATE_CORE_SIMPLE:
			ispSessionLicensType = 26;
			break;
		case ADC_ISPSTATE_CORE_ADV:
			ispSessionLicensType = 27;
			break;
		case ADC_ISPSTATE_CORE_ENT:
			ispSessionLicensType = 28;
			break;
		case ADC_ISPSTATE_CORE_BULK:
			ispSessionLicensType = 29;
			break;
		default:
			break;
	}
	// is it concerning one of the ISP Session licenses
	if (ispSessionLicensType != 0)
	{
		CComBSTR domainList;
		
		domainList.Attach(CComBSTR::Join(serversList, CComBSTR(L" ")));
		GUID guid = IspSessionGen(domainList, ispSessionLicensType);
		CComBSTR temp;
		temp.Attach(*license);		
		temp.Attach( sHexFromBt((PUCHAR)&guid, sizeof(GUID)));
		*license = temp.Detach();
		return 0;
	}
	else

		return S_OK;
}
static STDMETHODIMP _SetByteLength(_Inout_ BSTR *str, _In_ unsigned int length) throw()
	{
		/*#ifdef SysReAllocStringByteLen
			return SysReAllocStringByteLen2(str, NULL, length) == FALSE ? E_OUTOFMEMORY : S_OK;
		#else*/
			BSTR Copy = NULL;
			UINT origLen = SysStringByteLen(*str);
			if (origLen != 0)
			{
				if ((length % 2) == 0)
				{
					SysReAllocStringLen(str, NULL, length / 2);
				}
				else
				{
					Copy = SysAllocStringByteLen((LPCSTR)*str, length);
					SysFreeString(*str);
					*str = Copy;
				}
			}

			return *str == NULL  ? E_OUTOFMEMORY : S_OK;

		//#endif
	}
//
static CComBSTR& GetKey() throw()
{
	char p[81] = { 0 };
	char  a[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };
	for (int i = 0; i <= 10; i++){
		*(p + i) = (a[(rand() + 100) % 26]);
	}
	return CComBSTR(p);
}
std::wstring getenv(LPCWSTR envName)
{
	std::wstring env_var;
	size_t required_size;
	::_wgetenv_s(&required_size, nullptr, 0, envName);
	if (required_size == 0) return std::move(std::wstring());


	env_var.resize(required_size);
	::_wgetenv_s(&required_size, (wchar_t*)env_var.data(), required_size, envName);
	
	return std::move(env_var);
}
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
								LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	auto compname = getenv(L"COMPUTERNAME");
	

	//HTMLReplace match[] = { { L"&", L"&amp;" },{ L"<", L"&lt;" },{ L">", L"&gt;" } };
	char bytes[] = { 1,2,3,4,6,2,6,2,2,5,204,-128,6,3,5,45,2,7,7,128,254 };
	UINT hash=0;
	UINT hash2 = 0;
	auto hashResult = HashData((LPBYTE)bytes, sizeof(bytes), (BYTE*)&hash, sizeof(hash));

	 hashResult = HashData2((LPBYTE)bytes, sizeof(bytes), (BYTE*)&hash2, sizeof(hash));

	//for (int x = 0; x < sizeof(match) / sizeof(HTMLReplace); x++)
	//{
	//	auto t = x;
	//}

	//map<CComBSTR, CComVariant> xxx;
	////CRBMap<CComBSTR, CComVariant> keys;
	//for (int x = 0; x < 100000; x++)
	//{
	//	CComVariant asd(10);
	//	CComBSTR key;
	//	

	//	key.Append(GetKey());
	//	//keys.SetAt(key, asd);
	//	xxx.insert(pair<CComBSTR, CComVariant>(key, asd));
	//}
	//
	
	
	//auto vv = k;
	/*CComVariant v;
	CComBSTR key2;*/
	//auto pos = keys.Lookup(key2);

	//auto pos = xxx.find(L"sdf");
	
	//CComPtr<_Recordset> ptr;
	//CComPtr<ADOFields> fields;
	//CoInitialize(0);
 	//HRESULT hr = ptr.CoCreateInstance(L"ADODB.Recordset.6.0");
	//hr = ptr->put_CursorLocation(CursorLocationEnum::adUseClient);
	//VARIANT vt_missing = { 0 };
	//vt_missing.vt = VT_ERROR;
	//vt_missing.scode = DISP_E_PARAMNOTFOUND;

	//ptr->get_Fields(&fields);
	//hr = fields->Append(CComBSTR(L"piet"), DataTypeEnum::adVarChar, 100, FieldAttributeEnum::adFldUnspecified, vt_missing);
	//hr = ptr->Open(vt_missing, vt_missing);
	//hr = ptr->AddNew(vt_missing, vt_missing);
	//hr = ptr->put_Collect(CComVariant(0), CComVariant(L"hoi"));
	//

	//
	//CComPtr<IStream> pstr;
	//CComPtr<IPersistStream> persist;
	//persist = ptr;
	//::CreateStreamOnHGlobal(NULL, TRUE, &pstr);
	//HRESULT hrSave = OleSaveToStream(persist, pstr);
	//pstr.Release();
	//persist.Release();
	//ptr.Release();
	//CoUninitialize();
	//std::wifstream file(L"c:\\temp\\file.txt", ios_base::binary);
	//
	//std::locale utf8_locale(std::locale(), new std::codecvt_utf8<wchar_t>);
	//file.imbue(utf8_locale);
	//std::vector<std::wstring> linev;
	//
	//wchar_t buffer[10000] = { 0 };
	//while (file.good())
	//{
	//	file.getline(buffer, 10000);
	//	linev.push_back(buffer);
	//}
	//file.close();
	//std::wofstream file2(L"c:\\temp\\file.txt", ios_base::out);

	//file2.imbue(utf8_locale);

	//std::vector<std::wstring>::size_type sz = linev.size();
	//sz--;
	//for (std::vector<std::wstring>::size_type i = sz / 2; i<= sz ; i++)
	//{
	//	file2 << linev[i];		
	//}	
	//file2.close();
	//
	//return 0;
	//int dd = 5;
	//int zz = dd & 4;
		CComSafeArray<BSTR> domains;		
		domains.Add(CComBSTR(L"IAMLAB.OU").Detach(), false);
        /*domains.Add(CComBSTR(L"WORKGROUP").Detach(), false);
        domains.Add(CComBSTR(L"WIN-85FKBE92TOM").Detach(), false);*/
		//domains.Add(CComBSTR(L"sl.local").Detach(), false);
		//domains.Add(CComBSTR(L"3dc.local").Detach(), false);
		//domains.Add(CComBSTR(L"SchoolLogic").Detach(), false);
		//domains.Add(CComBSTR(L"UVONLINE").Detach(), false);
		//domains.Add(CComBSTR(L"UVONLINE-SERVER").Detach(), false);	
		//domains.Add(CComBSTR(L"DIGIDATA PROCESSING").Detach(), false);	
		//WWW-SERVER
		/*lunchtime.lu
			lunchtime.local
			lunchtime
			IRON.lunchtime.local
			IRON*/
		
	/*	domains.Add(CComBSTR(L"lunchtime.lu").Detach(), false);	
		domains.Add(CComBSTR(L"HB2259").Detach(), false);	
		domains.Add(CComBSTR(L"main.lunchtime.lu").Detach(), false);	
		domains.Add(CComBSTR(L"lunchtime.lu").Detach(), false);	
		domains.Add(CComBSTR(L"lunchtime").Detach(), false);
		domains.Add(CComBSTR(L"IRON").Detach(), false);
		domains.Add(CComBSTR(L"lunchtime.local").Detach(), false);
		domains.Add(CComBSTR(L"WWW-SERVER").Detach(), false);*/
		//domains.Add(CComBSTR(L"WIN-HPGECP6KUB0").Detach(), false); 
	//	domains.Add(CComBSTR(L"visma.com").Detach(), false);
		//domains.Add(CComBSTR(L"WORKGROUP").Detach(), false);

		//
		
		//domains.Add(CComBSTR(L"679777-app1").Detach(), false);	
		//domains.Add(CComBSTR(L"WORKGROUP").Detach(), false);	
		//domains.Add(CComBSTR(L"SSAAUR2VAPP01").Detach(), false);	
		//domains.Add(CComBSTR(L"SSAAUR2VAPP02").Detach(), false);	
		//domains.Add(CComBSTR(L"SSAAUR2VAPP03").Detach(), false);	
		//domains.Add(CComBSTR(L"SSAAUR2VAPP04").Detach(), false);	
	/*	domains.Add(CComBSTR(L"LKQ.LKQX.NET").Detach(), false);	
		domains.Add(CComBSTR(L"LKQX.DMZ").Detach(), false);*/


		//domains.Add(CComBSTR(L"treehousepark.com"));
		/*domains.Add(CComBSTR(L"NVIIS010").Detach(), false);
		domains.Add(CComBSTR(L"NVIIS020").Detach(), false);
		domains.Add(CComBSTR(L"NVIIS030").Detach(), false);*/
		//domains.Add(CComBSTR(L"WIN-85FKBE92TOM").Detach(), false);
		//domains.Add(CComBSTR(L"theeroticreview.com").Detach(), false);	
		//domains.Add(CComBSTR(L"OFFICE-WIN-08").Detach(), false);	
		//domains.Add(CComBSTR(L"ELECTRONIC").Detach(), false);	
		//domains.Add(CComBSTR(L"mrcctl.com").Detach(), false);	
		//domains.Add(CComBSTR(L"WIN-HPGECP6KUB0").Detach(), false);
		//domains.Add(CComBSTR(L"aftercollege.com").Detach(), false);

		//domains.Add(CComBSTR(L"uniservity.local").Detach(), false);
		//domains.Add(CComBSTR(L"uniservity2.local").Detach(), false);
		//domains.Add(CComBSTR(L"portaleducacao.com.br").Detach(), false);
		//domains.Add(CComBSTR(L"mijndomein2.com").Detach(), false);	
		//domains.Add(CComBSTR(L"KPACITASER3").Detach(), false);
		//domains.Add(CComBSTR(L"KPACITASERV3.local").Detach(), false);
		//domains.Add(CComBSTR(L"Kpacitaserv2.KPACITA.local").Detach(), false);
		//domains.Add(CComBSTR(L"nieropweb.local").Detach(), false);
		//domains.Add(CComBSTR(L"clc1.uniservity.local").Detach(), false);
		//domains.Add(CComBSTR(L"gzo.hdlz.loc").Detach(), false);
		//domains.Add(CComBSTR("cse-healthcare.com").Detach(), false);
		//domains.Add(CComBSTR(L"planetpinnacle.com").Detach(), false);
		//domains.Add(CComBSTR(L"pinnaclesports.local").Detach(), false);
		//domains.Add(CComBSTR(L"Pixelpushers.Inc").Detach(), false);
		
		//domains.Add(CComBSTR(L"WWW-SERVER").Detach(), false);
		//domains.Add(CComBSTR(L"HB2259").Detach(), false);
		//domains.Add(CComBSTR(L"main.lunchtime.lu").Detach(), false);


		//domains.Add(CComBSTR(L"streetsideauto.com").Detach(), false);
		
	/*	domains.Add(CComBSTR(L"Mrcctl.com").Detach(), false);		
		domains.Add(CComBSTR(L"ELECTRONIC").Detach(), false);	
		domains.Add(CComBSTR(L"ELECTRONIC2").Detach(), false);*/
		//domains.Add(CComBSTR(L"computershare.com.au"));
		/*domains.Add(CComBSTR(L"679777-app1"));
		domains.Add(CComBSTR(L"WORKGROUP"));*/

		// domains.Add(CComBSTR(L"comitas.ch"));
		/*domains.Add(CComBSTR(L"Mrcctl.com"));
		domains.Add(CComBSTR(L"ELECTRONIC2"));*/
		//.Add(CComBSTR(L"autobene.local"));
		//domains.Add(CComBSTR(L"innproc.com"));
	
		//domains.Add(CComBSTR(L"GBOXWEB01").Detach(), false);	
		//domains.Add(CComBSTR(L"GBOXWEB02").Detach(), false);
		//domains.Add(CComBSTR(L"GBOXWEB03").Detach(), false);
		//domains.Add(CComBSTR(L"GBOXWEB04").Detach(), false);
		//domains.Add(CComBSTR(L"GBOXWEB05").Detach(), false);
		//domains.Add(CComBSTR(L"GBOXWEB06").Detach(), false);
		//domains.Add(CComBSTR(L"GBOXWEB07").Detach(), false);
		//domains.Add(CComBSTR(L"GBOXWEB08").Detach(), false);
		//domains.Add(CComBSTR(L"GBOXWEB09").Detach(), false);
		//domains.Add(CComBSTR(L"GBOXWEB10").Detach(), false);
		//domains.Add(CComBSTR(L"KRESTENBUCH-PC").Detach(), false);
		//domains.Add(CComBSTR(L"LAST-PC").Detach(), false);
		//domains.Add(CComBSTR(L"PETE-PC").Detach(), false);
		//domains.Add(CComBSTR(L"PoulThomsen-PC").Detach(), false);
		//domains.Add(CComBSTR(L"RyanPearse-PC").Detach(), false);
		//domains.Add(CComBSTR(L"PETERFROSTPC").Detach(), false);
		//domains.Add(CComBSTR(L"WORKGROUP").Detach(), false);


		//domains.Add(CComBSTR(L"innproc.com").Detach(), false);
		//domains.Add(CComBSTR(L"ded0024552-16").Detach(), false);	
		CComBSTR result;
		GenerateLicense_(NULL, domains, ADC_PRODUCT::ADC_CACHE_BULK, &result);
		GUID license;
		std::cout << result.m_str;
		//CComBSTR val(L"02AE9D81EC519A9F047F6639025E143F");
		result.Remove(0, 2);
		result = L"03ED5EE1BF0EEFD19617492020E03B02";
		setstring((const PUCHAR)&license, result.m_str);
		CComBSTR licensedFor(L"sl.local\r\n3dc.local");
		auto ok = LicentieCheck(&license, licensedFor);
		ATLASSERT(ok == true, L"Something terribly failed");

	return _AtlModule.WinMain(nShowCmd);
}

