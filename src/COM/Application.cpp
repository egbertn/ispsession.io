#include "stdafx.h"
//TODO: 15-10-2017 make a REAL headerfile for CSessionDL because the split functions are not included at all in secundary includes.
#include "CSessionDL.h"
#include "config.h"
#include "application.h"
#include "CStream.h"
#include "tools.h"


STDMETHODIMP NWCApplication::OnStartPage(IUnknown *aspsvc) throw()
{
	logModule.Write(L"OnStartPage");
	if (aspsvc == nullptr)
	{
		return E_POINTER;
	}
	HRESULT hr = aspsvc->QueryInterface(&m_pScriptContext);
	if (FAILED(hr))
	{
		this->Error(L"Could not get ASP Scripting context", this->GetObjectCLSID(), hr);
		return hr;
	}
	if (FAILED(m_pScriptContext->get_Request(&m_piRequest))) return E_FAIL;
	if (FAILED(m_pScriptContext->get_Response(&m_piResponse))) return E_FAIL;
	if (FAILED(m_pScriptContext->get_Server(&m_piServer))) return E_FAIL;
	hr = ReadConfigFromWebConfig();
	if (FAILED(hr))
	{
		return hr;
	}

	m_OnStartPageCalled = TRUE;
	hr = InitializeDataSource();
	
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
STDMETHODIMP NWCApplication::get_Value(BSTR vkey, VARIANT* pVal) throw()
{
	HRESULT hr = S_OK;
	if (m_piVarDict != nullptr)
	{
		VARIANT vtemp = { 0 };
		vtemp.vt = VT_BSTR;
		vtemp.bstrVal = vkey;
		hr = m_piVarDict->get_Item(vtemp, pVal);
	}
	else
		hr = E_POINTER;
	return hr;
}

STDMETHODIMP NWCApplication::put_Value(BSTR vkey, VARIANT newVal) throw()
{
	HRESULT hr = S_OK;
	if (m_piVarDict != nullptr)
	{
		VARIANT vtemp = { 0 };
		vtemp.vt = VT_BSTR;
		vtemp.bstrVal = vkey;
		hr = m_piVarDict->put_Item(vtemp, newVal);
	}
	else
		hr = E_POINTER;
	return hr;
}

STDMETHODIMP NWCApplication::putref_Value(BSTR vkey, VARIANT newVal) throw()
{
	HRESULT hr = S_OK;
	if (m_piVarDict != nullptr)
	{
		VARIANT vtemp = { 0 };
		vtemp.vt = VT_BSTR;
		vtemp.bstrVal = vkey;
		hr = m_piVarDict->putref_Item(vtemp, newVal);
	}
	else
		hr = E_FAIL;
	return hr;
}

STDMETHODIMP NWCApplication::Lock() throw()
{//will perform a PUT [Guid]:lock 1 command to redis 2.2+
	
	
	CComBSTR appkey(m_AppKey);
	CComBSTR bytesAppKey;
	bytesAppKey.Attach(appkey.ToByteString());
	//https://github.com/jacket-code/redlock-cpp/blob/master/LockExample.cpp
	//dlm->Lock((PSTR)bytesAppKey, 100000, myLock);
	CComBSTR appKey(m_AppKey);

	return S_OK;	
}

STDMETHODIMP NWCApplication::UnLock() throw()
{
	
	return S_OK;
}

STDMETHODIMP NWCApplication::get_StaticObjects(INWCVariantDictionary **ppProperties) throw()
{
	this->Error(L"Static objects are not supported by this component", CLSID_NWCApplication, E_NOTIMPL);
	return E_NOTIMPL;
}
STDMETHODIMP NWCApplication::get_Contents(INWCVariantDictionary **ppProperties) throw()
{
	return S_OK;
}

STDMETHODIMP NWCApplication::ReadConfigFromWebConfig() throw()
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
		if ((exists = FileExists(retVal)) == true)
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
		logModule.Write(L"Application: searched web.Config up to: (%s) none found %x", retVal, hr);
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
	}
	ConfigurationManager config(retVal);
	const PWSTR prefix = L"ispsession_io:";

	CComBSTR bstrProp = L"APP_KEY";
	bstrProp.Insert(0, prefix);
	bstrProp.Attach(config.AppSettings(bstrProp));
	if (bstrProp.Length() > 0)
	{
		logModule.Write(L"AppKey: (%s)", bstrProp);
	}

#ifndef AppKeyNULL 
	if (setstring(reinterpret_cast<PUCHAR>(&m_AppKey), bstrProp) == FALSE)
		// we could do it without appkey it becomes GUID_NULL
		ZeroMemory(&m_AppKey, sizeof(GUID));
#endif	


	return hr;
}
STDMETHODIMP NWCApplication::InitializeDataSource() throw()
{
	int port = 5578;
	dlm->AddServerUrl("", port); //TODO: get con string
	return S_OK;
}