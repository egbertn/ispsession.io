// Session.cpp : Implementation of NWCSession
// Copyright ADC Cure 2014
#include "stdafx.h"
#include "CSessionDL.h"
#include "config.h"
#include "session.h"
#include "CStream.h"
#include "tools.h"

//TODO: consider optimistic locking http://www.redis.io/topics/transactions

// will be called if from asp Server.CreateObject is used
STDMETHODIMP NWCSession::OnStartPage(IUnknown* aspsvc) throw()
{
	logModule.Write(L"OnStartPage");

	HRESULT hr = aspsvc->QueryInterface(&m_pictx);
	if (FAILED(hr))
	{
		this->Error(L"Could not get ASP Scripting context", __uuidof(IScriptingContext), hr);
		return hr;
	}
	if (FAILED(m_pictx->get_Request(&m_piRequest))) return E_FAIL;
	if (FAILED(m_pictx->get_Response(&m_piResponse))) return E_FAIL;
	if (FAILED(m_pictx->get_Server(&m_piServer))) return E_FAIL;
	hr = ReadConfigFromWebConfig();
	if (FAILED(hr))
	{
		return hr;
	}

	m_OnStartPageCalled = true;
	hr = Initialize();
	if (hr == S_OK)
	{
		InvokeOnStartPage();
	}
	if (hr == S_FALSE) 
	{
		m_piResponse->Write(CComVariant(L"Redis Server is not running."));
		hr = E_FAIL;
	}
#ifndef Demo
	if (licenseOK == false)
	{
		m_piResponse->Write(CComVariant(L"The license for this server is invalid. Please contact ADC Cure for an updated license at information@adccure.nl"));
	}
#endif
	return hr;
}
//for each object type, we need to see if there is an OnStartPage event
void __stdcall NWCSession::InvokeOnStartPage() throw()
{
	HRESULT hr = S_OK;	

	//m_piVarDict->get_Key(idx, &valKey); //works, but performancewise worse than Enum
	CComPtr<IEnumVARIANT> pEnum;
	CComPtr<IUnknown> punk ;
	CComVariant v;
	m_piVarDict->_NewEnum(&punk);
	pEnum = punk;
	punk.Release();
	punk = m_pictx;
	CComVariant arg(punk);
	while (pEnum->Next(1, &v, NULL) == S_OK)
	{
		SHORT vt;
		CComVariant valVal;
		m_piVarDict->get_VarType(v, &vt);
		if (vt == VT_DISPATCH || vt == VT_UNKNOWN) //sloppy, should only be VT_DISPATCH
		{
			m_piVarDict->get_Item(v, &valVal);
			CComQIPtr<IDispatch> pdisp(valVal.punkVal);
			if (pdisp != NULL)
			{
				hr = pdisp.Invoke1(L"OnStartPage", &arg);
				logModule.Write(L"Invoke key %s. OnStartPage result %x", v.bstrVal, hr);
			}
		}	
		v.Clear();
	}	
}
STDMETHODIMP NWCSession::OnEndPage() throw()
{
	HRESULT hr = PersistSession();
	logModule.Write(L"OnEndPage");
	return hr;
}

STDMETHODIMP NWCSession::ReadConfigFromWebConfig() throw()
{
	HRESULT hr = S_OK;
	CComBSTR retVal, configFile(L"/web.Config");
	///traverse back to root if necessary, note, UNC paths are not advisable
	CComBSTR root;
	bool exists = false;	
	hr = m_piServer->MapPath(configFile, &root);
	configFile.Insert(0, L".");
	//IIS Setting 'enable parent paths' must be enabled
	for (;;)
	{		
		retVal.Empty();
		hr = m_piServer->MapPath(configFile, &retVal);
		if (FAILED(hr))
		{
			break;
		}
		if ((exists = FileExists(retVal))== true)
		{
			break;
		}
		logModule.Write(L"logic %s, phys %s", configFile, retVal);
		//avoid going beyond the root of this IIS website
		if (CComBSTR::Compare(retVal, root, true, false, false) == 0)
		{
			break;
		}
		if (configFile.StartsWith(L"./"))
		{
			configFile.Remove(0, 2);
		}
		configFile.Insert(0, L"../");
	}	
	if (exists == FALSE)
	{
		
		exists = FileExists(root);
		if (exists == TRUE)
		{//last resort
			retVal.Attach(root.Detach());
			hr = S_OK;
		}
	}
	if (exists == FALSE || FAILED(hr))
	{
		logModule.Write(L"searched web.Config up to: (%s) none found %x", retVal, hr);
		return HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND);
	}
	ConfigurationManager config(retVal);
	
	CComBSTR bstrProp ( L"AD_PATH");	
	
	bstrProp.Attach(config.AppSettings(bstrProp));
	if (bstrProp.Length() > 0)
	{
		strAppPath.Attach(bstrProp.Detach()); //attach + detach, an ugly optimization just moving pointers instead of values...
	}
	bstrProp = L"AD_DOMAIN";
	bstrProp.Attach(config.AppSettings(bstrProp));
	if (bstrProp.Length() > 0)
	{		
		strCookieDOM.Attach(bstrProp.Detach());		
	}
	logModule.Write(L"AD_DOMAIN: (%s), AD_PATH: (%s)", strCookieDOM, strAppPath);


	bstrProp = L"NoCookie";
	bstrProp.Attach(config.AppSettings(bstrProp));
	if (bstrProp.Length() > 0 && bstrProp.IsBool())
	{		
		blnNoCookie = bstrProp.ToBool() == VARIANT_TRUE ? TRUE : FALSE;
	}
	logModule.Write(L"NoCookie (%d)", blnNoCookie);
	
	bstrProp = L"HASH_SESSIONID";
	bstrProp.Attach(config.AppSettings(bstrProp));
	if (bstrProp.Length() > 0 && bstrProp.IsBool())
	{
		bHashsessionID = bstrProp.ToBool() == VARIANT_TRUE ? TRUE: FALSE;
	}
	logModule.Write(L"HashSessionId (%d)", bHashsessionID);

	bstrProp = L"SnifQueryStringFirst";
	bstrProp.Attach(config.AppSettings(bstrProp));
	if (bstrProp.Length() > 0 && bstrProp.IsBool())
	{	
		blnSnifQueryStringFirst = bstrProp.ToBool()  == VARIANT_TRUE ? TRUE : FALSE;
	}
	logModule.Write(L"SnifQueryStringFirst (%d)", blnSnifQueryStringFirst);

	
	bstrProp = L"CookieNoSSL";
	bstrProp.Attach(config.AppSettings(bstrProp));
	if (bstrProp.Length() > 0 && bstrProp.IsBool())
	{	
		blnCookieNoSSL = bstrProp.ToBool() == VARIANT_TRUE ? TRUE : FALSE;
	}	

	bstrProp = L"CookieExpires";
	bstrProp.Attach(config.AppSettings(bstrProp));
	if (bstrProp.Length() > 0 && bstrProp.IsNumeric())
	{// could be VT_I2 or VT_I4			
		
		// equivalent to DateAdd("n", vExpires, Now()) 
		dtExpires = Now() + (bstrProp.ToDouble() * ONEMINUTE);		
		hr = bstrProp.AssignDate(dtExpires);

	}	
	bstrProp.Attach(FormatDBTimeStamp(dtExpires));
	logModule.Write(L"CookieNoSSL (%d), expiration %s", blnCookieNoSSL, bstrProp);	

	bstrProp = L"APP_KEY";
	bstrProp.Attach(config.AppSettings(bstrProp));
	if (bstrProp.Length() > 0)
		logModule.Write(L"AppKey: (%s)", bstrProp);


