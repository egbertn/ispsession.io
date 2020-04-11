#pragma once
#include "stdafx.h"

class ATL_NO_VTABLE  WriteCookie:
	public CComObjectRoot,
	public CComCoClass<WriteCookie>,
	public IWriteCookie
	{
	public:
		HRESULT FinalConstruct() noexcept;
		void FinalRelease() noexcept;
		DECLARE_NO_REGISTRY()
		DECLARE_NOT_AGGREGATABLE(WriteCookie)


		
		STDMETHOD(put_Item)(VARIANT key, BSTR bstrValue) noexcept;
		STDMETHOD(put_Expires)(DATE dtExpires) noexcept;
		STDMETHOD(put_Domain)(BSTR bstrDomain) noexcept;
		STDMETHOD(put_Path)(BSTR bstrPath) noexcept;
		STDMETHOD(put_Secure)(VARIANT_BOOL fSecure) noexcept;
		STDMETHOD(get_HasKeys)(VARIANT_BOOL* pfHasKeys) noexcept;
		STDMETHOD(get__NewEnum)(IUnknown** ppEnumReturn) noexcept;
		//custom methods
		STDMETHODIMP Initialize(IResponse* response) noexcept;
		// writes the cookie to Set-Header 
		STDMETHODIMP Flush() noexcept;

	private:

		CComPtr<IResponse> m_pResonse;
		CComBSTR m_CookieValue;
		CComBSTR m_CookieKey;
		VARIANT_BOOL m_secure;
		DATE m_Expires;
		CComBSTR m_Domain;
		CComBSTR m_Path;

	};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(GUID_NULL, WriteCookie)