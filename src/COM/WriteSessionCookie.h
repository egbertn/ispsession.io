#pragma once
#include "stdafx.h"

enum SAMESITEPOLICY
{
	Strict = 1,
	Lax = 2,
	None = 3
};

class  WriteSessionCookie
{
public:

	STDMETHODIMP put_Item(const BSTR key, const BSTR bstrValue) noexcept;
	STDMETHODIMP put_Expires(const LONG dtExpires) noexcept;
	STDMETHODIMP put_Domain(const BSTR bstrDomain) noexcept;
	STDMETHODIMP put_Path(const BSTR bstrPath) noexcept;
	STDMETHODIMP put_Secure(const VARIANT_BOOL fSecure) noexcept;
	STDMETHODIMP put_SameSite(const SAMESITEPOLICY pfHasKeys) noexcept;
	// writes the cookie to Set-Header 
	STDMETHODIMP Flush(IResponse* response) noexcept;
	WriteSessionCookie() noexcept :
		m_Expires(0), m_secure(VARIANT_FALSE), m_sameSitePolicy(SAMESITEPOLICY::Lax)
	{
	}
private:

	CComBSTR m_CookieValue;
	CComBSTR m_CookieKey;
	VARIANT_BOOL m_secure;
	SAMESITEPOLICY m_sameSitePolicy;
	LONG m_Expires;
	CComBSTR m_Domain;
	CComBSTR m_Path;

};

