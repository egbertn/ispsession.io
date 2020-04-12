#pragma once
#include "resource.h"       // main symbols
#include "CSession.h"
#include "CEnum.h"
#include "tools.h"


#include "TextComparer.h"

// CVariantDictionary
class ATL_NO_VTABLE CVariantDictionary : 
	public CComObjectRoot,
	public CComCoClass<CVariantDictionary, &CLSID_CVariantDictionary>,
	public ISupportErrorInfoImpl<&IID_INWCVariantDictionary>,
 	public IDispatchImpl<INWCVariantDictionary, &IID_INWCVariantDictionary, &LIBID_ISPCSession>
{
public:
	DECLARE_PROTECT_FINAL_CONSTRUCT()
	DECLARE_NO_REGISTRY()
	DECLARE_NOT_AGGREGATABLE(CVariantDictionary)
	BEGIN_COM_MAP(CVariantDictionary)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)
		COM_INTERFACE_ENTRY(INWCVariantDictionary)		
		
	END_COM_MAP()

	HRESULT FinalConstruct()  
	{
		readOnly = 
		blnDirty = FALSE;	
		
		SEEK_NULL.QuadPart = (0L);
		//allocate predified buffer
		
		m_lpstrMulti.AllocateBytes(0x1000/2);
		m_currentBufLen = 0x1000 / 2;
		return S_OK;
	}

	void FinalRelease()  
	{
		_dictionary.clear();
		_isserialized.clear();
		//RELEASE(m_Stream)
		m_lpstrMulti.Free();
		
	}
	// NOT STDMETHOD because we don't support aggregation (we are sealed)
	
	STDMETHODIMP get_Item(VARIANT varKey, VARIANT* pVal) noexcept;
	STDMETHODIMP put_Item(VARIANT varKey, VARIANT newVal) noexcept;
	STDMETHODIMP putref_Item(VARIANT varKey, VARIANT newVal) noexcept;
	
	STDMETHODIMP get_Key(VARIANT KeyIndex, VARIANT* pVal) noexcept;
	STDMETHODIMP get_Count(int* pVal) noexcept;
	STDMETHODIMP _NewEnum(IUnknown** pVal) noexcept;
    STDMETHODIMP Remove(VARIANT varKey) noexcept;
	STDMETHODIMP RemoveAll(void) noexcept;
	STDMETHODIMP get_Exists(VARIANT vKey, VARIANT_BOOL* pVal) noexcept;
	STDMETHODIMP get_VarType(VARIANT vKey, SHORT * pVal) noexcept;
	STDMETHODIMP Persist(VARIANT vKey) noexcept;

	STDMETHODIMP LocalContents(DWORD* lSize, IStream **pSequentialStream) noexcept;

	STDMETHODIMP LocalLoad(IStream* pStream, const DWORD lSize) noexcept;
	STDMETHODIMP isDirty(BOOL *retval) noexcept;
	STDMETHODIMP WriteProperty(IStream *pStream, const BSTR strPropName, const VARIANT* TheVal) noexcept;
	STDMETHODIMP WriteValue(IStream *pStream, const VARIANT* TheVal, VARTYPE vtype, const BSTR theKey) noexcept;
	STDMETHODIMP ReadProperty(IStream *pStream, BSTR *strPropName, VARIANT* TheVal) noexcept;
	STDMETHODIMP ReadValue(IStream *pStream, VARIANT* TheValue, VARTYPE vtype) noexcept;
	STDMETHODIMP put_Readonly(const VARIANT_BOOL newVal) noexcept;
	STDMETHODIMP WriteString(IStream *pStream, const BSTR TheVal) noexcept;
	STDMETHODIMP ReadString(IStream *pStream, BSTR *retval) noexcept;
	STDMETHODIMP EnsureBuffer(INT newSize) noexcept;
	//STDMETHODIMP ReadUnsupportedValue(IStream *pStream, VARIANT* TheValue, DWORD size);
	//STDMETHODIMP WriteUnsupportedValue(IStream *pStream, VARIANT* TheValue);
private:


	//STDMETHOD(CleanAndEraseKey)(const BSTR key);
	//CComObject<CSequentialStream> *m_Stream;	
	// dictionary dirty or not
	BOOL blnDirty, readOnly;
	// I was thinking about set<CComBSTR> for holding the keys
	// and using std::map<int, CComVariant> where int would be the hashcode
	// however, a set is not gettable by ordinal, the problem that get_Key sucks remains more or less
	// still, iterating throught 10.000 keys (and assigning them to a copy), just took 15 ms. 
	std::map<CComBSTR, CComVariant, TextComparer> _dictionary;
	std::map<CComBSTR, bool, TextComparer> _isserialized;	
	
	CHeapPtr<unsigned char> m_lpstrMulti; // used for UTF-16 <-> UTF-8 operations contains multibytes do not use SysString* operations on it
	INT m_currentBufLen = 0;
	LARGE_INTEGER SEEK_NULL;	
};
OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(CLSID_CVariantDictionary, CVariantDictionary)
