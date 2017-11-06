#pragma once
#include "resource.h"
#include "CSession.h"
#include "message.h"
#include "CVariantDictionary.h"
#include <atlsync.h>
#include "tools.h"
#include "CRedLock.h"
class ATL_NO_VTABLE NWCApplication :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<NWCApplication, &CLSID_NWCApplication>,
	public IDispatchImpl<INWCApplicationObject, &IID_INWCApplicationObject, &LIBID_ISPCSession>,
	public ISupportErrorInfoImpl<&IID_IDispatch>

{
public:
	DECLARE_REGISTRY_RESOURCEID(IDR_APPLICATION)
	DECLARE_PROTECT_FINAL_CONSTRUCT();

	BEGIN_COM_MAP(NWCApplication)
		COM_INTERFACE_ENTRY(INWCApplicationObject)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)
	END_COM_MAP()

	HRESULT FinalConstruct() throw()
	{
		PCWSTR location = L"FinalConsturct";
		m_OnStartPageCalled = FALSE;
		m_bErrState = FALSE;
		HRESULT hr = S_OK;
		//early creation not in initialize!
		if (CComObject<CVariantDictionary>::CreateInstance(&m_piVarDict) == S_OK)
		{
			// refcount becomes 1 ...
			hr = m_piVarDict->AddRef();
		}
		int doLogging = 0;
		hr = ReadConfigFromWebConfig();

		if (SUCCEEDED(hr))
		{
			auto success = CSessionDL::OpenDBConnection(std::wstring(m_strConstruct), pool);
			if (!success)
			{
				hr = E_FAIL;
			}
		}
		if (FAILED(hr))
		{
			ReportComError2(hr, location);
			m_bErrState = TRUE;
		}
		dlm = new CRedLock();
		return hr;
	}
	void FinalRelease() throw()
	{
		m_piResponse.Release();
		m_piServer.Release();
		m_piRequest.Release();
		m_pScriptContext.Release();
		delete dlm;

	}
private:
	CComPtr<IScriptingContext> m_pScriptContext;
	CComObject<CVariantDictionary> *m_piVarDict;
	CComPtr<IRequest> m_piRequest;
	CComPtr<IResponse> m_piResponse;
	CComPtr<IServer> m_piServer;
	CRedLock  * dlm;
	CLock my_lock;
	CComBSTR m_strConstruct;
	//redis connection pool
	simple_pool::ptr_t pool;
	BOOL m_bErrState;
	GUID m_AppKey;
	BOOL m_OnStartPageCalled ;

	STDMETHODIMP OnStartPage(IUnknown* pctx);
	STDMETHODIMP OnEndPage();
	STDMETHODIMP ReadConfigFromWebConfig();
	STDMETHODIMP InitializeDataSource();

public:
	STDMETHOD (get_Value)(BSTR bstrValue, VARIANT* pvar);
	STDMETHOD( put_Value)(BSTR bstrValue, VARIANT var);
	STDMETHOD( putref_Value)(BSTR bstrValue, VARIANT var);
	STDMETHOD( Lock)();
	STDMETHOD( UnLock)();
	STDMETHOD( get_StaticObjects)(INWCVariantDictionary **ppProperties);
	STDMETHOD( get_Contents)( INWCVariantDictionary **ppProperties);

};

OBJECT_ENTRY_AUTO(CLSID_NWCApplication, NWCApplication)