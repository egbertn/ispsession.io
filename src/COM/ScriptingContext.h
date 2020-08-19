#pragma once

class ATL_NO_VTABLE  ScriptingContext :
	public CComObjectRoot,
	public CComCoClass<ScriptingContext>,
	public IDispatchImpl<IScriptingContext>
{

public:

	DECLARE_NO_REGISTRY()
	DECLARE_NOT_AGGREGATABLE(ScriptingContext)
	void FinalRelease();

	BEGIN_COM_MAP(ScriptingContext)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IScriptingContext)
	END_COM_MAP()
	STDMETHOD(get_Request)(IRequest** ppRequest) noexcept;
	STDMETHOD(get_Response)(IResponse** ppResponse) noexcept;
	STDMETHOD(get_Server)(IServer** ppServer) noexcept;
	STDMETHOD(get_Session)(ISessionObject** ) noexcept;
	STDMETHOD(get_Application)(IApplicationObject** ) noexcept;

	STDMETHODIMP Initialize(IDispatch* iRequest, IDispatch* iSErver, IDispatch* iResponse) noexcept;

private:
	CComPtr<IRequest> m_piRequest;
	CComPtr<IServer> m_piServer;
	CComPtr<IResponse> m_piREsponse;



};
OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(GUID_NULL, ScriptingContext)