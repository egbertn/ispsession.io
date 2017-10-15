#pragma once
#include "resource.h"
#include "CSession.h"
#include "message.h"
#include "CVariantDictionary.h"
#include "tools.h"

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
		m_OnStartPageCalled = FALSE;

	}
	void FinalRelease() throw()
	{

	}
private:
	CComPtr<IScriptingContext> m_pScriptContext;
	CComObject<CVariantDictionary> *m_piVarDict;
	CComPtr<IRequest> m_piRequest;
	CComPtr<IResponse> m_piResponse;
	CComPtr<IServer> m_piServer;

	GUID m_AppKey;
	BOOL m_OnStartPageCalled ;

	STDMETHODIMP OnStartPage(IUnknown* pctx);
	STDMETHODIMP OnEndPage();

public:
	STDMETHODIMP get_Value(BSTR bstrValue, VARIANT* pvar);
	STDMETHODIMP put_Value(BSTR bstrValue, VARIANT var);
	STDMETHODIMP putref_Value(BSTR bstrValue, VARIANT var);
	STDMETHODIMP Lock();
	STDMETHODIMP UnLock();
	STDMETHODIMP StaticObjects(INWCVariantDictionary **ppProperties);
	STDMETHODIMP Contents( INWCVariantDictionary **ppProperties);

};