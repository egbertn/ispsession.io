#pragma once
#include "resource.h"       // main symbols
#include "CSession.h"
#include "CEnum.h"
#include "bstrnocache.h"

struct wire_UDT
{
	ULONG clSize;
	DWORD ifaceSize;
	//GUID refguid;
	//GUID rGuidTypeInfo;
	//WORD uVerMajor;
	//WORD uVerMinor;
	//LCID lcid;
} ;
struct ARRAY_DESCRIPTOR
{
	VARTYPE type;
	LONG ElemSize;
	LONG Dims;
};
struct TextComparer
{
public:
	bool operator()(CComBSTR x, CComBSTR y) const throw()
	{
		return x.CompareTo(y, true) == -1;		
	}
};
// like OleSaveToStream2 but uses IPersistStreamInit instead of IPersistStream
STDMETHODIMP OleSaveToStream2(IPersistStreamInit *ppersistStreamInit, IStream *pStm);
// like like OleLoadFromStream but uses the IPersistStreamInit instead of IPersistStream
STDMETHODIMP OleLoadFromStream2(IStream *pStm, REFIID iidInterface, void** ppvObj);

// CVariantDictionary
class ATL_NO_VTABLE CVariantDictionary : 
	public CComObjectRoot,
	public CComCoClass<CVariantDictionary, &CLSID_CVariantDictionary>,
	public ISupportErrorInfoImpl<&IID_INWCVariantDictionary>,
 	public IDispatchImpl<INWCVariantDictionary, &IID_INWCVariantDictionary, &LIBID_ISPCSession>,
	public IVariantDictionary2

{
public:
	DECLARE_PROTECT_FINAL_CONSTRUCT()
	DECLARE_NO_REGISTRY()
	DECLARE_NOT_AGGREGATABLE(CVariantDictionary)
	BEGIN_COM_MAP(CVariantDictionary)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)
		COM_INTERFACE_ENTRY(INWCVariantDictionary)		
		COM_INTERFACE_ENTRY(IVariantDictionary2)
		
	END_COM_MAP()

	HRESULT FinalConstruct() throw()
	{
		readOnly = 
		blnDirty = FALSE;
		m_lpstrMulti = NULL;		
		m_dwMultiLen = 0x1000;
		m_dwBufSize =0;
		
		SEEK_NULL.QuadPart = (0L);
		//allocate predified buffer
		AllocPSTR(&m_lpstrMulti, m_dwMultiLen, &m_dwBufSize);
		return S_OK;
	}

	void FinalRelease() throw()
	{
		_dictionary.clear();
		_isserialized.clear();
		//RELEASE(m_Stream)
		FreeMulti(&m_lpstrMulti);

		
	}
	// NOT STDMETHOD because we don't support aggregation (we are sealed)
	
	STDMETHODIMP get_Item(VARIANT varKey, VARIANT* pVal);
	STDMETHODIMP put_Item(VARIANT varKey, VARIANT newVal);
	STDMETHODIMP putref_Item(VARIANT varKey, VARIANT newVal);
	
	STDMETHODIMP get_Key(VARIANT KeyIndex, VARIANT* pVal);
	STDMETHODIMP get_Count(int* pVal);
	STDMETHODIMP _NewEnum(IUnknown** pVal);
	STDMETHODIMP Remove(VARIANT varKey);
	STDMETHODIMP RemoveAll(void);
	STDMETHODIMP get_Exists(VARIANT vKey, VARIANT_BOOL* pVal);
	STDMETHODIMP get_VarType(VARIANT vKey, SHORT * pVal);
	STDMETHODIMP get_CaseSensitive(VARIANT_BOOL* pVal);
	STDMETHODIMP put_CaseSensitive(VARIANT_BOOL newVal);
	STDMETHODIMP Persist(VARIANT vKey);

	//STDMETHODIMP LocalContents(DWORD* lSize, HGLOBAL *hglob);
	
	STDMETHODIMP LocalContents(DWORD* lSize, IStream **pSequentialStream);

	STDMETHODIMP LocalLoad(IStream* pStream, const DWORD lSize);
	STDMETHODIMP isDirty(BOOL *retval);
	STDMETHODIMP WriteProperty(IStream *pStream, const BSTR strPropName, const VARIANT* TheVal);
	STDMETHODIMP WriteValue(IStream *pStream, const VARIANT* TheVal, VARTYPE vtype, const BSTR theKey);
	STDMETHODIMP ReadProperty(IStream *pStream, BSTR *strPropName, VARIANT* TheVal);
	STDMETHODIMP ReadValue(IStream *pStream, VARIANT* TheValue, VARTYPE vtype);	
	STDMETHODIMP put_Readonly(const VARIANT_BOOL newVal);
	STDMETHODIMP WriteString(IStream *pStream, const BSTR TheVal);
	STDMETHODIMP ReadString(IStream *pStream, BSTR *retval);
	//STDMETHODIMP ReadUnsupportedValue(IStream *pStream, VARIANT* TheValue, DWORD size);
	//STDMETHODIMP WriteUnsupportedValue(IStream *pStream, VARIANT* TheValue);
private:
	//IVariantDictionary2
	STDMETHOD(GetTypeInfoCount2)(UINT *); //idispatch stubs
	STDMETHOD(GetTypeInfo2)(UINT, LCID, ITypeInfo **);
	
	STDMETHOD(GetIDsOfNames2)(REFIID , LPOLESTR *, UINT, LCID, DISPID *);
	STDMETHOD(Invoke2)(DISPID dispid, REFIID, LCID, WORD, DISPPARAMS *, VARIANT *, EXCEPINFO *, UINT *);
	STDMETHOD(get_Item2)(VARIANT VarKey, VARIANT *pvar);
    STDMETHOD(put_Item2)(VARIANT VarKey,VARIANT pvar);
    STDMETHOD(putref_Item2)(VARIANT VarKey, VARIANT pvar);
    STDMETHOD(get_Key2)(VARIANT VarKey, VARIANT *pvar);
	STDMETHOD(get_Count2)(int *cStrRet);
	STDMETHOD(get__NewEnum2)(IUnknown **ppEnumReturn);
    STDMETHOD(Remove2)(VARIANT VarKey);
	STDMETHOD(RemoveAll2)( void);
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

	SIZE_T m_dwBufSize; //keeps a copy of the allocated buffer for multibyte processing

	PSTR m_lpstrMulti; // used for UTF-16 <-> UTF-8 operations contains multibytes do not use SysString* operations on it
	UINT m_dwMultiLen;
	LARGE_INTEGER SEEK_NULL;	
};
OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(CLSID_CVariantDictionary, CVariantDictionary)
