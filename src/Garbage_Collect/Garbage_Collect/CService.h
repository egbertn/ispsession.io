#include "..\..\tools.h"
class _CpFetchStuffAccessor
{
public:
	BYTE m_App_Key[16];
	BYTE m_GUID[16];
	BEGIN_COLUMN_MAP(_CpFetchStuffAccessor)
		COLUMN_ENTRY(1, m_App_Key)
		COLUMN_ENTRY(2, m_GUID)
	END_COLUMN_MAP()
};

class CpFetchStuff: 
	public CCommand<CAccessor<_CpFetchStuffAccessor>>
{

public:
	HRESULT OpenRowset(const CSession& session) throw()
	{	
		CDBPropSet propset(DBPROPSET_ROWSET);
		propset.AddProperty(DBPROP_COMMANDTIMEOUT, (LONG)30);
		HRESULT hr = Open(session, L"EXEC dbo.FetchAllTimeOut", &propset);
		#ifdef _DEBUG
        if(FAILED(hr))
            AtlTraceErrorRecords(hr);
		#endif        
		return hr;
	}
};


class CpCountSessionsPerApp: 
	public CCommand<CNoAccessor, CNoRowset>
{
public:
	HRESULT Execute(const CSession& session) throw()
	{
		CDBPropSet propset(DBPROPSET_ROWSET);     
		propset.AddProperty(DBPROP_COMMANDTIMEOUT, (LONG)30);		
	  
		HRESULT hr = Open(session, "EXEC dbo.pSessionUpdateStats", &propset);
		#ifdef _DEBUG
        if(FAILED(hr))
            AtlTraceErrorRecords(hr);
		#endif        
		return hr;
	}
};
class CpCreateStuff :
	public CCommand <CNoAccessor, CNoRowset>
{
public:
	HRESULT Execute(const CSession& session) throw()
	{
		CDBPropSet propset(DBPROPSET_ROWSET);
		propset.AddProperty(DBPROP_COMMANDTIMEOUT, (LONG)30);		

		CComBSTR sql(L"EXEC dbo.CreateTempTables");
		HRESULT hr = Open(session, sql, &propset);
#ifdef _DEBUG
		 if (FAILED(hr))
			AtlTraceErrorRecords(hr);
#endif        
		return hr;
	}
};
class CpDellStuff: 
	public CCommand<CNoAccessor, CNoRowset>
{
public:
	HRESULT Execute(const CSession& session, PUCHAR pAppKey, PUCHAR pGUID) throw()
	{
		CDBPropSet propset(DBPROPSET_ROWSET);     
		propset.AddProperty(DBPROP_COMMANDTIMEOUT, (LONG)30);	
		CComBSTR sql, sAppKey, sGuid;
		
		sAppKey.Attach(sHexFromBt(pAppKey));
		sGuid.Attach(sHexFromBt(pGUID));
		sql.Format(L"EXEC dbo.pSessionDelete %s,%s", sAppKey, sGuid);

		HRESULT hr = Open(session, sql, &propset);
		#ifdef _DEBUG
        if(FAILED(hr))
            AtlTraceErrorRecords(hr);
		#endif        
		return hr;
	}
	
};


void CALLBACK  TimerProc(PVOID pdata);
DWORD WINAPI mainJob(LPVOID lpThreadParameter) ;
// our global flag, that indicates all threads to stop/pauze or not.
