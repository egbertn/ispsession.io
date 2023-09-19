#pragma once
#include "TextComparer.h"

class ReadSessionCookie
{
public:

	STDMETHODIMP Initialize(IRequest* request, wstring& token) noexcept;
	STDMETHODIMP get_Item(const VARIANT var, BSTR* pRet) noexcept;
	STDMETHODIMP get_HasKeys(VARIANT_BOOL* pfHasKeys) noexcept;
	STDMETHODIMP get_Count(int* cStrRet) noexcept;
	STDMETHODIMP get_Key(const int key, BSTR* pvar) noexcept;

private:
	std::map<CComBSTR, CComBSTR, TextComparer> _dictionary;
	CComBSTR m_CookieValue;

};
