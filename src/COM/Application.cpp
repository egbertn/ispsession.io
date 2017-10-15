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
	hr = Initialize();
	
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
STDMETHODIMP NWCApplication::get_Value(BSTR bstrValue, VARIANT* pvar) throw()
{
	return S_OK;
}

STDMETHODIMP NWCApplication::put_Value(BSTR bstrValue, VARIANT var) throw()
{
	return S_OK;
}

STDMETHODIMP NWCApplication::putref_Value(BSTR bstrValue, VARIANT var) throw()
{
	return S_OK;
}

STDMETHODIMP NWCApplication::Lock() throw()
{//will perform a PUT [Guid]:lock 1 command to redis 2.2+
	return S_OK;
}

STDMETHODIMP NWCApplication::UnLock() throw()
{
	//perform a PUT [Guid]:lock 0 command
	return S_OK;
}

STDMETHODIMP NWCApplication::StaticObjects(INWCVariantDictionary **ppProperties) throw()
{
	this->Error(L"Static objects are not supported by this component", CLSID_NWCApplication, E_NOTIMPL);
	return E_NOTIMPL;
}
STDMETHODIMP NWCApplication::Contents(INWCVariantDictionary **ppProperties) throw()
{
	return S_OK;
}
