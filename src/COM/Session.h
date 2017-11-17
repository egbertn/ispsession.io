// Session.h : Declaration of the NWCSession
#pragma once

#include "resource.h"       // main symbols
#include "CSession.h"
#include "message.h"
#include "CVariantDictionary.h"


bool __stdcall LicentieCheck(GUID *licence, BSTR licenseContents);


LONG dwInstanceCount = 0; // keep track of the nr of instances that depend on connection

// NWCSession
class ATL_NO_VTABLE NWCSession : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<NWCSession, &CLSID_NWCSession>,
	public IDispatchImpl<INWCSession, &IID_INWCSession, &LIBID_ISPCSession>	,	
	public ISupportErrorInfoImpl<&IID_IDispatch>
	
{
public:
	DECLARE_OBJECT_DESCRIPTION("Session object replacement over Redis, for classic ASP ADC Cure")
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
		m_piVarDict = NULL;
		dtExpires = 0;
		bHashsessionID =
		bIsDBOpen = 
		bErrState = blnPersistDoneByOnEndPage=	blnCancel = blnNew = blnExpired =
			 blnSnifQueryStringFirst = blnCookieNoSSL = blnLoggedOn = blnReEntrance =
			blnLiquid = blnWasLiquid = blnSucceeded = blnFoundURLGuid =
			g_blnValid = bReadonly = FALSE;
		
		ZeroMemory(&oldGuid, sizeof(GUID));
		ZeroMemory(&btAppKey, sizeof(GUID));
		ZeroMemory(&guid, sizeof(GUID));

		#ifdef EXPIREAT 
		MAXTIME = EXPIREAT;
		#endif
		lngTimeOutSetting = 0;		
		
		GUID license= {0};
		CComBSTR strLicensedFor;
		int doLogging = 0;
		HRESULT hr = ReadDllConfig(&strConstruct, &lngTimeOutSetting, &doLogging, &license, &strLicensedFor);

		logModule.set_Logging(doLogging);

		logModule.Write(L"Read defaults timeout %d, dologging %d", lngTimeOutSetting, doLogging);
		licenseOK = LicentieCheck(&license, strLicensedFor);
#ifndef Demo	
		if (licenseOK == false) 
		{
			hr = CLASS_E_NOTLICENSED;
			Error(L"No valid License Found", CLSID_NWCSession, hr);			
		}
#endif

		if (SUCCEEDED(hr))
		{
			auto success= CSessionDL::OpenDBConnection(std::wstring(strConstruct), pool);
			if (!success)
			{
				hr = E_FAIL;
			}
		}
		if (FAILED(hr))
		{			
			ReportComError2(hr, location);
			bErrState = TRUE;						
		}
		else
		{
			::InterlockedIncrement(&dwInstanceCount);
		}
		int diff = MAXINSTANCES - dwInstanceCount;
		if (diff < 0)

		{
			PCWSTR msg[] = { L"Session" };
			logModule.Write(L"Sorry, out of licenses.");
			LogMessage(MSG_OUTOF_LICENSES, msg, 1);
			Sleep(-diff * 500);
		}
		//early creation not in initialize!
		if (CComObject<CVariantDictionary>::CreateInstance(&m_piVarDict) == S_OK)
		{
			// refcount becomes 1 ...
			hr = m_piVarDict->AddRef();
		}
		if (licenseOK == false)
		{
			Sleep(200);			
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
		if (bErrState == FALSE)
		{
			InterlockedDecrement(&dwInstanceCount);
			
			
		}
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
	//unique connection with sql	

	bool licenseOK;
	BOOL blnCancel,
		blnNew,
		blnExpired,		
		blnSnifQueryStringFirst,
		blnCookieNoSSL,
		blnLoggedOn,	
		blnReEntrance,
		blnLiquid,
		blnWasLiquid,
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
	STDMETHODIMP get_SessionID(BSTR* pbstrRet);
	STDMETHODIMP get_Value(BSTR vkey, VARIANT* pVal);
	STDMETHODIMP put_Value(BSTR vkey, VARIANT newVal);
	STDMETHODIMP putref_Value(BSTR vkey, VARIANT newVal);
	STDMETHODIMP get_TimeOut(LONG* pVal);
	STDMETHODIMP put_TimeOut(LONG pVal);
	STDMETHODIMP Abandon(void);
	STDMETHODIMP get_CodePage(LONG* pVal);
	STDMETHODIMP put_CodePage(LONG newVal);
	STDMETHODIMP get_LCID(LONG* pVal);
	STDMETHODIMP put_LCID(LONG newVal);
	STDMETHODIMP get_StaticObjects(INWCVariantDictionary** pVal);
	STDMETHODIMP get_Contents(INWCVariantDictionary** pVal);	
	STDMETHODIMP get_IsExpired(VARIANT_BOOL* pVal);
	STDMETHODIMP get_IsNew(VARIANT_BOOL* pVal);
	STDMETHODIMP get_LiquidCookie(VARIANT_BOOL* pVal);
	STDMETHODIMP put_LiquidCookie(VARIANT_BOOL newVal);
	STDMETHODIMP get_ReEntrance(VARIANT_BOOL* pVal);
	STDMETHODIMP put_ReEntrance(VARIANT_BOOL newVal);
	STDMETHODIMP get_URL(VARIANT strCheckA, VARIANT* pVal);
	STDMETHODIMP Execute(BSTR ToPage);
	STDMETHODIMP Transfer(BSTR ToPage);
	STDMETHODIMP get_CreateInstance(BSTR progid, IDispatch** pVal);		
	
	STDMETHODIMP put_Readonly(VARIANT_BOOL newVal);	
	
	STDMETHODIMP Statistics (VARIANT App_Key, VARIANT sessionID, INWCVariantDictionary** retval);
	STDMETHODIMP EnsureURLCookie ();
	STDMETHODIMP get_OldSessionID(BSTR *pVal);

private:	
	// non TLB exposed methods
	// Inits database
	STDMETHODIMP localInit();
	
	STDMETHODIMP PersistSession();
	// creates a new GUID and writes cookie, if possible
	STDMETHODIMP NewID();
	
	STDMETHODIMP WriteCookie(BSTR cookie);
	STDMETHODIMP NewGuid(GUID * guid);
	
	STDMETHODIMP OnStartPage(IUnknown* pctx);
	STDMETHODIMP OnEndPage();


	void __stdcall InvokeOnStartPage();
	STDMETHODIMP Initialize();
	STDMETHODIMP ReadConfigFromWebConfig();
	
	STDMETHOD_(void,ReadCookieFromQueryString());
};
OBJECT_ENTRY_AUTO(CLSID_NWCSession, NWCSession)