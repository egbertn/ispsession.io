#include "stdafx.h"
#include "ReadSessionCookie.h"

STDMETHODIMP ReadSessionCookie::Initialize(IRequest* request, CComBSTR& token) noexcept
{
	auto hr = S_FALSE;
	if (request == nullptr || token.IsEmpty())
	{
		return E_INVALIDARG;
	}
	CComPtr<IRequestDictionary> req;

	hr = request->get_ServerVariables(&req);
	CComVariant ret;
	if (SUCCEEDED(req->get_Item(CComVariant(L"HTTP_COOKIE"), &ret)))
	{
		if (ret.vt != VT_DISPATCH)
		{
			return S_FALSE;
		}
		req.Release();
		CComPtr<IStringList> pstringList;
		ret.pdispVal->QueryInterface(&pstringList);
		ret.Clear();
		int itemsFound = 0;
		bool found = false;
		pstringList->get_Count(&itemsFound);
		// we could have theoretically more HTTP headers having Cookie: 
		for (auto item = 1; item <= itemsFound && found == false; item++)
		{
			VARIANT vItemIndex, vItemVal;
			vItemIndex.vt = VT_I4;
			vItemIndex.intVal = item;
			pstringList->get_Item(vItemIndex, &vItemVal);
			CComBSTR splitter;
			splitter.Attach(vItemVal.bstrVal); // steal the data, no memory issue here
			ATL::CComSafeArray<BSTR> cookies;
			cookies.Attach(splitter.Split(L"; ", true));

			CComBSTR line;
			for (LONG x = 0; x < static_cast<LONG>(cookies.GetCount()); x++)
			{
				ATL::CComSafeArray<BSTR> keyvaluePair;
				line.Attach(cookies.GetAt(x));
				keyvaluePair.Attach(line.Split(L"=", true));
				line.Detach();
				if (keyvaluePair.GetCount() == 2)
				{
					if (token.CompareTo(keyvaluePair.GetAt(0)) == 0)
					{
						m_CookieValue.AssignBSTR(keyvaluePair.GetAt(1));
						if (m_CookieValue.IndexOf(L"&", 0U, true) > 0)
						{
							CComSafeArray<BSTR> lines;
							CComSafeArray<BSTR> keyvaluePair2;
							lines.Attach(m_CookieValue.Split(L"&", true));
							for (LONG y = 0; y < static_cast<LONG>(lines.GetCount()); y++)
							{
								line.Attach(lines.GetAt(y));
								keyvaluePair2.Attach(line.Split(L"=", true));
								line.Detach();
								if (keyvaluePair2.GetCount() == 2)
								{
									_dictionary.insert(pair<CComBSTR, CComBSTR>(keyvaluePair2.GetAt(0), keyvaluePair2.GetAt(1)));
								}
							}
						}
						found = true;
						hr = S_OK;
						break;
					}
				}
			}
		}
	}	
	return hr;
}

STDMETHODIMP ReadSessionCookie::get_Item(const VARIANT item, BSTR* strRet) noexcept
{
	auto isMissing = (item.vt == VT_ERROR && item.scode == DISP_E_PARAMNOTFOUND);
	if (isMissing)
	{
		m_CookieValue.CopyTo(strRet);
		return S_OK;
	}
	else if (item.vt == VT_I4)
	{
		INT here = 1;		
		for (auto it = _dictionary.begin(); it != _dictionary.end(); ++it)
		{
			if (here++ == item.intVal)
			{
				it->first.CopyTo(strRet);
				return S_OK;
			}
		}
	}
	else if (item.vt == VT_BSTR)
	{
		auto findK = _dictionary.find(item.bstrVal);
		if (findK != _dictionary.end())
		{
			findK->second.CopyTo(strRet);
			return S_OK;
		}
	}
	return S_FALSE;
}

STDMETHODIMP ReadSessionCookie::get_HasKeys(VARIANT_BOOL* ret) noexcept
{
	//First=firstkeyvalue&Second=secondkeyvalue 
	*ret = _dictionary.size() > 0 ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP ReadSessionCookie::get_Count(int* cStrRet) noexcept
{
	*cStrRet = (int)_dictionary.size();
	return S_OK;
}
STDMETHODIMP ReadSessionCookie::get_Key(const int key, BSTR* pvar) noexcept
{	
	INT here = 1;
	for (auto it = _dictionary.begin(); it != _dictionary.end(); ++it)
	{
		if (here++ == key)
		{
			it->first.CopyTo(pvar);
			return S_OK;
		}
	}
	
	return S_FALSE;
}