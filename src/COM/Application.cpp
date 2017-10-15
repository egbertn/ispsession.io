#include "stdafx.h"
//TODO: 15-10-2017 make a REAL headerfile for CSessionDL because the split functions are not included at all in secundary includes.
#include "CSessionDL.h"
#include "config.h"
#include "application.h"
#include "CStream.h"
#include "tools.h"

STDMETHODIMP NWCApplication::get_Value(BSTR bstrValue, VARIANT* pvar) throw()
{
	return S_OK;
}

STDMETHODIMP NWCApplication::put_Value(BSTR bstrValue, VARIANT var) throw()
{
	return S_OK;
}

STDMETHODIMP NWCApplication::putref_Value(BSTR bstrValue, VARIANT var)
{
	return S_OK;
}

STDMETHODIMP NWCApplication::Lock()
{
	return S_OK;
}

STDMETHODIMP NWCApplication::UnLock()
{
	return S_OK;
}

STDMETHODIMP NWCApplication::StaticObjects(INWCVariantDictionary **ppProperties)
{
	return E_NOTIMPL;
}
STDMETHODIMP NWCApplication::Contents(INWCVariantDictionary **ppProperties)
{
	return S_OK;
}
