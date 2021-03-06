// Session.h : Declaration of the NWCSession
#pragma once

#include "resource.h"       // main symbols
#include "CSession.h"
#include "message.h"
#include "CVariantDictionary.h"


// NWCSession
class ATL_NO_VTABLE NWCSession : 
	public CComObjectRoot,
	public CComCoClass<NWCSession, &CLSID_NWCSession>,
	public IDispatchImpl<INWCSession, &IID_INWCSession, &LIBID_ISPCSession>	,	
	public ISupportErrorInfoImpl<&IID_INWCSession>
	
{
public:
	DECLARE_REGISTRY_RESOURCEID(IDR_SESSION)
	DECLARE_PROTECT_FINAL_CONSTRUCT()
	
	BEGIN_COM_MAP(NWCSession)		
		COM_INTERFACE_ENTRY(INWCSession)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)		
	END_COM_MAP()
	HRESULT FinalConstruct() throw()
	{	
		PCWSTR location = L"Session::FinalConstruct";
//#ifdef DEBUG 
		//DisableBSTRCache();
//#endif
		lngTimeout = 0;
		m_bstrToken = L"GUID";
		m_OnStartPageCalled = false;
		m_piVarDict = nullptr;
		dtExpires = 0;
		bHashsessionID =
		bIsDBOpen = 
		bErrState = blnPersistDoneByOnEndPage=	blnCancel = blnNew = blnExpired =
			 blnSnifQueryStringFirst = blnCookieNoSSL = blnLoggedOn = blnReEntrance =
			blnLiquid = blnSucceeded = blnFoundURLGuid =
			g_blnValid = bReadonly = FALSE;
		
		ZeroMemory(&oldGuid, sizeof(GUID));
		ZeroMemory(&btAppKey, sizeof(GUID));
		ZeroMemory(&guid, sizeof(GUID));

		#ifdef EXPIREAT 
		MAXTIME = EXPIREAT;
		#endif
		lngTimeOutSetting = 0;		
		HRESULT hr = S_OK;

	/*	if (diff < 0)

		{
			PCWSTR msg[] = { L"Session" };
			logModule.Write(L"Sorry, out of licenses.");
			LogMessage(MSG_OUTOF_LICENSES, msg, 1);
			Sleep(-diff * 500);
		}*/
		//early creation not in initialize!
		if (CComObject<CVariantDictionary>::CreateInstance(&m_piVarDict) == S_OK)
		{
			// refcount becomes 1 ...
			hr = m_piVarDict->AddRef();
		}
		

		return hr;
	}

	void FinalRelease() throw()
	{
		m_piResponse.Release();
		m_piServer.Release();
		m_piSession.Release();
		m_piRequest.Release();
		m_pictx.Release();
		PersistSession();//normally already executed by OnEndPage
		logModule.Close();
		RELEASE(m_piVarDict);

		strGUID.Empty();
		strCookieDOM.Empty();
		m_bstrToken.Empty();
		strAppPath.Empty();
		m_OldSessionId.Empty();
		/*if (bErrState == FALSE)
		{
			InterlockedDecrement(&dwInstanceCount);
			
			
		}*/
		/*if (bIsDBOpen == TRUE) 
			pool->;*/
		strConstruct.Empty();
		
	}

