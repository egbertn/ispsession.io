#pragma once
#include "resource.h"
#include "CSession.h"
#include "message.h"
#include <chrono>


#include "tools.h"
#include "CRedLock.h"
struct ARRAY_DESCRIPTOR
{
	VARTYPE type;
	LONG ElemSize;
	LONG Dims;
};
//defines a datavalue structure
struct ElementModel
{
	CComVariant val;
	BOOL IsNew;
	BOOL IsDirty;
	// meaning if VT == VT_UNKNOWN or VT_DISPATCH. If TRUE we have an IStream if FALSE we have an object instance
	BOOL IsSerialized;
	//milliseconds
	INT ExpireAt;	
~ElementModel()
{
	val.ClearToZero();
}
};

struct KeyComparer
{
public:
	bool operator()(CComBSTR x, CComBSTR y) const throw()
	{
		return x.CompareTo(y, true) == -1;
	}
};

class ATL_NO_VTABLE NWCApplication :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<NWCApplication, &CLSID_NWCApplication>,
	public IDispatchImpl<IApplicationCache, &IID_IApplicationCache, &LIBID_ISPCSession>,
	public ISupportErrorInfoImpl<&IID_IDispatch>

{
public:
	DECLARE_OBJECT_DESCRIPTION("Classic ASP Application replacement by ADC Cure")
	DECLARE_REGISTRY_RESOURCEID(IDR_APPLICATION)
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(NWCApplication)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)	
		COM_INTERFACE_ENTRY(IApplicationCache)
	END_COM_MAP()

	HRESULT FinalConstruct() throw()
	{
		PCWSTR location = L"FinalConsturct";
		m_OnStartPageCalled = FALSE;
		m_bErrState = FALSE;
		m_Dirty = FALSE;

		SEEK_NULL.QuadPart = 0;
		HRESULT hr = 
			ReadDllConfig(&m_strConstruct, NULL, &m_doLogging, NULL, NULL);
		
		if (FAILED(hr))
		{
			ReportComError2(hr, location);
			m_bErrState = TRUE;
		}
		
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
	CComPtr<IRequest> m_piRequest;
	CComPtr<IResponse> m_piResponse;
	CComPtr<IServer> m_piServer;
	CRedLock  * dlm;
	CLock my_lock;
	CComBSTR m_strConstruct;
	//redis connection pool
	simple_pool::ptr_t pool;
	BYTE m_dbTimeStamp[8];
	BOOL m_bErrState;
	BOOL m_doLogging;
	BOOL m_Dirty;
	BOOL m_OnStartPageCalled ;
	GUID m_AppKey;
	std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration> m_startSessionRequest;
	std::map<CComBSTR, ElementModel, KeyComparer> _dictionary;
	std::vector<PWCHAR> _removed;
	LARGE_INTEGER SEEK_NULL;
	std::string m_lpstrMulti; // used for UTF-16 <-> UTF-8 operations contains multibytes do not use SysString* operations on it
	UINT m_dwMultiLen;

public:
	//IApplicationObject
	STDMETHOD (get_Value)(BSTR Key, VARIANT* pvar);
	STDMETHOD(put_Value)(BSTR Key, VARIANT var);
	STDMETHOD(putref_Value)(BSTR Key, VARIANT var);
	STDMETHOD(get_Key)(INT Index, BSTR* pval);
	STDMETHOD(get_Count)(PINT pval);
	STDMETHOD(_NewEnum)(IUnknown** ppval);
	STDMETHOD(RemoveKey)(BSTR Key);
	STDMETHOD(RemoveAll)();
	STDMETHOD(LockKey)(BSTR Key);
	STDMETHOD(UnlockKey)(BSTR Key);
	STDMETHOD(ExpireKeyAt)(BSTR Key, INT at);
	
	STDMETHOD(get_KeyExists)(BSTR Key, VARIANT_BOOL *pVal);
	STDMETHOD(get_KeyType)(BSTR Key, VARTYPE* pVal);


private:
	//IIS specific 
	STDMETHODIMP OnStartPage(IUnknown* pctx);
	STDMETHODIMP OnEndPage();
	//End IIS Specific
	///<summary> 
	/// finds element by specified key if result is S_FALSE it is not found
	///</summary>
	STDMETHODIMP LocalFind(BSTR Key, ElementModel* found);
	STDMETHODIMP IsDirty(BOOL* pRet);
	//std:vector<byte> probably semantically is better, instead of std::string
	STDMETHODIMP SerializeKey(BSTR key, std::string& binaryString);
	STDMETHODIMP ReadString(std::istream& stream, BSTR* outputString);
	STDMETHODIMP ReadValue(std::istream& pStream, VARIANT* TheValue, VARTYPE vtype);
	//allows recursion
	STDMETHODIMP WriteString(BSTR Key, std::string& inoutputString);
	STDMETHODIMP WriteValue(VARTYPE vt, VARIANT& val, std::string& binaryString);
	// converts a e.g. recordset into a VT_IUNKNOWN of IStream
	STDMETHODIMP ConvertObjectToStream(VARIANT& val);
	// converts an IStream to an instance of a COM class
	STDMETHODIMP ConvertVStreamToObject(VARIANT& val);
	STDMETHODIMP DeserializeKey(BSTR key, std::string binaryString);
	
	STDMETHODIMP ReadConfigFromWebConfig();
	STDMETHODIMP InitializeDataSource();
	STDMETHODIMP PersistApplication();
	
};

OBJECT_ENTRY_AUTO(CLSID_NWCApplication, NWCApplication)