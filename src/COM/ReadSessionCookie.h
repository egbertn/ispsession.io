#pragma once
#include "TextComparer.h"

MIDL_INTERFACE("a05571be-fa5d-493f-8d3a-9e6fac31ae8b")
IReadSessionCookie : public IUnknown
{
public:
    STDMETHOD(Initialize)(IRequest* request, CComBSTR& token) noexcept = 0;
    STDMETHOD(get_Item)(const VARIANT var,  BSTR  * pRet) noexcept= 0 ;
	STDMETHOD(get_HasKeys)(VARIANT_BOOL* pfHasKeys) noexcept = 0;
	STDMETHOD(get_Count)(int* cStrRet) noexcept = 0;
    STDMETHOD(get_Key)(const VARIANT VarKey, VARIANT* pvar) noexcept = 0;
};

class ATL_NO_VTABLE  ReadSessionCookie :
	public CComObjectRoot,
	public CComCoClass<ReadSessionCookie>,
	public IReadSessionCookie
{
public:

	DECLARE_NO_REGISTRY()
	DECLARE_NOT_AGGREGATABLE(ReadSessionCookie)


	BEGIN_COM_MAP(ReadSessionCookie)
		COM_INTERFACE_ENTRY(IReadSessionCookie)
	END_COM_MAP()
	//custom methods

	STDMETHOD(Initialize)(IRequest* request, CComBSTR& token) noexcept;
	STDMETHOD(get_Item)(const VARIANT var, BSTR* pRet) noexcept;
	STDMETHOD(get_HasKeys)(VARIANT_BOOL* pfHasKeys) noexcept;
	STDMETHOD(get_Count)(int* cStrRet) noexcept;
	STDMETHOD(get_Key)(const VARIANT VarKey, VARIANT* pvar) noexcept;

private:
	std::map<CComBSTR, CComBSTR, TextComparer> _dictionary;
	CComBSTR m_CookieValue;
	VARIANT_BOOL m_hasKeys;
	int m_Count;

};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(GUID_NULL, ReadSessionCookie)