// INWCSession
private:
	CComPtr<IRequest> m_piRequest;
	CComPtr<IResponse> m_piResponse;
	CComPtr<IServer> m_piServer;	
	CComPtr<ISessionObject> m_piSession;	
	CComObject<CVariantDictionary> *m_piVarDict;
	//passed by IIS ASP Runtime to our COM component in DISP invoked OnStartPage
	CComPtr<IScriptingContext> m_pictx;

	bool m_OnStartPageCalled;
	std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration> m_startSessionRequest;

	//redis connection pool
	simple_pool::ptr_t pool;

	bool licenseOK;
	BOOL blnCancel,
		blnNew,
		blnExpired,		
		blnSnifQueryStringFirst,
		blnCookieNoSSL,
		blnLoggedOn,	
		blnReEntrance,
		blnLiquid,		
		blnSucceeded,
		blnFoundURLGuid,
		bErrState,
		g_blnValid,		
		// if TRUE then we have a clientside cursor
		//m_bCursLoc, 
		blnPersistDoneByOnEndPage,
		bHashsessionID,
		bReadonly,
		bIsDirty, bIsDBOpen; //for the application

	DATE dtExpires;
	
	// keeps track of the original record
	// to avoid concurrency problems
	BYTE m_dbTimeStamp[8];
	
	LONG lngTimeout,		
		lngTimeOutSetting;

	CComBSTR strConstruct, m_bstrToken,
		strGUID,
		strCookieDOM,
		strAppPath, m_OldSessionId;
	
	
	GUID btAppKey,
		guid, oldGuid;

#ifdef Demo 
	double MAXTIME ;
#endif

public:
	STDMETHOD( get_SessionID)(BSTR* pbstrRet);
	STDMETHOD( get_Value)(BSTR vkey, VARIANT* pVal);
	STDMETHOD( put_Value)(BSTR vkey, VARIANT newVal);
	STDMETHOD( putref_Value)(BSTR vkey, VARIANT newVal);
	STDMETHOD( get_TimeOut)(LONG* pVal);
	STDMETHOD( put_TimeOut)(LONG pVal);
	STDMETHOD( Abandon)(void);
	STDMETHOD( get_CodePage)(LONG* pVal);
	STDMETHOD( put_CodePage)(LONG newVal);
	STDMETHOD( get_LCID)(LONG* pVal);
	STDMETHOD( put_LCID)(LONG newVal);
	STDMETHOD( get_StaticObjects)(INWCVariantDictionary** pVal);
	STDMETHOD( get_Contents)(INWCVariantDictionary** pVal);
	STDMETHOD( get_IsExpired)(VARIANT_BOOL* pVal);
	STDMETHOD( get_IsNew)(VARIANT_BOOL* pVal);
	STDMETHOD( get_LiquidCookie)(VARIANT_BOOL* pVal);
	STDMETHOD( put_LiquidCookie)(VARIANT_BOOL newVal);
	STDMETHOD( get_ReEntrance)(VARIANT_BOOL* pVal);
	STDMETHOD( put_ReEntrance)(VARIANT_BOOL newVal);
	STDMETHOD( get_URL)(VARIANT strCheckA, VARIANT* pVal);
	STDMETHOD( Execute)(BSTR ToPage);
	STDMETHOD( Transfer)(BSTR ToPage);
	STDMETHOD( get_CreateInstance)(BSTR progid, IDispatch** pVal);
	STDMETHOD( put_Readonly)(VARIANT_BOOL newVal);
	STDMETHOD( Statistics)(VARIANT App_Key, VARIANT sessionID, INWCVariantDictionary** retval);
	STDMETHOD( EnsureURLCookie) ();
	STDMETHOD( get_OldSessionID)(BSTR *pVal);

private:	
	// non TLB exposed methods
	// Inits database
	STDMETHOD(localInit)();
	
	STDMETHOD(PersistSession)();
	// creates a new GUID and writes cookie, if possible
	STDMETHOD( NewID)();
	
	STDMETHOD( WriteCookie)(BSTR cookie);
	STDMETHOD( NewGuid)(GUID * guid);
	
	STDMETHOD( OnStartPage)(IUnknown* pctx);
	STDMETHOD( OnEndPage)();


	STDMETHOD_(void,InvokeOnStartPage)(void);
	STDMETHOD( Initialize)();
	STDMETHOD( ReadConfigFromWebConfig)();
	
	STDMETHOD_(void,ReadCookieFromQueryString());
};
OBJECT_ENTRY_AUTO(CLSID_NWCSession, NWCSession)