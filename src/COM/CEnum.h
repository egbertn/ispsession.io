#pragma once

class ATL_NO_VTABLE  CCEnum: 
	public IEnumVARIANT ,
	public CComObjectRoot
{
private:
	ULONG curel, maxel;
	VARTYPE vt;
	//safearray because of flexibility of data type. though, it's a hard thing to read
	SAFEARRAY *psaCopy;
	
public:

	CCEnum() throw() : curel(0), vt(VT_EMPTY), maxel(0), psaCopy(nullptr)
	  
	{
	}
	void FinalRelease() throw()
	{
		if (psaCopy != nullptr)
		{
			HRESULT hr = ::SafeArrayUnaccessData(psaCopy);
			if (hr == S_OK)
			{
				::SafeArrayDestroy(psaCopy);
			}
		}
	}
	//DECLARE_NO_REGISTRY()
	DECLARE_NOT_AGGREGATABLE(CCEnum)

	BEGIN_COM_MAP(CCEnum)
	COM_INTERFACE_ENTRY(IEnumVARIANT)
	END_COM_MAP()

	STDMETHODIMP Next(ULONG, VARIANT * rgvar, ULONG * pceltFetched) throw()
	{
		HRESULT hr = S_OK;
		ATLASSERT(rgvar != nullptr);

		if (curel < maxel)
		{
			if (pceltFetched != nullptr) *pceltFetched = 1;
			if (this->vt == VT_VARIANT)
			{				
				hr = ::VariantCopyInd(rgvar, &static_cast <VARIANT*>(this->psaCopy->pvData)[curel++]);
			}
			else if (this->vt == VT_BSTR)
			{				
				::VariantClear(rgvar);
				CComBSTR temp;
				temp.AssignBSTR(static_cast <BSTR*>(this->psaCopy->pvData)[curel++]);
				rgvar->vt = VT_BSTR;
				rgvar->bstrVal = temp.Detach();
			}
		}
		else
		{
			if (pceltFetched  != nullptr) *pceltFetched = 0;
			hr = S_FALSE;
		}
		return hr;
	}
	STDMETHODIMP Clone(IEnumVARIANT **) throw()
	{
		return E_NOTIMPL;
	}
	STDMETHODIMP Reset() throw()
	{
		curel = 0;
		return S_OK;
	}

	STDMETHODIMP Skip(ULONG celt) throw()
	{
		HRESULT hr = S_OK;		
		if (curel  + celt < maxel)	
			curel += celt;
		else
			hr = S_FALSE;	
		return hr;
	}
	// pass a VT_VARIANT safe array or a VT_BSTR
	// also SafeArrayAccessData must be called by caller
	// the enum will deref (UnaccessData) and cleanup
	HRESULT Init(SAFEARRAY *psaVariantArray) throw()
	{
		HRESULT hr = S_OK;
		if (psaVariantArray == nullptr)
			hr = E_INVALIDARG;
		else
		{
			hr = SafeArrayGetVartype(psaVariantArray, &this->vt);
			this->maxel = psaVariantArray->rgsabound->cElements ;
			this->psaCopy = psaVariantArray;
			this->curel = 0;
		}
		return hr;
	}
};