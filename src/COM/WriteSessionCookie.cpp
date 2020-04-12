#include "stdafx.h"
#include "WriteSessionCookie.h"
//#include <winhttp.h>

STDMETHODIMP WriteSessionCookie::put_Item(const BSTR key, const BSTR bstrValue) noexcept
{
	if (bstrValue == nullptr)
	{
		return E_INVALIDARG;
	}
	m_CookieKey.AssignBSTR(key);
	m_CookieValue.AssignBSTR(bstrValue);
	
	return S_OK;
}
// in minutes
STDMETHODIMP WriteSessionCookie::put_Expires(const LONG dtExpires) noexcept
{
	m_Expires = dtExpires;
	return S_OK;

}
STDMETHODIMP WriteSessionCookie::put_Domain(const BSTR bstrDomain) noexcept
{
	m_Domain.AssignBSTR(bstrDomain);
	return S_OK;

}
STDMETHODIMP WriteSessionCookie::put_Path(const BSTR bstrPath) noexcept
{
	m_Path.AssignBSTR(bstrPath);
	return S_OK;
}
STDMETHODIMP WriteSessionCookie::put_Secure(const VARIANT_BOOL fSecure) noexcept
{
	m_secure = fSecure;
	return S_OK;

}
STDMETHODIMP WriteSessionCookie::put_SameSite(const SAMESITEPOLICY ) noexcept
{
	
	return S_OK;

}

STDMETHODIMP WriteSessionCookie::Flush(IResponse* response) noexcept
{
	ATL::CComSafeArray<BSTR> items;
	CComBSTR buf;
	buf.Format(L"%s=%s", m_CookieKey.m_str, m_CookieValue.m_str);
	
	//auto dateFormst = L"<day-name>, <day> <month> <year> <hour>:<minute>:<second> GMT";
	//   Sun, 06 Nov 1994 08:49:37 GMT    ; IMF-fixdate
	items.Add(buf);
	/*if (m_Expires > 0.0)
	{
		//When an Expires date is set, the deadline is relative to the client the cookie is being set on, not the server.
		// ergo: WE cannot use this feature! You first have to get the client time, and then add to it.
		SYSTEMTIME st;
		::VariantTimeToSystemTime(m_Expires, &st);
		WCHAR pwszTimeStr[WINHTTP_TIME_FORMAT_BUFSIZE / sizeof(WCHAR)] = { 0 };
		::WinHttpTimeFromSystemTime(&st, pwszTimeStr);
		buf.Empty();
		buf.Append(L"Expires=");
		buf.Append(pwszTimeStr);
		items.push_back(buf);
		delete[] pwszTimeStr;
	}*/
	if (!m_Path.IsEmpty())
	{
		buf.Format(L"Path=%s", m_Path);
		items.Add(buf);
	}
	if (!m_Domain.IsEmpty())
	{
		buf.Format(L"Domain=%s", m_Domain.m_str);
		items.Add(buf);
	}
	if (m_Expires > 0)
	{
		/*
		Max-Age=<number> Optional
		Number of seconds until the cookie expires. A zero or negative number will expire the cookie immediately. If both Expires and Max-Age are set, Max-Age has precedence.
		*/
		buf.Format(L"Max-Age=%d", (m_Expires * 60)); //convert to seconds

		items.Add(buf);
	}
	else if (m_Expires < 0)
	{
		buf.Format(L"Max-Age=0");
		items.Add(buf);
	}
	if (m_secure == VARIANT_TRUE)
	{
		items.Add(CComBSTR(L"Secure"));
	}
	items.Add(CComBSTR(L"SameSite=Lax"));
	
	buf.Attach(CComBSTR::Join(items.m_psa, CComBSTR(L"; ")));
	
	response->AddHeader(CComBSTR(L"Set-Cookie"), buf);
	return S_OK;
}