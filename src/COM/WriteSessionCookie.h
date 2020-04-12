#pragma once
#include "stdafx.h"

enum SAMESITEPOLICY
{
	Strict = 1,
	Lax = 2,
	None = 3
};
MIDL_INTERFACE("70f2328c-f21d-4072-bfce-571803c9a4c6")
IWriteSessionCookie: public IUnknown
{
	STDMETHOD(put_Item)(const BSTR key, const BSTR bstrValue)noexcept = 0;
// relative expiration in minutes
	STDMETHOD(put_Expires)(const LONG expires)noexcept = 0;
	STDMETHOD(put_Domain)(const BSTR bstrDomain)noexcept = 0;
	STDMETHOD(put_Path)(const BSTR bstrPath)noexcept = 0;
	STDMETHOD(put_Secure)(const VARIANT_BOOL fSecure) noexcept = 0;
	STDMETHOD(put_SameSite)(const SAMESITEPOLICY sameSite)noexcept = 0;
	//finishes the final cookie to be written to the client
	STDMETHOD(Flush)(IResponse* response) noexcept = 0;
};
class ATL_NO_VTABLE  WriteSessionCookie:
	public CComObjectRoot,
	public CComCoClass<WriteSessionCookie>,
	public IWriteSessionCookie
	{
	public:
		
		DECLARE_NO_REGISTRY()
		DECLARE_NOT_AGGREGATABLE(WriteSessionCookie)


		BEGIN_COM_MAP(WriteSessionCookie)
			COM_INTERFACE_ENTRY(IWriteSessionCookie)
		END_COM_MAP()
		//custom methods

		STDMETHOD(put_Item)(const BSTR key, const BSTR bstrValue) noexcept;
		STDMETHOD(put_Expires)(const LONG dtExpires) noexcept;
		STDMETHOD(put_Domain)(const BSTR bstrDomain) noexcept;
		STDMETHOD(put_Path)(const BSTR bstrPath) noexcept;
		STDMETHOD(put_Secure)(const VARIANT_BOOL fSecure) noexcept;
		STDMETHOD(put_SameSite)(const SAMESITEPOLICY pfHasKeys) noexcept;
		// writes the cookie to Set-Header 
		STDMETHOD(Flush)(IResponse* response) noexcept;
		HRESULT FinalConstruct() noexcept
		{
			m_Expires = 0;
			m_secure = VARIANT_FALSE;
			return S_OK;
		}
	private:

		CComBSTR m_CookieValue;
		CComBSTR m_CookieKey;
		VARIANT_BOOL m_secure;
		LONG m_Expires;
		CComBSTR m_Domain;
		CComBSTR m_Path;

	};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(GUID_NULL, WriteSessionCookie)