#ifndef AppKeyNULL 
	if (setstring(reinterpret_cast<PUCHAR>(&btAppKey), bstrProp) == FALSE)
		// we could do it without appkey it becomes GUID_NULL
		ZeroMemory(&btAppKey, sizeof(GUID));
#endif	
	

	return hr;
}
STDMETHODIMP NWCSession::Initialize() throw()
{
	HRESULT hr = S_OK;
	CComVariant vitem;
	CComBSTR bstrProp;
	::GetSystemTimeAsFileTime(&m_startSessionRequest);
	
	#ifdef Demo 
	CComBSTR strTemp;
	int t;        
	double tmpdate;
	
	tmpdate = Now();
	if (tmpdate > MAXTIME) 
	{	hr = E_ACCESSDENIED;
		Error(L"NWCTools.CSession DEMO Compatible ASP Session replacer expired! We would welcome your order at <a href=\"http://www.nieropwebconsult.nl/asp_session_manager.htm?page=order\">order here</a>", GUID_NULL, hr);				
		goto error;
	}	
	strTemp = L"NWCTools.CSession DEMO Compatible ASP Session replacer expired! We would welcome your order at <a href=\"http://www.nieropwebconsult.nl/asp_session_manager.htm?page=order\">order here</a>";
	t = strTemp.Length();	
	#endif
	if (blnSnifQueryStringFirst == TRUE)
	{
		ReadCookieFromQueryString();
	}
	VARIANT vMissing;
	vMissing.vt = VT_ERROR;
	vMissing.scode = DISP_E_PARAMNOTFOUND;

	if (blnNoCookie == FALSE && blnFoundURLGuid == FALSE) 
	{
		CComPtr<IRequestDictionary> oReqDict;
		hr = m_piRequest->get_Cookies(&oReqDict);	
		VARIANT vkey;
		vkey.vt = VT_BSTR;
		vkey.bstrVal = m_bstrToken;
		hr = oReqDict->get_Item(vkey, &vitem);			
		vkey.vt = VT_EMPTY;
		
		if (SUCCEEDED(hr))
		{
			CComQIPtr<IReadCookie> pReadCookie(vitem.pdispVal);
			vitem.Clear();
			VARIANT_BOOL hasKeys;
			pReadCookie->get_HasKeys(&hasKeys);
			if (hasKeys == VARIANT_TRUE)
			{
				INT cnt;
				pReadCookie->get_Count(&cnt);
				logModule.Write(L"IReadCookie found more cookies! %d", cnt);
				//ignore other keys, read the first one.
				VARIANT key = {0};
				key.vt = VT_I4;
				key.intVal = 1; // collections in classic asp are one based, not zero
				hr = pReadCookie->get_Item(key, &vitem);
			}
			else
			{
				hr = pReadCookie->get_Item(vMissing, &vitem);
			}
			if (SUCCEEDED(hr) && vitem.vt == VT_BSTR)				
				vitem.Detach(&strGUID);
				
			logModule.Write(L"get_Cookies with token %s Item: %s %x", m_bstrToken, strGUID, hr);
		}
	}

	#ifdef Demo 
	if (tmpdate <= MAXTIME) strTemp.Empty();    
	#endif

	if ((strGUID.IsEmpty() || IsValidHex(strGUID)==FALSE) && blnFoundURLGuid == FALSE)
	{
		ReadCookieFromQueryString();
	}

	g_blnValid = setstring(reinterpret_cast<PUCHAR>(&guid), strGUID);
	logModule.Write(L"cookie valid: %d", g_blnValid);
		
	if (guid == GUID_NULL)
		g_blnValid = FALSE;

	blnExpired = 
	blnCancel = FALSE;
	#ifdef Demo 
		if (strTemp != NULL) 
		{
			vitem.Clear();
			vitem.vt = VT_BSTR;
			vitem.bstrVal = strTemp;
			m_piResponse->Write (vitem);			
			vitem.vt = VT_EMPTY;
		}
	#endif 		
		
	if (g_blnValid  == FALSE)
	{
		blnNew = TRUE;
		RtlZeroMemory(&guid, sizeof(GUID));
	}
	else 
		oldGuid = guid;
	#ifdef Demo
	t = t / t;
	#endif
		
	
	// if it *is* NULL it should be done using put_SessionID () = "02003 etc"	
	if (SUCCEEDED(hr))
	{	logModule.Write(L"trying localInit");
		hr = localInit();
		logModule.Write(L"localinit %x", hr);
	}

	error:
	if (FAILED(hr))
	{
		ReportComError2(hr, L"Session Activate");
		bErrState = TRUE;
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
	DWORD result1 = DsGetDcNameW(NT4NETBIOSNAME, NULL, NULL, NULL, DS_DIRECTORY_SERVICE_PREFERRED | DS_RETURN_DNS_NAME, &pdomInfo);
	if (result1 != ERROR_SUCCESS)
	{
		logModule.Write(L"NO AD DOMAIN (%d)", result1);		//mostly ERROR_NO_SUCH_DOMAIN
		NETSETUP_JOIN_STATUS status;
		PWSTR wgname = NULL;
		NET_API_STATUS apistatus = ::NetGetJoinInformation(NULL, &wgname, &status);
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
	logModule.Write(L"Names %s, %s", NT4NETBIOSNAME, cwName);

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
	while(arraySize-- != 1) // line 0 contains 
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
		logModule.Write(L"Could not find licensedItem %s in allowed licensee %s", cwName, buf);			
		return false;
	}
	CComBSTR space (L" "); // reuse variable
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
	switch(licenseType)
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


/*
	STDMETHOD(GetTypeInfoCount)(UINT *pctinfo); //idispatch stubs
	STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
	STDMETHOD(GetIDsOfNames)( REFIID riid,LPOLESTR *rgszNames,UINT cNames,LCID lcid,DISPID *rgDispId);
	STDMETHOD(Invoke)(DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo,UINT *puArgErr);
*/
// implementation ISessionObject2
STDMETHODIMP NWCSession::GetTypeInfoCount2(UINT *) throw()
{
	return E_NOTIMPL;
}
STDMETHODIMP NWCSession::GetTypeInfo2(UINT , LCID , UINT **) throw()
{
	return E_NOTIMPL;
}
STDMETHODIMP NWCSession::GetIDsOfNames2 ( UINT* ,LPOLESTR *,UINT ,LCID ,DISPID *) throw()
{
	return E_NOTIMPL;
}
STDMETHODIMP NWCSession::Invoke2(DISPID ,UINT* ,LCID ,WORD ,UINT *,VARIANT *,UINT *,UINT *) throw()
{
	return E_NOTIMPL;
}

STDMETHODIMP NWCSession::get_Value2(BSTR vKey, VARIANT* pVal) throw()
{
	return get_Value(vKey, pVal);
}
STDMETHODIMP NWCSession::put_Value2(BSTR vKey, VARIANT newVal) throw()
{
	return put_Value(vKey, newVal);
}
STDMETHODIMP NWCSession::putref_Value2(BSTR vKey, VARIANT newVal) throw()
{
	return putref_Value(vKey, newVal);
}
STDMETHODIMP NWCSession::get_Timeout2(LONG *plvar) throw()
{
	return get_TimeOut(plvar);
}
STDMETHODIMP NWCSession::put_Timeout2(LONG lvar) throw()
{
	return put_TimeOut(lvar);
}
STDMETHODIMP NWCSession::Abandon2(void) throw()
{
	return Abandon();
}
STDMETHODIMP NWCSession::get_CodePage2(LONG *) throw()
{
	return E_NOTIMPL;
}
STDMETHODIMP NWCSession::put_CodePage2(LONG ) throw()
{
	return E_NOTIMPL;
}
STDMETHODIMP NWCSession::get_LCID2(LONG *pVal) throw()
{
	return get_LCID(pVal);
}
STDMETHODIMP NWCSession::put_LCID2(LONG lVal) throw()
{
	return put_LCID(lVal);
}
STDMETHODIMP NWCSession::get_StaticObjects2(IVariantDictionary2 **) throw()
{
	return E_NOTIMPL;
}

STDMETHODIMP NWCSession::get_Contents2(IVariantDictionary2 **ppret) throw()
{
	HRESULT hr= S_OK;
	
	INWCVariantDictionary* ptr;
	hr = get_Contents(&ptr);
	if (hr == S_OK)
	{
		hr = ptr->QueryInterface(ppret);
		ptr->Release();
	}
	return hr;
}

STDMETHODIMP NWCSession::get_IsExpired(VARIANT_BOOL* pVal) throw()
{
	*pVal = blnExpired == TRUE ? VARIANT_TRUE: VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP NWCSession::get_IsNew(VARIANT_BOOL* pVal) throw()
{
	*pVal = blnNew == TRUE? VARIANT_TRUE: VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP NWCSession::get_SessionID(BSTR* pVal) throw()
{
	*pVal = NULL;
	if (guid != GUID_NULL)
	{
		if (bHashsessionID == TRUE)
		{
			//put return in pVal using a temp BSTR copy			
			CComBSTR temp;
			temp =strGUID.GetHashCode();
			*pVal = temp.Detach();
		}
		else
			*pVal = strGUID.Copy();
				
	}
	return S_OK;
}


STDMETHODIMP NWCSession::get_CodePage(LONG* pVal) throw()
{	
	VARIANT vt = { 0 };
	m_piVarDict->get_Item(CComVariant(L"__cp"), &vt);

	*pVal = vt.vt == VT_I4 ? vt.intVal : CP_ACP;
	return S_OK;
}

STDMETHODIMP NWCSession::put_CodePage(LONG newVal) throw()
{
	VARIANT vt = { 0 };
	vt.vt = VT_I4;
	vt.intVal = newVal;
	m_piVarDict->put_Item(CComVariant(L"__cp"), vt);
	return S_OK;
}

STDMETHODIMP NWCSession::get_LCID(LONG* pVal) throw()
{	
	VARIANT vt = { 0 };
	m_piVarDict->get_Item(CComVariant(L"__lcid"), &vt);
	
	*pVal = vt.vt == VT_I4 ? vt.intVal : GetThreadLocale();
	return S_OK;
}

STDMETHODIMP NWCSession::put_LCID(LONG newVal) throw()
{
	
	HRESULT hr = S_OK;
	if (this->bReadonly == TRUE)
	{
		hr = E_FAIL;
		ReportComError2(hr, ERROR_READONLY);

	}
	else if (::IsValidLocale(newVal, LCID_SUPPORTED) == FALSE)
		hr = TYPE_E_UNKNOWNLCID;
	else
	{
		VARIANT vt = { 0 };
		vt.vt = VT_I4;
		vt.intVal = newVal;
		m_piVarDict->put_Item(CComVariant(L"__lcid"), vt);
		::SetThreadLocale(newVal);
	}
	return hr;
}

STDMETHODIMP NWCSession::get_StaticObjects(INWCVariantDictionary** ) throw()
{
	return E_NOTIMPL;
}
STDMETHODIMP NWCSession::get_Contents(INWCVariantDictionary** pVal) throw()
{
	HRESULT hr = S_OK;
	if (m_piVarDict != NULL)
	{
		hr = m_piVarDict->QueryInterface(pVal);
	}
	else
		hr = E_FAIL;
	return hr;
}

STDMETHODIMP NWCSession::Execute(BSTR ToPage) throw() 
{
	BOOL saveliquid = blnLiquid;
	if (ToPage == NULL || ::SysStringLen(ToPage) == 0)
	{
		return E_INVALIDARG;
	}
	blnLiquid = FALSE;
	logModule.Write(L"Session.Execute %s", ToPage);
    HRESULT hr = PersistSession();
	m_piVarDict->RemoveAll();
	//RELEASE(papp)
	blnNew = FALSE; // if we don't set this flag...	
	
	blnExpired = FALSE;
	
	if (SUCCEEDED(hr))
		hr = m_piServer->Execute( ToPage);
    //re read the session. It might have been changed in the just executed stuff
    hr = localInit();
	



	blnLiquid = saveliquid;
	return hr;
}

STDMETHODIMP NWCSession::Transfer(BSTR ToPage) throw()
{
	BOOL saveliquid = blnLiquid;
	blnLiquid = FALSE;

    HRESULT hr = PersistSession();
	m_piVarDict->RemoveAll();
	//RELEASE(papp)
	blnNew = FALSE; // if we don't set this flag...	
	
	blnExpired = FALSE;

	if (SUCCEEDED(hr))
	{
		hr = m_piServer->Transfer (ToPage);
		if (SUCCEEDED(hr)) 
			hr = localInit();
	}

	blnLiquid = saveliquid;

	return hr;
}

STDMETHODIMP NWCSession::get_URL(VARIANT strCheckA, VARIANT* pVal)  throw()
{
	bool docleanup = false;
	HRESULT hr = S_OK;
	CComVariant vcopy;
	GUID testValid;
	if (strCheckA.vt  != VT_BSTR)	
	{
		hr = vcopy.ChangeType(VT_BSTR, &strCheckA);
		if (FAILED(hr)) return hr;
		docleanup = true;
	}
	else
	{
		vcopy.bstrVal = strCheckA.bstrVal; //steal data
		vcopy.vt = VT_BSTR;
	}	

	CComBSTR workbuf (vcopy.bstrVal);
	
	bool found  =  workbuf.IndexOf('?') >= 0;  
	CComBSTR findthis(m_bstrToken);
	findthis.Append(L'='); //find GUID=
	int foundGUID = workbuf.IndexOf(findthis);

	
//if GUID is already equal and included
	// do NOT add the GUID again!
	//URL = (strCheckA + strPar + m_bstrToken + "=" + strGUID);
	bool replaceDone = false;
	if (foundGUID >=0)
	{
		//http://localhost/nocookieweb/login.asp?blaat=7&GUID=754DF26965B14CEDE77EE9914842FB02
		if ((int)workbuf.Length() >= foundGUID + 5 + 32)
		{
			CComBSTR tempGuid;
			tempGuid.Attach(workbuf.Substring(foundGUID + 5, 32));
			if (setstring((PUCHAR)&testValid, tempGuid) == TRUE)
			{
				workbuf.MergeString(foundGUID + 5, strGUID);
				replaceDone = true;
			}
		}
		//remove invalid data
		else
		{
			int nextToken = workbuf.IndexOf('&', foundGUID, true);
			if (nextToken < 0)
			{
				workbuf.Remove(foundGUID-1, workbuf.Length() - foundGUID+1);
			}
			else
			{
				workbuf.Remove(foundGUID-1, nextToken - foundGUID+1);
			}
		}
	}
	
	
	if (replaceDone == false)
	{
		PWSTR strPar = found ? L"&amp;" : L"?"; 	
		workbuf.Append(strPar);
		workbuf.AppendBSTR(m_bstrToken);
		workbuf.Append(L'=');
		workbuf += strGUID;
	}
	
	pVal->vt = VT_BSTR;
	pVal->bstrVal = workbuf.Detach();
	if (!docleanup) vcopy.vt = VT_EMPTY;
	return hr;
}

STDMETHODIMP NWCSession::get_CreateInstance(BSTR progid, IDispatch** pVal) throw()
{
	CComPtr<IDispatch>pTemp;
	
	HRESULT hr = pTemp.CoCreateInstance(progid, NULL,CLSCTX_INPROC_HANDLER | CLSCTX_INPROC_SERVER | CLSCTX_NO_CODE_DOWNLOAD);
	if (SUCCEEDED(hr))
	{
		CComQIPtr<IPersistStreamInit> pper(pTemp);		
		if (pper != NULL)
		{
			hr = pper->InitNew();		
		};
		CComQIPtr<IDispatch> pdisp(pTemp); // Script clients always will get a IDispatch
		if (pdisp != NULL)
		{
			CComVariant arg(m_pictx);
			hr = pdisp.Invoke1(L"OnStartPage", &arg); //legacy which still is supported :)
			logModule.Write(L"Invoke %s. OnStartPage %x", progid, hr);
		}
		*pVal = pTemp.Detach();
	}
	return hr;
}

STDMETHODIMP NWCSession::Abandon(void) throw() 
{
    blnCancel = TRUE;
    // extra to secure that the session cookie
    // wont be reused
	HRESULT hr = S_OK;
	if (m_piResponse != NULL)
	{	
		dtExpires = -Now();
		hr = WriteCookie(strGUID);
		dtExpires = 0;

		if (FAILED(hr)) //headers already written if Response.Buffer == false
		{
			::SetErrorInfo(0, NULL); // Clear out any stale ErrorInfos
			hr = S_OK;
		}
		m_piVarDict->RemoveAll();
	}
	return hr;
}

STDMETHODIMP NWCSession::get_TimeOut(LONG* pVal) throw()
{
	*pVal = lngTimeout;
	return S_OK;
}
STDMETHODIMP NWCSession::put_TimeOut(LONG pVal) throw()
{
	HRESULT hr = S_OK;
	if (bReadonly == TRUE)
	{
		hr = E_FAIL;
		ReportComError2(hr ,ERROR_READONLY);		
		
	}
	else 
		lngTimeout = pVal;
	return hr;
}

STDMETHODIMP NWCSession::get_LiquidCookie(VARIANT_BOOL* pVal) throw()
{
	*pVal = blnLiquid == FALSE? VARIANT_FALSE: VARIANT_TRUE;
	return S_OK;
}

STDMETHODIMP NWCSession::put_LiquidCookie(VARIANT_BOOL newVal) throw()
{
	HRESULT hr = S_OK;
	if (bReadonly == TRUE)
	{
		hr = E_FAIL;
		ReportComError2(hr ,ERROR_READONLY);	
	}
	else if (newVal == VARIANT_TRUE)
	{

		if (blnLiquid == FALSE)
		{
			blnLiquid = TRUE;
			NewID();
			WriteCookie(strGUID);
		}
	}
	else 
	{
		if (blnLiquid == TRUE) 
		{
			blnLiquid = FALSE;
			blnWasLiquid = TRUE;
			if (m_OldSessionId.Length() > 0)
			{
				m_OldSessionId.CopyTo(&strGUID);
				m_OldSessionId.Empty();
				WriteCookie(strGUID);
			}
		}
		
	}
	return hr;
}

STDMETHODIMP NWCSession::get_Value(BSTR vkey, VARIANT* pVal) throw()
{
	HRESULT hr = S_OK;
	if (m_piVarDict != NULL)
	{
		VARIANT vtemp = {0};
		vtemp.vt = VT_BSTR;
		vtemp.bstrVal = vkey;
		hr = m_piVarDict->get_Item(vtemp, pVal);
	}
	else
		hr = E_POINTER;
	return hr;	
}

STDMETHODIMP NWCSession::put_Value(BSTR vkey, VARIANT newVal) throw()
{
	HRESULT hr = S_OK;
	if (bReadonly == TRUE)
	{
		hr = E_FAIL;
		ReportComError2(hr ,ERROR_READONLY);
		Error(ERROR_READONLY);
		
	}
	else if (m_piVarDict != NULL)
	{
		VARIANT vtemp = {0};
		vtemp.vt = VT_BSTR;
		vtemp.bstrVal = vkey;
		hr = m_piVarDict->put_Item(vtemp, newVal);

		//if (bSessionSyncON == TRUE)		
		//	m_piSession->put_Value(vkey, newVal);
		
	}
	else
		hr = E_POINTER;
	return hr;
	
}
STDMETHODIMP NWCSession::putref_Value(BSTR vkey, VARIANT newVal) throw()
{
	HRESULT hr = S_OK;

	if (bReadonly == TRUE)
	{
		hr = E_FAIL;
		ReportComError2(hr ,ERROR_READONLY);		
		Error(ERROR_READONLY);
	}
	else if (m_piVarDict != NULL)
	{
		VARIANT vtemp = {0};
		vtemp.vt = VT_BSTR;
		vtemp.bstrVal = vkey;
		hr = m_piVarDict->putref_Item(vtemp, newVal);
		//if (bSessionSyncON == TRUE)		
		//	hr = m_piVarDict->putref_Item(vtemp, newVal);
	}
	else
		hr = E_FAIL;
	return hr;
}

STDMETHODIMP NWCSession::put_Readonly(VARIANT_BOOL newVal)  throw()
{
	HRESULT hr = S_OK;
	if (m_piVarDict != NULL)
	{
		BOOL isDirty = FALSE;
		m_piVarDict->isDirty(&isDirty);
		if (isDirty == TRUE && newVal == VARIANT_TRUE)
		{
			hr = E_FAIL;
			Error(L"Session is already modified", __uuidof(ISessionObject), hr);
			return hr;
		}
		hr = m_piVarDict->put_Readonly(newVal);
	}
	bReadonly = newVal == VARIANT_TRUE ? TRUE : FALSE;
	return hr;
}


// Inits database
STDMETHODIMP STDMETHODCALLTYPE NWCSession::localInit(void) throw()
{
    if (bErrState == TRUE) return S_OK;
	
    HRESULT hr = S_OK;
	
	
	bool bDidInsert = false;
#ifdef DEBUG
	CComBSTR temp2;
#endif
	PCWSTR location = L"localInit";
	CpSessionGet pgetSession;
	for(;;)
	{
		hr = pgetSession.OpenRowset(pool, (PBYTE)&btAppKey, (PBYTE)&guid);
	
		if (FAILED(hr))
		{
			logModule.Write(L"db: pgetSession.OpenRowset(g_dc) %x", hr);
			ReportComError2(hr, location);
			goto error;
		}		
		logModule.Write(L"db: found existing session %s", hr == S_FALSE ? L"false": L"true");
		//S_FALSE means go and insert
		if (hr == S_FALSE && bDidInsert == false)
		{
			logModule.Write(L"db: Insert new record");
			if (blnNew == FALSE)
				blnExpired = TRUE;			
			blnNew = TRUE;
			hr = NewID();
			oldGuid = guid;
			
			hr = CSessionDL::SessionInsert(pool, &btAppKey, &guid, lngTimeOutSetting,
				blnReEntrance , 
				blnLiquid);				
			lngTimeout = lngTimeOutSetting; //default
			logModule.Write(L"CSessionDL.SessionInsert(g_dc) %x", hr);
			
			CHECKHR2

			bDidInsert = true;

		}
		else
		{
			break;
		}
		
	}
	//we always must read the timestamp, because 'did insert' might be set to true at the very first time
	memcpy(m_dbTimeStamp, pgetSession.m_timest, sizeof(m_dbTimeStamp));
#ifdef DEBUG	
	temp2.Attach(sHexFromBt(m_dbTimeStamp, 8));
	logModule.Write(L"timestamp %s", temp2);
#endif
	CHECKHR2

	if ((blnNew == FALSE) && (blnExpired == FALSE))
	{
        if ((blnFoundURLGuid == TRUE) && (pgetSession.m_ReEntrance == VARIANT_FALSE) && (blnNoCookie == FALSE))
		{
			logModule.Write(L"found Cookie in QueryString (or in AJAX callback) but disallowed because ReEntrance=false");
            blnExpired =  blnNew = TRUE;
            hr = NewID();

            m_piVarDict->RemoveAll();
			CHECKHR
		}
	}

    //read persisted record   
	if (bDidInsert == false ) 
	{
		logModule.Write(L"CSessionDL.SessionGet expires(%d), reentrance(%d), liquid(%d), size(%d)", 
							pgetSession.m_Expires,
							-pgetSession.m_ReEntrance,
							-pgetSession.m_Liquid,
							pgetSession.m_blobLength);

	    lngTimeout = pgetSession.m_Expires;
		blnReEntrance = -pgetSession.m_ReEntrance;
		blnLiquid = -pgetSession.m_Liquid;	

	/*	int z_result = -1;*/
		//int z_result = Z_OK;
		// pgetSession.m_blobLength precaution because this seems to stay 0 on SQL NATIVE CLIENT.
		if (pgetSession.IsNULL == FALSE && pgetSession.m_blobLength > 0)
		{
			CComObject<CStream>* cseqs;
			CComObject<CStream>::CreateInstance(&cseqs);
			cseqs->AddRef();
			const INT bufSize = 0x1000;
			ULONG read = 0;
			BYTE buf[bufSize]; //stack memory
			hr = S_OK;
			do
			{
				hr = pgetSession.m_pStream->Read(buf, bufSize, &read);				
				if (read > 0)
				{
					cseqs->Write(buf, read, NULL);
				}
			} while (read != 0 && hr == S_OK);
			RELEASE(pgetSession.m_pStream)
			hr = S_OK;
			LARGE_INTEGER nl = { 0 };
			cseqs->Seek(nl, STREAM_SEEK_SET, NULL);
			hr = m_piVarDict->LocalLoad(cseqs, pgetSession.m_zLen);
			if (FAILED(hr))
			{
				logModule.Write(L"loading Session failed %x", hr);
			}
			cseqs->Release();
		}		

		// so this is a workaround
		if (blnLiquid == TRUE) 
		{
			hr = NewID();
			CHECKHR
		}

	}

	{
		VARIANT testel = { VT_EMPTY };
		CComVariant vitem(L"__lcid");
		if (m_piVarDict->get_Item(vitem, &testel) == S_OK && testel.vt == VT_I4)
		{
			::SetThreadLocale(testel.iVal);
		}
	}
	

error:
	if (FAILED(hr)) 	
		bErrState = TRUE;

	return hr;
}


//    create a new session cookie.
STDMETHODIMP NWCSession::NewID(void) throw()
{
    HRESULT hr = S_OK;
	if (blnLiquid == TRUE && blnNew == FALSE)
	{
		m_OldSessionId.AssignBSTR(strGUID);
	}
	if ((blnLiquid == TRUE) || (blnNew == TRUE))
	{
		hr = this->NewGuid(&guid);		
	}
	
	sHexFromBt(&guid, &strGUID);		
	if (blnNoCookie == FALSE && m_piResponse != NULL) 		
		hr = WriteCookie(strGUID);

	return hr;
}

STDMETHODIMP NWCSession::PersistSession(void) throw()
{
	if (blnPersistDoneByOnEndPage == TRUE)
	{
		return S_OK;
	}
	logModule.Write(L"PersistSession %d", bErrState);
	if (bErrState == TRUE) return S_OK;
	DWORD lSize =0;
	
	HRESULT hr = S_OK;
//	PWCHAR location = L"PersistSession";
	if (bReadonly == TRUE)
	{
		logModule.Write(L"Session is Readonly");
	}
	if (blnCancel == FALSE && bReadonly == FALSE)
	{
		BOOL blnIsDirty;
		m_piVarDict->isDirty(&blnIsDirty);
		CComPtr<IStream> pStream;
	/*	LONG compressedSize = 0;*/
		// new in ISP Session 5.1
		if (blnIsDirty == TRUE || blnNew == TRUE || blnExpired == TRUE) 
		{			
			hr = m_piVarDict->LocalContents(&lSize, &pStream);
		}
		/* calculate the number of MS that have expired since m_startSessionRequest*/
		FILETIME endTime;
 		::GetSystemTimeAsFileTime(&endTime);
		ULARGE_INTEGER start, end;
		memcpy(&start, &m_startSessionRequest, sizeof(FILETIME));
		memcpy(&end, &endTime, sizeof(FILETIME));
		LONG totalRequestTimeMS = (LONG) (end.QuadPart - start.QuadPart) / 10000;
		hr = CSessionDL::SessionSave(pool, (PUCHAR)&btAppKey, (PUCHAR)&oldGuid,
				lngTimeout, 
				blnReEntrance, 
				blnLiquid, 
				blnLiquid == FALSE && blnWasLiquid == FALSE ? NULL : &guid, 
				pStream,
				lSize, m_dbTimeStamp, totalRequestTimeMS);
		logModule.Write(L"CSessionDL.SessionSave timeOut (%d), reEntrance(%d), Liquid(%d), size(%d) time(%d), hr(%x)", lngTimeout, blnReEntrance, blnLiquid, lSize, totalRequestTimeMS, hr);
		pStream.Release();
	
	}
	else if (blnCancel == TRUE)
	{  
		
		hr = CSessionDL::SessionDelete(pool, (PUCHAR)&btAppKey, (PUCHAR)&oldGuid);
		logModule.Write(L"CSessionDL.SessionDelete %x", hr);
	
	}
	blnPersistDoneByOnEndPage = TRUE;
	if (FAILED(hr)) 
			bErrState = TRUE;
	logModule.Write(L"db: PersistSession result %x", hr);
	return hr;
}
STDMETHODIMP NWCSession::get_ReEntrance(VARIANT_BOOL* pVal) throw()
{
	*pVal = blnReEntrance == TRUE ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP NWCSession::put_ReEntrance(VARIANT_BOOL newVal) throw()
{
	blnReEntrance = newVal == VARIANT_FALSE ? FALSE: TRUE;
	return S_OK;
}

STDMETHODIMP NWCSession::WriteCookie(BSTR cookie) throw()
{
		
	VARIANT vMissing;
	//lend the data
	vMissing.vt = VT_BSTR;
	vMissing.bstrVal = m_bstrToken;
	CComVariant vRet;
	CComPtr<IRequestDictionary> pReq;

	HRESULT hr = m_piResponse->get_Cookies(&pReq);
	if (SUCCEEDED(hr))
	{
		hr = pReq->get_Item(vMissing, &vRet);
		pReq.Release();
	}
	
	//set item now to 'optional param' type
	vMissing.vt = VT_ERROR;
	vMissing.scode = DISP_E_PARAMNOTFOUND;

	if (vRet.vt == VT_DISPATCH)
	{
		CComQIPtr<IWriteCookie> pWriteCookie(vRet.pdispVal);			
		vRet.Clear();	

		if (strCookieDOM != NULL)
		{
			hr = pWriteCookie->put_Domain(strCookieDOM);
			logModule.Write(L"CookieDomain written %s %x", strCookieDOM, hr);
		}
		if (strAppPath != NULL)
		{
			hr = pWriteCookie->put_Path(strAppPath);
			logModule.Write(L"CookiePath %s %x", strAppPath, hr);
		}
		DATE writeExpires = 0;
		if (dtExpires > 0) 		
			writeExpires = dtExpires;
		else if (dtExpires < 0)
			writeExpires = -dtExpires -(ONEMINUTE * 5);	
		
		if (writeExpires != 0)
			hr = pWriteCookie->put_Expires(writeExpires);
		CComBSTR bstrOn;
		if (logModule.get_Logging() != 0 && dtExpires> 0)
		{			
			bstrOn.AssignDate(dtExpires);			
			logModule.Write(L"put_Expires %s %x", bstrOn, hr);
		}
		
		hr = pWriteCookie->put_Item(vMissing, cookie);
		logModule.Write( L"Cookie: %s %x", cookie, hr);			
		

		// be sure if it was 'secure' to keep it like that.
		//if (SUCCEEDED(hr)) 		
		hr = m_piRequest->get_ServerVariables(&pReq);
		CComVariant vHTTPS (L"HTTPS");
		bstrOn = L"on"; 

		//returns IStringList pointer
		hr = pReq->get_Item(vHTTPS, &vRet);

		if (vRet.vt == VT_DISPATCH)
		{
			CComQIPtr<IStringList> pStringList(vRet.pdispVal);
			vHTTPS.Clear();
			hr = pStringList->get_Item(vMissing, &vHTTPS);
			logModule.Write(L"pStringList %s", vHTTPS.bstrVal);
			if (vHTTPS.vt == VT_BSTR)
			{
				VARIANT_BOOL fSecure = (bstrOn.CompareTo(vHTTPS.bstrVal, true) == 0
					&& blnCookieNoSSL == FALSE) ?
							VARIANT_TRUE : VARIANT_FALSE;
				logModule.Write(L"IWriteCookie->put_Secure(%d)", fSecure); 
				hr = pWriteCookie->put_Secure(fSecure);
			}
		}
		
	}
	if (FAILED(hr))
	{
		ReportComError2(hr, L"WriteCookie");
	}
	return hr;
}
/*/////
 * Creates a new GUID, better than CoCreateGUID
 *////////
STDMETHODIMP NWCSession::NewGuid(GUID *val) throw ()
{
	
	HCRYPTPROV hprovider = NULL;
	HRESULT hr = S_OK;
	if (::CryptAcquireContext(&hprovider, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT) == FALSE) 
		hr = ATL::AtlHresultFromLastError();
	else
	{
		if (::CryptGenRandom(hprovider, sizeof(GUID), reinterpret_cast<PBYTE>(val)) == FALSE )
			hr = ATL::AtlHresultFromLastError();    
		::CryptReleaseContext (hprovider, 0);
	}
	return hr;
}


STDMETHODIMP NWCSession::Statistics(VARIANT vAppKey, VARIANT vSessionID, INWCVariantDictionary** retval) throw()
{
	return E_NOTIMPL;
	bool isMissing = vAppKey.vt == VT_ERROR && vAppKey.scode == DISP_E_PARAMNOTFOUND;
	bool isMissingSession = vSessionID.vt == VT_ERROR && vSessionID.scode == DISP_E_PARAMNOTFOUND;

	CComObject<CVariantDictionary> *pdir;
	logModule.Write(L"Statistics");
	HRESULT hr = CComObject<CVariantDictionary>::CreateInstance(&pdir);
	hr = pdir->QueryInterface(retval);
	CpSessionStats getStats;
	GUID session = { 0 };
	GUID guid = { 0 };
	if (!isMissingSession)
	{
		if (vSessionID.vt != VT_BSTR)
		{
			hr = E_INVALIDARG;
			Error(L"vSesionID must be a string", GetObjectCLSID(), hr);
			return hr;
		}
		
		BOOL isValid = setstring((const PUCHAR)&session, vSessionID.bstrVal);
		if (isValid != TRUE)
		{
			logModule.Write(L"Statistics valid vSessionId %d", isValid);
		}
	}
	if (!isMissing && vAppKey.vt== VT_BSTR)
	{		
		BOOL isValid = setstring((const PUCHAR)&guid, vAppKey.bstrVal);
		if (isValid != TRUE)
			hr = E_INVALIDARG;
		logModule.Write(L"Statistics valid appkey %d", isValid);
	}
	else
	{
		guid = btAppKey;
	}
	
	
	hr = getStats.OpenRowset(pool, isMissing ? (PBYTE)& btAppKey: (PBYTE)&guid, isMissingSession ? NULL : (PBYTE)&session);
	

	if (SUCCEEDED(hr))
	{
		/*hr = getStats.MoveNext();
		logModule.Write(L"db: getStats.MoveNext() %x", hr);*/
		VARIANT val = {0};
		CComVariant key;
		key = L"SessionCount";
		val.vt = VT_I4;
		val.intVal = getStats.m_SessionCount;
		pdir->put_Item(key, val);
		
		key = L"AvgStateLength";
		val.intVal = getStats.m_AvgStateLength;
		pdir->put_Item(key, val);
		val.intVal = getStats.m_AvgSessionDuration;
		
		key = L"AvgSessionDuration";
		pdir->put_Item(key, val);
		val.intVal = getStats.m_AvgSessionDuration;
		pdir->put_Item(key, val);
		
		key = L"CountExpired";
		val.intVal = getStats.m_CountExpired;
		pdir->put_Item(key, val);
		
		key = L"MaxStateLength";		
		val.intVal = getStats.m_MaxStateLength;
		pdir->put_Item(key, val);
		
		key = L"MinStateLength";
		val.intVal = getStats.m_MinStateLength;
		pdir->put_Item(key, val);		
		
		key = L"AvgCompressionRate";
		val.vt = VT_R4;
		val.fltVal = getStats.m_AvgCompressionRate;
		pdir->put_Item(key, val);
		
		val.vt = VT_I4;
		key = L"AverageRequestTime";
		val.intVal = getStats.m_AverageRequestTime;
		pdir->put_Item(key, val);

		key = L"TotalRequestTime";
		val.intVal = getStats.m_totalRequestTime;
		pdir->put_Item(key, val);

		key = L"MaxRequestTime";
		val.intVal = getStats.m_MaxRequestTime;
		pdir->put_Item(key, val);

		key = L"TotalRequests";
		val.intVal = getStats.m_TotalRequests;
		pdir->put_Item(key, val);
	}
	else
	{
		logModule.Write(L"db: getStats.OpenRowset %x", hr);
	}

	return hr;
}

STDMETHODIMP NWCSession::EnsureURLCookie() throw()
{
	HRESULT hr = S_OK;
	/*
	Dim url, QS, Guid
	url = Request.ServerVariables("URL")
	QS = Request.QueryString
	If Len(QS) > 0 Then
	    url = url + "?" + QS
	End If
	Guid = Request.QueryString("GUID")
	If Len(Guid) <> 32 Then 
	    url = Replace(Session.Url(url), "&amp;", "&")
	    Response.Redirect url
	End If
	*/
	if (m_piRequest == NULL) return E_POINTER;
	

	CComPtr<IRequestDictionary> pQueryString;
	m_piRequest->get_QueryString(&pQueryString);
	CComVariant varReturn;
	CComVariant key(m_bstrToken.m_str);	
	pQueryString->get_Item(key, &varReturn); //key = Request.QueryString(m_bstrToken)
	
	//VT_DISPATCH to VT_BSTR
	// because otherwise we would have to QI from
	// IDispatch to IStringList		
	
	CComQIPtr<IStringList> pStringList(varReturn.pdispVal);
	int intListCount = 0;
	VARIANT vtIdx;
	vtIdx.vt = VT_I4;
	vtIdx.intVal = 1;
	pStringList->get_Count(&intListCount);
	
	varReturn.Clear();
	if (intListCount == 1)
	{		
		pStringList->get_Item(vtIdx, &varReturn);
	}
	pStringList.Release();
	if (intListCount !=1 || strGUID.CompareTo(varReturn.bstrVal, true) != 0) 
	{
		varReturn.Clear();

		CComPtr<IRequestDictionary> pDict;
		hr = m_piRequest->get_ServerVariables(&pDict);
		
		key = L"URL"; //Request.QueryString("URL")
		
		pDict->get_Item(key, &varReturn);
		//VT_DISPATCH to VT_BSTR
		//varReturn.ChangeType(VT_BSTR); would be a shortcut.
		pStringList = varReturn.pdispVal;
		varReturn.Clear();	
		pStringList->get_Item(vtIdx, &varReturn);
		pStringList.Release();
		CComBSTR bstrUrl;
		varReturn.Detach(&bstrUrl);
		bstrUrl.Append(L'?'); //url = url + "?"

		bstrUrl.AppendBSTR(m_bstrToken);
		bstrUrl.Append(L'=');
		bstrUrl.AppendBSTR(strGUID);

		IUnknown* pEnum;
		pQueryString->get__NewEnum(&pEnum);
		CComQIPtr<IEnumVARIANT> pVarEnum(pEnum);
		pEnum->Release();
		
		ULONG fetched = 0;
		key.Clear();
		while(pVarEnum->Next(1, &key, &fetched) == S_OK && fetched > 0)
		{
			CComVariant varValue;
			pQueryString->get_Item(key, &varValue);
			// if guid <> "GUID" then
			if (m_bstrToken.CompareTo(key.bstrVal, true) != 0)
			{
				
				if (varValue.vt != VT_DISPATCH) return E_UNEXPECTED;
				
				pStringList = varValue.pdispVal;
				pStringList->get_Count(&intListCount);
				
				for (vtIdx.intVal = 1; vtIdx.intVal <= intListCount; vtIdx.intVal++)
				{				
					bstrUrl.Append(L'&');
					bstrUrl.AppendBSTR(key.bstrVal);
					bstrUrl.Append(L'=');				
					varValue.Clear();
					pStringList->get_Item(vtIdx, &varValue);
					CComBSTR bstrEncoded;
					m_piServer->URLEncode(varValue.bstrVal, &bstrEncoded);
					bstrUrl.AppendBSTR(bstrEncoded);
				}
				pStringList.Release();
			}		
			key.Clear();		
		}
		hr = m_piResponse->Redirect(bstrUrl);
	}
	return hr;
}

STDMETHODIMP NWCSession::get_OldSessionID(BSTR *pVal) throw()
{
	*pVal = NULL;
	if (m_OldSessionId.Length() > 0)
	{
		if (bHashsessionID == TRUE)
		{
			//put return in pVal using a temp BSTR copy			
			CComBSTR temp;
			temp = m_OldSessionId.GetHashCode();
			*pVal = temp.Detach();
		}
		else
			*pVal = m_OldSessionId.Copy();
				
	}
	else
	{
		*pVal = SysAllocString(L"0");
	}
	return S_OK;
}
STDMETHODIMP_(void) NWCSession::ReadCookieFromQueryString() throw()
{
	CComPtr<IRequestDictionary> oReqDict;
	HRESULT hr = m_piRequest->get_QueryString(&oReqDict);			
	VARIANT vMissing;
	CComVariant vitem;
	
	vMissing.vt = VT_ERROR;
	vMissing.scode = DISP_E_PARAMNOTFOUND;
	if (SUCCEEDED(hr))
	{ 
		VARIANT vkey;
		vkey.vt = VT_BSTR; //lend the data
		vkey.bstrVal = m_bstrToken;
		hr = oReqDict->get_Item(vkey, &vitem);
		logModule.Write(L"Get QueryString with key %s %x", m_bstrToken, hr);
		vkey.vt = VT_EMPTY;

		CComQIPtr<IStringList> pStringList(vitem.pdispVal);
		vitem.Clear();
		vMissing.vt = VT_I4;
		vMissing.intVal = 1; //just item 1 (index 0)
		pStringList->get_Item(vMissing, &vitem);
		//ignore invalid index, in case there really is no cookie
		BOOL isValid = vitem.vt == VT_BSTR && IsValidHex(vitem.bstrVal);
		if (isValid == TRUE)
		{					
			blnFoundURLGuid = TRUE;
			logModule.Write(L"QueryString: %s, %x", vitem.bstrVal, hr);					
			vitem.Detach(&strGUID);
			if (blnNoCookie == FALSE) 
				WriteCookie(strGUID);
		}
		else
			logModule.Write(L"tried QueryString(GUID)");	
	}
}
