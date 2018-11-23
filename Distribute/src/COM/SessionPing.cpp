// SessionPing.cpp : Implementation of CSessionPing

#include "stdafx.h"
#include "SessionPing.h"
#include "tools.h"


STDMETHODIMP CSessionPing::OpenDBConnection() throw()
{
	CComBSTR strConstruct;
	HRESULT hr = ReadDllConfig(&strConstruct, NULL, NULL, NULL);
	CComBSTR server;
	server.Attach(strConstruct.ToByteString());
	try
	{
		m_dc = simple_pool::create((LPSTR)server.m_str); //uggh
	}
	catch (too_much_retries ex)
	{
		hr = E_FAIL; //TODO this sucks
	}
	if (hr == S_OK) m_isDBOpen = TRUE;
	return hr;
}
// CSessionPing
STDMETHODIMP CSessionPing::KeepAlive(VARIANT vSessionID) throw()
{
	//(SessionID.vt == VT_ERROR && SessionID.scode == DISP_E_PARAMNOTFOUND) 
	
	bool overRideSessionID = false;
	BYTE btOverrideSession[16] = {0};

	if (vSessionID.vt == VT_BSTR)
	{
		BOOL isvalid = setstring(btOverrideSession, vSessionID.bstrVal);
		overRideSessionID = isvalid == TRUE ;
	}
	HRESULT hr = S_OK;
	m_Success = m_isDBOpen;
	
	if (m_Success == TRUE)
	{
		CpSessionKeepAlive ping;		
		//try twice for SQL could have been restarted.
		for (int cx = 0; cx < 2; cx++)
		{
			hr = ping.Execute(m_dc, (overRideSessionID ? (GUID*)btOverrideSession : (GUID*)theCookie), &theApp);
			if (hr == E_FAIL)
			{
				m_dc.reset();
				hr = OpenDBConnection();
			}
			else break;
		}
		if (FAILED(hr))
		{
			m_Success = FALSE;
		}
	}		
	return hr;
}

STDMETHODIMP CSessionPing::OnStartPage(IUnknown* punk) throw()
{	
	HRESULT hr = S_OK;
	CComQIPtr<IScriptingContext> pscv(punk);
	CComPtr<IRequest> pRequest;
	CComPtr<IApplicationObject> pApplication;
	pscv->get_Request(&pRequest);
	pscv->get_Application(&pApplication);
	hr = Initialize(pRequest, pApplication);
	return hr;
}
STDMETHODIMP CSessionPing::OnEndPage(void) throw()
{
	return S_OK;
}

STDMETHODIMP CSessionPing::Initialize(IRequest* pRequest, IApplicationObject* pApplication) throw()
{

	HRESULT hr = S_OK;
	CComVariant vitem;

	//}
	if (SUCCEEDED(hr))
    {		

		CComPtr<IRequestDictionary> pReq;
		// Fetch the Cookies collection from the Request object ie Request.Cookies
		hr = pRequest->get_Cookies(&pReq);

		CComVariant var(L"GUID"); //TODO? Token...
		// now we are here: Request.Cookies.Item("GUID")
		hr = pReq->get_Item(var, &vitem);
		var.Clear();
		if (SUCCEEDED(hr))
		{
			CComQIPtr<IReadCookie> pReadCookie(vitem.pdispVal);
			var.vt = VT_ERROR;
			var.scode = DISP_E_PARAMNOTFOUND;
			vitem.Clear();
			hr = pReadCookie->get_Item(var, &vitem);		
			

			if (SUCCEEDED(hr))
			{
				setstring(theCookie, vitem.bstrVal);
				vitem.Clear();
			}
		}
		if (SUCCEEDED(hr))
		{				
			CComBSTR bstrProp ( L"APP_KEY");
				
			hr = pApplication->get_Value(bstrProp, &vitem);
			
			if (SUCCEEDED(hr))
			{
				setstring((const PUCHAR)&theApp, vitem.bstrVal);
				vitem.Clear();
			}			
		}
	}
	
	return hr;
}
