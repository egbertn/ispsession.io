// Garbage_Collect.cpp : Implementation of WinMain


#include "stdafx.h"
#include "resource.h"
#include "\Data\TFS\CSession\config.h"
#include "CService.h"
#pragma warning(disable: 4482) // nonstandard extension used: 'enum CGGarbageStatus' used in qualified name

[v1_enum] //32 bit rulez
enum CGarbageStatus
{
	run = 0,
	pauze=1,
	stop = 2,
	shutmedown=3
};

class CGarbage_CollectModule : public CAtlServiceModuleT< CGarbage_CollectModule, IDS_SERVICENAME >
{
private:
	DWORD m_iCollectTime;
	HANDLE m_ThreadHandle;
	BOOL m_DisableStats;
	HANDLE  m_hServerStopEvent;
	bool connDone;
	bool bInitDone ;
	CGarbageStatus m_ServiceStatus ;
	bool stopNow;
public :
	~CGarbage_CollectModule() throw()
	{
		if (m_ThreadHandle != NULL) 
			CloseHandle(m_ThreadHandle);
		if (m_hServerStopEvent != NULL)
			CloseHandle(m_hServerStopEvent);
	}
	CGarbage_CollectModule()  throw():   m_iCollectTime(10), m_ThreadHandle(NULL), m_hServerStopEvent(NULL),
		stopNow(false)
	{
		bInitDone = false;
		m_ServiceStatus =CGarbageStatus::run;
		//m_dwTimeOut = 60000; // one minute

		
	}
	
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_GARBAGE_COLLECT, "{22DC084D-8BEF-4A78-B2A7-F20F1DAE0F21}")

	HRESULT InitializeSecurity() throw()
	{
		// TODO : Call CoInitializeSecurity and provide the appropriate security settings for 
		// your service
		// Suggested - PKT Level Authentication, 
		// Impersonation Level of RPC_C_IMP_LEVEL_IDENTIFY 
		// and an appropiate Non NULL Security Descriptor.

		return S_OK;
	}

	void OnPauze() throw()
	{
		logModule.Write(L"OnPauze");
		m_ServiceStatus = CGarbageStatus::pauze; 
		__super::OnPause();
		if ( m_hServerStopEvent != NULL)
			SetEvent(m_hServerStopEvent);
		
	}
	void OnStop() throw()
	{
		logModule.Write(L"OnStop");
		__super::OnStop();
		m_ServiceStatus =CGarbageStatus::stop; 
		if ( m_hServerStopEvent != NULL)
		{
			stopNow= true;
			
			SetEvent(m_hServerStopEvent);
			DWORD tc = GetTickCount();
			while(m_hServerStopEvent != NULL || GetTickCount() - tc > 20000) // max 20 seconds wait
				Sleep(10);
		}
	}

	void OnContinue( ) throw( )
	{
		logModule.Write(L"OnContinue");
		__super::OnContinue();
		m_ServiceStatus =CGarbageStatus::run; 
		SetServiceStatus(SERVICE_RUNNING);
		if ( m_hServerStopEvent != NULL)
			SetEvent(m_hServerStopEvent);
	}
	/*HRESULT RegisterServer(BOOL bRegTypeLib = 0, const CLSID* pClsid = 0 )
	{
		HRESULT hr = __super::RegisterServer(BOOL bRegTypeLib, pClsid);
		SC_HANDLE handle = OpenSCManagerW(NULL, NULL, SC_MANAGER_LOCK || SC_MANAGER_CONNECT || SC_MANAGER_MODIFY_BOOT_CONFIG);
		
		lService = OpenService(handle, m_szServiceName, SERVICE_CHANGE_CONFIG)
		//OpenService(NULL, NULL
		return hr;
	}*/
	void OnShutDown() throw()
	{

		logModule.Write(L"OnShutDown");
	
		//__super::OnShutDown();
		m_ServiceStatus =CGarbageStatus::pauze; 
		if ( m_hServerStopEvent != NULL)
			SetEvent(m_hServerStopEvent);
	}
	// returns the valid networkservice name
	BSTR __stdcall NetworkService()
	{
		DWORD SidSize;
		PSID TheSID;
		//PWSTR p;
		CComBSTR retVal;
		SidSize = SECURITY_MAX_SID_SIZE;
		// Allocate enough memory for the largest possible SID.
		if(!(TheSID = ::LocalAlloc(LMEM_FIXED, SidSize)))
		{    
			logModule.Write(L"Could not allocate memory.");
			return NULL;
		}
		// Create a SID for the Everyone group on the local computer.
		if(!::CreateWellKnownSid(WinNetworkServiceSid, NULL, TheSID, &SidSize))
		{
			logModule.Write(L"CreateWellKnownSid Error %u", ::GetLastError());
		}
		else
		{
			CSid cid((const SID*)TheSID, NULL);

			retVal = cid.AccountName();			
			retVal.Insert(0, "\\");
			retVal.Insert(0, cid.Domain());
			// When done, free the memory used.
			LocalFree(TheSID);
			return retVal.Detach();
		}
		return NULL;
	}
	HRESULT RegisterAppId(bool bService = false) throw()
	{		
		HRESULT hr = S_OK;
		BOOL res = __super::RegisterAppId(bService); 
		if (bService && IsInstalled())
		{
			SC_HANDLE hSCM = ::OpenSCManagerW(NULL, NULL, SERVICE_CHANGE_CONFIG);
			SC_HANDLE hService = NULL;
			if (hSCM == NULL)
				hr = AtlHresultFromLastError();
			if (hr == S_OK)
			{
				hService = ::OpenServiceW(hSCM, m_szServiceName, SERVICE_CHANGE_CONFIG);
				if (hService != NULL)
				{
					const int m_szServiceNameLen = 4096;
					WCHAR m_szServiceName[m_szServiceNameLen]={0};
					 
					 LoadStringW(_AtlBaseModule.GetModuleInstance(), 
						 IDS_Description, m_szServiceName, m_szServiceNameLen);
					 SERVICE_DESCRIPTION sdBuf = {m_szServiceName};
					 res = ChangeServiceConfig2W(hService, SERVICE_CONFIG_DESCRIPTION, &sdBuf);
					 logModule.Write(L"Result ChangeServiceConfig2 %d %d", res, GetLastError());
					 CComBSTR userNetWorkService;
					 userNetWorkService.Attach(NetworkService());
					 logModule.Write(L"UserName %s", userNetWorkService);
					 if (userNetWorkService.Length() > 0)
					 {
						 //use Network Service instead of SYSTEM!
						 res = ChangeServiceConfigW(hService, 
							 SERVICE_NO_CHANGE, 
							 SERVICE_NO_CHANGE, 
							 SERVICE_NO_CHANGE, 
							 NULL, NULL, NULL, NULL, userNetWorkService, NULL, NULL);
						 logModule.Write(L"Result ChangeServiceConfig %d %d", res, GetLastError());
					 }
					 ::CloseServiceHandle(hService);
				}
				else
					hr = AtlHresultFromLastError();
				::CloseServiceHandle(hSCM);
			}
		}
		return hr;
	}
	HRESULT PreMessageLoop(int nShowCmd)  throw()
	{
		//problem how to set a timer, we must provide the proc
		// a pointer to this
		// we could hack it and use HWND instead?


		HRESULT hr = __super::PreMessageLoop(nShowCmd);
		// if we don't have any COM classes, RegisterClassObjects
		// retunrs S_FALSE
		// This Causes the process to terminate
		// We don't want this, so we return S_OK in this case		
		if (hr == S_FALSE) hr = S_OK;

		if (m_bService == TRUE && hr == S_OK)
		{ 
			
			m_ThreadHandle = CreateThread(NULL, 0, mainJob, this, 0, 0);
		SetServiceStatus(SERVICE_RUNNING); //FIX Vs 2010

		}
		return hr;
	}
	void __stdcall set_CollectTime(INT iCollectTime) throw()
	{
		m_iCollectTime = iCollectTime;
	}
	INT get_CollectTime() throw()
	{
		return m_iCollectTime;
	}
	void __stdcall SetEventHandleForStop(HANDLE eventHandle) throw()
	{
		m_hServerStopEvent = eventHandle;
	}
	CGarbageStatus GetServiceStatus() throw()
	{
		return this->m_ServiceStatus;
	}
	void CALLBACK  TimerProc(PVOID pdata) throw()
	{

		//		unsigned long iBufLen;
		HRESULT hr = S_OK;



		logModule.Write(L"TimerProc");
		CComBSTR bstrConnBuff;
		CComBSTR bstrKey, bstrTemp;
		VARIANT_BOOL iKeepConnection = 0;
		DWORD dwVersion = 0;
	
		
		ConfigurationManager config;
		bstrKey = L"EnableLogging";
		bstrTemp.Attach(config.AppSettings(bstrKey));
		int EnableLogging = bstrTemp.ToLong();
		if (EnableLogging < 0 || EnableLogging > 3) EnableLogging = 0;
		logModule.set_Logging(EnableLogging);

		bstrKey = L"ASPSessionConnString";
		logModule.Write(L"Configuration initialized");
		bstrConnBuff.Attach(config.AppSettings(bstrKey));
		logModule.Write(L"using connection (%s)", bstrConnBuff);
		bstrKey = L"disableStats";
		bstrTemp.Attach(config.AppSettings(bstrKey, L"false"));
		m_DisableStats = bstrTemp.ToBool();
		logModule.Write(L"Stats (%d)", m_DisableStats);
		bstrKey = L"CollectTimer";
		bstrTemp.Attach(config.AppSettings(bstrKey));
		m_iCollectTime = bstrTemp.ToLong();
		
		if (m_iCollectTime < 1000) m_iCollectTime = 1000;
		logModule.Write(L"collecttimer (%d)", m_iCollectTime);

		bstrKey = L"ASPSessionVersion";
		bstrTemp.Attach(config.AppSettings(bstrKey));
		dwVersion = bstrTemp.ToLong();
		logModule.Write(L"Version (%s)", bstrTemp);
		bstrKey = L"KeepConnection";
		bstrTemp.Attach(config.AppSettings(bstrKey));
		iKeepConnection = bstrTemp.ToBool()  == VARIANT_TRUE ? 1 : 0;
		logModule.Write(L"KeepConnection (%s)", bstrTemp);
		hr = DoADOJob(iKeepConnection, bstrConnBuff, dwVersion);
		if (hr==S_OK) 
			::SetLastError(ERROR_SUCCESS);
		
				
		return;
	}

	STDMETHODIMP DoADOJob(int iAction, PCWSTR bstrConnBuff, DWORD dwVersion) throw()
	{
		
		//	AddToMessageLog(_T("DoADOJob ran..."));
			//------------------ ADO jobs
		/* iAction 0 do work and exit
		 * iAction 1 do work, cache connection and exit
		 * iAction 2 release cached connection and exit
		 */
		HRESULT hr = S_OK;
		logModule.Write(L"DoAdoJob");
		
		static ATL::CDataConnection pConn;
		
		IErrorInfo *iErr = 0;
		
		if (iAction == 2) 
			goto cleanup2;

		if (connDone == false)
		{
			CComBSTR strConstruct(bstrConnBuff);
			if (strConstruct.IndexOf(L"ole db services") == -1)
			{
				LONG lSupport =DBPROPVAL_OS_ENABLEALL & ~DBPROPVAL_OS_TXNENLISTMENT
					& ~DBPROPVAL_OS_RESOURCEPOOLING ;
				CComBSTR buff;
				buff = lSupport;
				
				strConstruct.Append(L";");
				strConstruct.Append(L"ole db services");
				strConstruct.Append(L"=");
				strConstruct.Append(buff);
			}
			
			hr = pConn.Open(strConstruct);
			logModule.Write(L"opened using (%s) %x", strConstruct, hr);
			strConstruct.Empty();
		
		}
		if (hr == S_OK) 
		{
			if (bInitDone == false)
			{
				bInitDone = true;
				CpCreateStuff createit;
				hr = createit.Execute(pConn);
				logModule.Write(L"result creating the muke %x", hr);
				createit.ReleaseCommand();
			}
			connDone = true;
			if (m_DisableStats == FALSE)
			{
				CpCountSessionsPerApp stats;
				hr = stats.Execute(pConn);	
				if (hr == DB_E_NOTABLE)
				{
					bInitDone = false;
				}
				stats.ReleaseCommand();
				logModule.Write(L"stats updated %x", hr);
			}
			CpFetchStuff fetch;
			
			if (hr == S_OK)
				hr = fetch.OpenRowset(pConn);
			if (hr == DB_E_NOTABLE)
			{
				bInitDone = false;
			}
			while (SUCCEEDED(hr) )
			{
				if (stopNow == true)
				{
					iAction = 0;
					break;
				}
				if (this->m_status.dwCurrentState != SERVICE_RUNNING)  break;
				hr = fetch.MoveNext();
				if (hr ==DB_S_ENDOFROWSET )
				{
					hr = S_OK;
					break;
				}
				if (FAILED(hr))  break;
				//hr = fetch.Delete();
				CpDellStuff del;		
				hr = del.Execute(pConn, fetch.m_App_Key, fetch.m_GUID);
				
				logModule.Write(L"deleted a session %08x", hr);
				
				del.ReleaseCommand();

			}
			if (fetch.GetNumAccessors() > 0) {		
				fetch.ReleaseCommand();
				fetch.ClearRecordMemory();
				fetch.Close();			}
		}

		if (hr != S_OK) 
		{
			
			if (GetErrorInfo(NULL, &iErr) == S_OK) 
			{ 
				BSTR bstrDescr=NULL;
				iErr->GetDescription(&bstrDescr);
				::SetLastError(hr);
				LogEventEx(MSG_ERROR, bstrDescr);
				::SysFreeString(bstrDescr);
				iErr->Release();
			}
			else
			{
				WCHAR msg[MAX_PATH] = L"DoADOJob hex: ";
				WCHAR num[10] = {0};
				_ltow_s(hr, num, 10, 16);  //hex
				wcscat_s(msg, MAX_PATH, num);				
				LogEventEx(MSG_ERROR, msg);
			}

		}
	cleanup2:		
		if ((iAction == 0 || iAction == 2) || (hr != S_OK) || (this->m_status.dwCurrentState ==SERVICE_STOP_PENDING || 
												this->m_status.dwCurrentState == SERVICE_STOPPED)) 
		{		
			
			pConn.CloseDataSource();	
			logModule.Write(L"Closed datasource");
			connDone = false;
			

		}
		SetLastError(hr);
		return hr;
	}

};

