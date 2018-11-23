// SessionPing.h : Declaration of the CSessionPing

#pragma once
#include "resource.h"       // main symbols
#include "CSession.h"
#include "CSessionDL.h"


// CSessionPing
class ATL_NO_VTABLE CSessionPing : 
	public CComObjectRoot,
	public CComCoClass<CSessionPing, &CLSID_CSessionPing>,
	public IDispatchImpl<ISessionPing, &IID_ISessionPing, &LIBID_ISPCSession>
{
public:
	HRESULT FinalConstruct() throw()
	{
		ZeroMemory(theCookie, 16);
		ZeroMemory(&theApp, 16);
		m_isDBOpen = FALSE;
		m_Success = FALSE;
		return OpenDBConnection();
	}
	void FinalRelease()
	{
		AtlTrace(L"Closing CSessionPing...\r\n");
		m_dc.reset();
	}
	DECLARE_REGISTRY_RESOURCEID(IDR_SESSIONPING)

	//DECLARE_NOT_AGGREGATABLE(CSessionPing)

	BEGIN_COM_MAP(CSessionPing)
		COM_INTERFACE_ENTRY(ISessionPing)
		COM_INTERFACE_ENTRY(IDispatch)		
	END_COM_MAP()

	
	STDMETHOD(KeepAlive)(VARIANT SesssionID);
	STDMETHOD( OnStartPage)(IUnknown* pctx);
	STDMETHOD(OnEndPage)(void);

private:	
	STDMETHODIMP Initialize (IRequest* pRequest, IApplicationObject* pApplication);
	void __stdcall Deactivate();
	//BOOL __stdcall CanBePooled();
	//opens a connection
	STDMETHODIMP OpenDBConnection();
	//member variables
	BOOL m_Success;
	BOOL m_isDBOpen;
	BYTE theCookie[16]; 
	GUID theApp;
	simple_pool::ptr_t  m_dc;
};
OBJECT_ENTRY_AUTO(CLSID_CSessionPing, CSessionPing)
