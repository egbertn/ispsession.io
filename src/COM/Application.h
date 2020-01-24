#pragma once
#include "resource.h"
#include "CSession.h"
#include "message.h"
#include <chrono>

#include "IKeySerializer.h"
#include "tools.h"
//#include "CRedLock.h"

//defines a datavalue structure
struct ElementModel
{
	ElementModel(_In_ const ElementModel& src) noexcept
	{
		this->IsNew = src.IsNew;
		this->IsDirty = src.IsDirty;
		this->IsSerialized = src.IsSerialized;
		this->ExpireAt = src.ExpireAt;
		memset(&this->val, 0, sizeof(VARIANT));
		this->val = src.val;
	}
	ElementModel& operator=(_In_ const ElementModel& src) noexcept
	{
		IsNew = src.IsNew;
		IsDirty = src.IsDirty;
		IsSerialized = src.IsSerialized;
		ExpireAt = src.ExpireAt;
		return *this;
	}
	ElementModel() noexcept
	{
		memset(this, 0, sizeof(ElementModel));
	}
	CComVariant val;
	BOOL IsNew;
	BOOL IsDirty;
	// meaning if VT == VT_UNKNOWN or VT_DISPATCH. If TRUE we have an IStream if FALSE we have an object instance
	BOOL IsSerialized;
	//milliseconds
	INT ExpireAt;	
~ElementModel() noexcept
{
	val.ClearToZero();
}
};

struct KeyComparer
{
public:
	bool operator()(BSTR x, BSTR y) const 
	{
		return ::VarBstrCmp(x, y, ::GetThreadLocale(), NORM_IGNORECASE) == VARCMP_LT;
	}
};

class ATL_NO_VTABLE CApplication :
	public CComObjectRoot,
	public CComCoClass<CApplication, &CLSID_NWCApplication>,
	public IDispatchImpl<IApplicationCache, &IID_IApplicationCache, &LIBID_ISPCSession>,
	public ISupportErrorInfoImpl<&IID_IApplicationCache>,
	public IKeySerializer
{
public:
	DECLARE_REGISTRY_RESOURCEID(IDR_APPLICATION)
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CApplication)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)	
		COM_INTERFACE_ENTRY(IApplicationCache)
		COM_INTERFACE_ENTRY(IKeySerializer)
		
	END_COM_MAP()

	HRESULT FinalConstruct() throw()
	{
        NoConnectionPooling =
		m_OnStartPageCalled = FALSE;
		m_bErrState = FALSE;

		SEEK_NULL.QuadPart = 0;
		m_currentBufLen = 0;
		//dlm = new CRedLock();
		return S_OK;
	}
	void FinalRelease() throw()
	{
		/*if (dlm != NULL) {
		delete dlm;
		}*/

	}
private:
	
	//CRedLock  * dlm;
	//CLock my_lock;
	//redis connection pool
	simple_pool::ptr_t pool;
	BYTE m_dbTimeStamp[8];
	BOOL m_bErrState;
	BOOL m_doLogging;
	BOOL m_OnStartPageCalled ;
    BOOL NoConnectionPooling;
	GUID m_AppKey;
	BOOL m_licenseOK;
	std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration> m_startSessionRequest;
	std::map<CComBSTR, ElementModel, KeyComparer> _dictionary;
	std::vector<string> _removed; // do not change to char* for some unknown reason, this gets corrupted
	LARGE_INTEGER SEEK_NULL;	
	CHeapPtr<unsigned char> m_lpstrMulti; // used for UTF-16 <-> UTF-8 operations contains multibytes do not use SysString* operations on it
	INT m_currentBufLen;

public:
	//IApplicationObject
	STDMETHOD(get_Value)(BSTR Key, VARIANT* pvar) noexcept;
	STDMETHOD(put_Value)(BSTR Key, VARIANT var) noexcept;
	STDMETHOD(putref_Value)(BSTR Key, VARIANT var) noexcept;
	STDMETHOD(get_Key)(INT Index, BSTR* pval) noexcept;
	STDMETHOD(get_Count)(PINT pval) noexcept;
	STDMETHOD(_NewEnum)(IUnknown** ppval) noexcept;
	STDMETHOD(Remove)(BSTR Key) noexcept;
	STDMETHOD(RemoveAll)() noexcept;
	STDMETHOD(LockKey)(BSTR Key) noexcept;
	STDMETHOD(UnlockKey)(BSTR Key) noexcept;
	STDMETHOD(ExpireKeyAt)(BSTR Key, INT at) noexcept;
	STDMETHOD(get_Exists)(BSTR Key, VARIANT_BOOL *pVal) noexcept;
	STDMETHOD(get_KeyType)(BSTR Key, SHORT* pVal) noexcept;

	// public but not exposed to IDL
	////IKeySerializer
	STDMETHOD (get_KeyStates)(
		std::vector<string> &dirty_keys,
		std::vector<string> &new_keys,
		std::vector<string> &other_keys,
		std::vector<std::pair<string, INT>> & expireKeys,
		std::vector<string> &removed_keys) noexcept;
	STDMETHOD(SerializeKey)(BSTR key, IStream* binaryString) noexcept;
	//unpacks key & value from the blob       
	STDMETHOD (DeserializeKey)(const std::string& binaryString) noexcept;

	static STDMETHODIMP SerializeKeys(const std::vector<string> &keys, __in IKeySerializer* pDictionary, command& cmd, const string& appkeyPrefix) noexcept;


private:

	STDMETHOD( HasOnStartPageBeenCalled)();
	//IIS specific 
	STDMETHOD(OnStartPage)(IUnknown* pctx) noexcept;
	STDMETHOD(OnEndPage)() noexcept;
	//End IIS Specific
	///<summary> 
	/// finds element by specified key if result is S_FALSE it is not found
	///</summary>

	STDMETHOD(IsDirty)(BOOL* pRet) noexcept;
	//std:vector<byte> probably semantically is better, instead of std::string
	STDMETHOD(ReadString)(IStream* stream, BSTR* outputString) noexcept;
	STDMETHOD(ReadValue)(IStream* pStream, VARIANT* TheValue, VARTYPE vtype) noexcept;
	//allows recursion
	STDMETHOD(WriteString)(BSTR Key, IStream* inoutputString) noexcept;
	STDMETHOD(WriteValue)(VARTYPE vt, VARIANT& val, IStream* binaryString) noexcept;
	// converts a e.g. recordset into a VT_IUNKNOWN of IStream
	STDMETHOD(ConvertObjectToStream)(VARIANT& val) noexcept;
	// converts an IStream to an instance of a COM class
	STDMETHOD(ConvertVStreamToObject)(ElementModel& val) noexcept;

	
	STDMETHOD(InitializeDataSource)(IServer* pServer) noexcept;
	STDMETHOD(PersistApplication)() noexcept;
	STDMETHOD(EnsureBuffer)(INT newBuffer) noexcept;
	
};

OBJECT_ENTRY_AUTO(CLSID_NWCApplication, CApplication)