CGarbage_CollectModule _AtlModule;

DWORD WINAPI mainJob(LPVOID lpThreadParameter) throw()
{
	LARGE_INTEGER liDueTime;
	//fetch the pointer to our main class
	CGarbage_CollectModule* p = static_cast<CGarbage_CollectModule*>(lpThreadParameter);
	logModule.Write(L"mainJob");
	
	DWORD dwError = ERROR_SUCCESS;
	//optimize memory usage
	// should automatically increase if needed
	//::SetProcessWorkingSetSize(GetCurrentProcess(), 1280000, 2560000);
  
	DWORD dwWait =0;
	
	HANDLE hServerStopEvent = CreateEventW(
                                 NULL,    // no security attributes
                                 TRUE,    // manual reset event
                                 FALSE,   // not-signalled
                                 NULL);   // no name
	p->SetEventHandleForStop(hServerStopEvent);
	// array with 2 events we want to monitors
	HANDLE          hEvents[2] = {hServerStopEvent, 
						::CreateWaitableTimer(NULL, TRUE, L"ASP_Session_Collect") };

	HRESULT hr =::CoInitializeEx(NULL, COINIT_DISABLE_OLE1DDE | COINIT_MULTITHREADED);		
	
	for (;;) //endless loop
	{
		INT iCollectTime = p->get_CollectTime();
		liDueTime.QuadPart=-(LONGLONG) iCollectTime * 10000;
		//this timer is not periodic and is recreated each at loop so a negatieve value means a 'relative time'.
		
		if (::SetWaitableTimer(hEvents[1], &liDueTime,0, NULL, NULL, FALSE) == FALSE)
		{
			dwError = ::GetLastError();
			//MessageBox(NULL, _T("CreateTimerFailed"), _T("Error"), MB_OK | MB_ICONERROR);
			goto cleanup;
		}

		dwWait = ::WaitForMultipleObjects( 2, hEvents, FALSE, INFINITE);
		if (dwWait == WAIT_FAILED)
		{
			logModule.Write(L"Stopping in mainjob because of error");
			break;
		}
		else if (dwWait == WAIT_OBJECT_0+1) 		
			
			p->TimerProc(NULL);// no need to suspend timer sinced this is recreated each time			
		
		else if (dwWait == WAIT_OBJECT_0) // it was not the timer but a service event
		{
			CGarbageStatus status = p->GetServiceStatus();
			if (status==CGarbageStatus::stop || status == CGarbageStatus::shutmedown )
			{
				logModule.Write(L"Stopping in mainjob");
				break;
			}
			while (status == CGarbageStatus::pauze)
			{
				logModule.Write(L"Pauzing in mainjob");
				status = p->GetServiceStatus();
				Sleep(1000); //wait 1 second
			}
			if (status==CGarbageStatus::stop || status == CGarbageStatus::shutmedown )
			{
				logModule.Write(L"Stopping in mainjob");
				break;
			}
			// not timer event - error occurred,
		}
   }

cleanup:
	p->DoADOJob(2, NULL, 0);
	CoUninitialize();

   if (hEvents[1]) 
	   ::CloseHandle(hEvents[1]);

     if (hServerStopEvent != NULL)
   {
	   CloseHandle(hServerStopEvent);
	   hServerStopEvent=NULL;
	   p->SetEventHandleForStop(hServerStopEvent);
   }

	return 0;
}

//
extern "C" int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR lpCmdLine, int nShowCmd) throw()
{   
    return _AtlModule.WinMain(nShowCmd);
}