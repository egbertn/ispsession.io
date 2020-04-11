#include "stdafx.h"
#include "ReadSessionCookie.h"

STDMETHODIMP ReadSessionCookie::Initialize(IRequest* request, CComBSTR& token) noexcept
{
	auto hr = S_OK;
	if (request == nullptr)
	{
		return E_INVALIDARG;
	}
	CComPtr<IRequestDictionary> req;

	hr = request->get_ServerVariables(&req);
	VARIANT ret = { 0 };
	if (SUCCEEDED(req->get_Item(CComVariant(L"HTTP_COOKIE"), &ret)))
	{
		CComBSTR splitter;
		splitter.Attach(ret.bstrVal); // steal the data
		ATL::CComSafeArray<BSTR> cookies;
		cookies.Attach(splitter.Split(L"; ", true));
		CComBSTR line;
		for (LONG x = 0; x < cookies.GetCount(); x++)
		{
			ATL::CComSafeArray<BSTR> keyvalue;
			line.Attach(keyvalue.GetAt(x));
			keyvalue.Attach(line.Split(L'=', true));
			line.Detach();
			if (keyvalue.GetCount() == 2)
			{
				//this->_dictionary.insert(keyvalue.GetAt(0), keyvalue.GetAt(1));
				if (token.CompareTo(keyvalue.GetAt(0)) == 0)
				{
					m_CookieValue.AssignBSTR(keyvalue.GetAt(1));
					if (m_CookieValue.IndexOf(L"&", 0U, true) > 0)
					{
						CComSafeArray<BSTR> lines;
						CComSafeArray<BSTR> keyvalue2;
						lines.Attach(m_CookieValue.Split(L"&", true));						
						for (LONG y = 0; y < lines.GetCount(); y++)
						{
							keyvalue2.Attach(line.Split(L'=', true));
							if (keyvalue2.GetCount() == 2)
							{
								_dictionary.insert(keyvalue2.GetAt(0), keyvalue2.GetAt(1));
							}
						}
					}
					break;
				}
			}
		}
	}
	
	return hr;
}

STDMETHODIMP ReadSessionCookie::get_Item(const VARIANT item, BSTR* strRet) noexcept
{
	auto hr = S_OK;
	auto isMissing = (item.vt == VT_ERROR && item.vt == DISP_E_PARAMNOTFOUND);
	if (isMissing)
	{
		hr = m_CookieValue.CopyTo(strRet);
	}
	else if (item.vt == VT_I4 || item.vt == VT_I2)
	{
		INT here =0;
		
		for (auto it = _dictionary.begin(); it != _dictionary.end(); ++it)
		{
			if (++here == item.intVal)
			{
				it->first.CopyTo(strRet);
				break;
			}
		}
	}
	else if (item.vt == VT_BSTR)
	{
		auto findK = _dictionary.find(item.bstrVal);
		findK->second.CopyTo(strRet);
	}
	return hr;
}

STDMETHODIMP ReadSessionCookie::get_HasKeys(VARIANT_BOOL* ret) noexcept
{
	//First=firstkeyvalue&Second=secondkeyvalue 
	auto hr = S_OK;
	*ret = _dictionary.size() > 0 ? VARIANT_TRUE : VARIANT_FALSE;
	return hr;
}

STDMETHODIMP ReadSessionCookie::get__NewEnum(IUnknown** ppEnumReturn) noexcept
{
	return E_NOTIMPL;
}

STDMETHODIMP ReadSessionCookie::get_Count(int* cStrRet) noexcept
{
	auto hr = S_OK;
	*cStrRet = (int)_dictionary.size();
	return hr;
}
STDMETHODIMP ReadSessionCookie::get_Key(const VARIANT key, VARIANT* pvar) noexcept
{
	auto hr = S_OK;
	if (key.vt == VT_I2 || key.vt == VT_I4)
	{
		INT here = 1;
		for (auto it = _dictionary.begin(); it != _dictionary.end(); ++it)
		{
			if (here++ == key.intVal)
			{
				it->first.CopyTo(pvar);
				break;
			}
		}
	}
	return hr;
}