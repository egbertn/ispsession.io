// Session.cpp : Implementation of NWCSession
// Copyright ADC Cure 2002-2018
#include "stdafx.h"
#include "CSessionDL.h"
#include "ConfigurationManager.h"
#include "session.h"
#include "CStream.h"
#include "tools.h"
#include <chrono>
//TODO: consider optimistic locking http://www.redis.io/topics/transactions

// will be called if from asp Server.CreateObject is used
STDMETHODIMP NWCSession::OnStartPage(IUnknown* aspsvc) throw()
{

	HRESULT hr = aspsvc->QueryInterface(&m_pictx);
	if (FAILED(hr))
	{
		this->Error(L"Could not get ASP Scripting context", this->GetObjectCLSID(), hr);
		return hr;
	}
	if (FAILED(m_pictx->get_Request(&m_piRequest))) return E_FAIL;
	if (FAILED(m_pictx->get_Response(&m_piResponse))) return E_FAIL;
	if (FAILED(m_pictx->get_Server(&m_piServer))) return E_FAIL;
	hr = ReadConfigFromWebConfig();
	logModule.Write(L"OnStartPage");
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
STDMETHODIMP_( void) NWCSession::InvokeOnStartPage() throw()
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
	while (pEnum->Next(1, &v, nullptr) == S_OK)
	{
		SHORT vt;
		CComVariant valVal;
		m_piVarDict->get_VarType(v, &vt);
		if (vt == VT_DISPATCH || vt == VT_UNKNOWN) //sloppy, should only be VT_DISPATCH
		{
			m_piVarDict->get_Item(v, &valVal);
			CComQIPtr<IDispatch> pdisp(valVal.punkVal);
			if (pdisp != nullptr)
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
	const PWSTR prefix = L"ispsession_io:";
	CComBSTR bstrProp ( L"EnableLogging"), temp, strLicensedFor;	
	GUID license = { 0 };
	bstrProp.Insert(0, prefix);
	bstrProp.Attach(config.AppSettings(bstrProp));
	if (bstrProp.Length() > 0 && bstrProp.IsNumeric())
	{
		auto enableLogging = bstrProp.ToLong();
		logModule.set_Logging(enableLogging);
		logModule.Write(L"Read dologging %d", enableLogging);
	}

	bstrProp = L"ClassicLicense";
	bstrProp.Insert(0, prefix);
	bstrProp.Attach(config.AppSettings(bstrProp));
	if (!bstrProp.IsEmpty())
	{
		setstring((const PUCHAR)&license, bstrProp);
	}
	bstrProp = L"ClassicCsession.LIC";
	bstrProp.Insert(0, prefix);
	bstrProp.Attach(config.AppSettings(bstrProp));
	if (bstrProp.Length() != 0)
	{
		strLicensedFor.Attach(bstrProp.Detach());//move
	}

	licenseOK = LicentieCheck(&license, strLicensedFor);
#ifndef Demo	
	if (licenseOK == false)
	{
		hr = CLASS_E_NOTLICENSED;
		Error(L"No valid License Found", CLSID_NWCSession, hr);
		return hr;
	}if (licenseOK == false)
	{
		Sleep(200);
	}
#endif
	

	bstrProp = L"AD_PATH";
	bstrProp.Insert(0, prefix);
	bstrProp.Attach(config.AppSettings(bstrProp));
	if (bstrProp.Length() > 0)
	{
		strAppPath.Attach(bstrProp.Detach()); 
	}
	bstrProp = L"AD_DOMAIN";
	bstrProp.Insert(0, prefix);
	bstrProp.Attach(config.AppSettings(bstrProp));
	if (bstrProp.Length() > 0)
	{		
		strCookieDOM.Attach(bstrProp.Detach());		
	}
	bstrProp = L"DataSource";
	bstrProp.Insert(0, prefix);
	strConstruct.SetLength(512);
	auto stored = ::GetEnvironmentVariableW(bstrProp, strConstruct, 512);
	if (stored > 0)
	{
		strConstruct.SetLength(stored);		
	}
	else
	{
		strConstruct.Attach(config.AppSettings(bstrProp));
	}
	if (strConstruct.IsEmpty())
	{
		hr = HRESULT_FROM_WIN32(ERROR_CANTREAD);
		Error(L"Invalid web.Config datasource not found",this->GetObjectCLSID(), hr);
		return hr;
	}
	if (SUCCEEDED(hr))
	{
		auto success = CSessionDL::OpenDBConnection(std::wstring(strConstruct), pool);
		if (!success)
		{
			hr = E_FAIL;
		}
	}

	bstrProp = L"SessionTimeout";
	bstrProp.Insert(0, prefix);
	temp.Attach(config.AppSettings(bstrProp));
	if (temp.Length() > 0 && temp.IsNumeric())
	{
		lngTimeOutSetting = temp.ToLong();
	}
	else
	{
		lngTimeOutSetting = 20;//?? too small
	}
	

	bstrProp = L"CookieName";
	bstrProp.Insert(0, prefix);
	bstrProp.Attach(config.AppSettings(bstrProp));
	if (bstrProp.Length() > 0)
	{
		m_bstrToken.Attach(bstrProp.Detach());
	}
	logModule.Write(L"Timeout (%d), AD_DOMAIN: (%s), AD_PATH: (%s), CookieName (%s)", lngTimeout, strCookieDOM, strAppPath, m_bstrToken);

	bstrProp = L"SessionTimeout";
	bstrProp.Insert(0, prefix);
	bstrProp.Attach(config.AppSettings(bstrProp));
	if (bstrProp.Length() > 0 && bstrProp.IsNumeric())
	{
		this->lngTimeout = bstrProp.ToLong();
	}

	bstrProp = L"HASH_SESSIONID";
	bstrProp.Insert(0, prefix);
	bstrProp.Attach(config.AppSettings(bstrProp));
	if (bstrProp.Length() > 0 && bstrProp.IsBool())
	{
		bHashsessionID = bstrProp.ToBool() == VARIANT_TRUE ? TRUE: FALSE;
	}
	logModule.Write(L"HashSessionId (%d)", bHashsessionID);

	bstrProp = L"SnifQueryStringFirst";
	bstrProp.Insert(0, prefix);
	bstrProp.Attach(config.AppSettings(bstrProp));
	if (bstrProp.Length() > 0 && bstrProp.IsBool())
	{	
		blnSnifQueryStringFirst = bstrProp.ToBool()  == VARIANT_TRUE ? TRUE : FALSE;
	}
	logModule.Write(L"SnifQueryStringFirst (%d)", blnSnifQueryStringFirst);

	
	bstrProp = L"CookieNoSSL";
	bstrProp.Insert(0, prefix);
	bstrProp.Attach(config.AppSettings(bstrProp));
	if (bstrProp.Length() > 0 && bstrProp.IsBool())
	{	
		blnCookieNoSSL = bstrProp.ToBool() == VARIANT_TRUE ? TRUE : FALSE;
	}	

	bstrProp = L"CookieExpires";
	bstrProp.Insert(0, prefix);
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
	bstrProp.Insert(0, prefix);
	bstrProp.Attach(config.AppSettings(bstrProp));
	if (bstrProp.Length() != 32)
	{
		hr = E_INVALIDARG;
		this->Error(L"APP_KEY missing", this->GetObjectCLSID(), hr);
	}
	else
	{
		logModule.Write(L"AppKey: (%s)", bstrProp);
	}

	if (setstring(reinterpret_cast<PUCHAR>(&btAppKey), bstrProp) == FALSE)
	{
		hr = E_INVALIDARG;
		this->Error(L"APP_KEY missing", this->GetObjectCLSID(), hr);
	}
	

	return hr;
}
STDMETHODIMP NWCSession::Initialize() throw()
{
	HRESULT hr = S_OK;
	CComVariant vitem;
	CComBSTR bstrProp;
	m_startSessionRequest = std::chrono::system_clock::now();
	
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

	if (blnFoundURLGuid == FALSE) 
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
		if (!strTemp.IsEmpty()) 
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
		
	
	// if it *is* nullptr it should be done using put_SessionID () = "02003 etc"	
	if (SUCCEEDED(hr))
	{	logModule.Write(L"trying localInit");
		hr = localInit();
		logModule.Write(L"localinit %x", hr);
	}

	error:
	if (FAILED(hr))
	{
		//ReportComError2(hr, L"Session Activate");
		Error(L"Session Activate", this->GetObjectCLSID(), hr);
		bErrState = TRUE;
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
	*pVal = nullptr;
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
		//ReportComError2(hr, ERROR_READONLY);

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
	if (m_piVarDict != nullptr)
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
	if (ToPage == nullptr || ::SysStringLen(ToPage) == 0)
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
	
	HRESULT hr = pTemp.CoCreateInstance(progid, nullptr,CLSCTX_INPROC_HANDLER | CLSCTX_INPROC_SERVER | CLSCTX_NO_CODE_DOWNLOAD);
	if (SUCCEEDED(hr))
	{
		CComQIPtr<IPersistStreamInit> pper(pTemp);		
		if (pper != nullptr)
		{
			hr = pper->InitNew();		
		};
		CComQIPtr<IDispatch> pdisp(pTemp); // Script clients always will get a IDispatch
		if (pdisp != nullptr)
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
	if (m_piResponse != nullptr)
	{	
		dtExpires = -Now();
		hr = WriteCookie(strGUID);
		dtExpires = 0;

		if (FAILED(hr)) //headers already written if Response.Buffer == false
		{
			::SetErrorInfo(0, nullptr); // Clear out any stale ErrorInfos
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
		//ReportComError2(hr ,ERROR_READONLY);		
		
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
		//ReportComError2(hr ,ERROR_READONLY);	
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
	if (m_piVarDict != nullptr)
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
		//ReportComError2(hr ,ERROR_READONLY);
		Error(ERROR_READONLY);
		
	}
	else if (m_piVarDict != nullptr)
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
		//ReportComError2(hr ,ERROR_READONLY);		
		Error(ERROR_READONLY);
	}
	else if (m_piVarDict != nullptr)
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
	if (m_piVarDict != nullptr)
	{
		BOOL isDirty = FALSE;
		m_piVarDict->isDirty(&isDirty);
		if (isDirty == TRUE && newVal == VARIANT_TRUE)
		{
			hr = E_FAIL;
			Error(L"Session is already modified", this->GetObjectCLSID(), hr);
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
    if (bErrState == TRUE) return E_FAIL;
	
    HRESULT hr = S_OK;
	
	
	bool bDidInsert = false;
#ifdef DEBUG
	CComBSTR temp2;
#endif
	PCWSTR location = L"localInit";
	CpSessionGet pgetSession;
	for(;;)
	{
		hr = pgetSession.OpenRowset(pool, btAppKey, guid);
	
		if (FAILED(hr))
		{
			logModule.Write(L"db: pgetSession.OpenRowset(g_dc) %x", hr);
			this->bErrState = TRUE;
			//ReportComError2(hr, location);
			this->Error(L"Cannot open or initiate a connection to Redis %x", this->GetObjectCLSID(), hr);
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
        if ((blnFoundURLGuid == TRUE) && (pgetSession.m_ReEntrance == VARIANT_FALSE) )
		{
			logModule.Write(L"found Cookie in QueryString (or in AJAX callback) but disallowed because ReEntrance=false");
            blnExpired =  blnNew = TRUE;
            hr = NewID();

            m_piVarDict->RemoveAll();
			CHECKHR
		}
	}
	//always write a cookie
	
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
					cseqs->Write(buf, read, nullptr);
				}
			} while (read != 0 && hr == S_OK);
			
			hr = S_OK;
			LARGE_INTEGER nl = { 0 };
			cseqs->Seek(nl, STREAM_SEEK_SET, nullptr);
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
	WriteCookie(strGUID);

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
		
		auto totalRequestTimeMS = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now() - m_startSessionRequest).count();
		hr = CSessionDL::SessionSave(pool, (PUCHAR)&btAppKey, (PUCHAR)&oldGuid,
				lngTimeout, 
				blnReEntrance, 
				blnLiquid, 
				blnLiquid == FALSE && blnWasLiquid == FALSE ? nullptr : &guid, 
				pStream,
				lSize, m_dbTimeStamp, (LONG)totalRequestTimeMS);
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
	//m_piResponse->put_Expires(-1); classic asp already does this
	CComBSTR noCache(L"no-cache");// , pragma(L"Pragma");
	m_piResponse->put_CacheControl(noCache);
	
	//m_piResponse->AddHeader(pragma, noCache); causes duplicates no-cache, nocache etc
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

		if (!strCookieDOM.IsEmpty())
		{
			hr = pWriteCookie->put_Domain(strCookieDOM);
			logModule.Write(L"CookieDomain written %s %x", strCookieDOM, hr);
		}
		if (!strAppPath.IsEmpty())
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
	/*if (FAILED(hr))
	{
		ReportComError2(hr, L"WriteCookie");
	}*/
	return hr;
}
/*/////
 * Creates a new GUID, using rand_s and RtlGenRandom
 *////////
STDMETHODIMP NWCSession::NewGuid(GUID *val) throw ()
{
	auto max = sizeof(GUID) / sizeof(int);
	auto  guidAccessor = reinterpret_cast<PINT>(val);
	for (UINT i = 0; i < max; i++) 
	{
		unsigned int number;

		if (rand_s(&number) == 0)
		{			
			guidAccessor[i] = number;			
		}
		else
		{
			return E_FAIL;
		}
	}

	return S_OK;
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
	
	
	hr = getStats.OpenRowset(pool, isMissing ? (PBYTE)& btAppKey: (PBYTE)&guid, isMissingSession ? nullptr : (PBYTE)&session);
	

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
	if (m_piRequest == nullptr) return E_POINTER;
	

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
	*pVal = nullptr;
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
				
		}
		else
			logModule.Write(L"tried QueryString(GUID)");	
	}
}
