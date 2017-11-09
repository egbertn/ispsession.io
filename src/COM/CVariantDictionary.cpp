// CVariantDictionary Copyright ADC Cure 2005-2017
#include "stdafx.h"

#include "CStream.h"
#include <map>
#include "cvariantdictionary.h"

STDMETHODIMP OleSaveToStream2(IPersistStreamInit *pPersistStmInit, IStream *pStm) throw()
{
	if (pPersistStmInit == nullptr || pStm == nullptr)
		return E_INVALIDARG;
	CLSID clsd;

	HRESULT hr = pPersistStmInit->GetClassID(&clsd);
	if (hr == S_OK)
		hr = WriteClassStm(pStm, clsd);
	if (hr == S_OK)
		hr = pPersistStmInit->Save(pStm, TRUE);
	return hr;
}

STDMETHODIMP OleLoadFromStream2(IStream *pStm, REFIID iidInterface, void** ppvObj)
{
	if (pStm == nullptr || ppvObj == nullptr) return E_INVALIDARG;
	
	CLSID clsd;
	CComPtr<IPersistStreamInit> pPersist;
	HRESULT hr = ReadClassStm(pStm, &clsd);
	if (hr == S_OK)
	{
		hr = pPersist.CoCreateInstance(clsd, nullptr, CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER | CLSCTX_NO_CODE_DOWNLOAD);
		if (hr == S_OK)
		{
			hr = pPersist->Load(pStm);
			if (hr == S_OK) 
			{
				hr = pPersist->QueryInterface(iidInterface, ppvObj);
			}
				
		}
	}
	return hr;
}

//IVariantDictionary compatiblity with 
	STDMETHODIMP CVariantDictionary::GetTypeInfoCount2(UINT * pctInfo) throw()
	{
		//why would somebody with C++ do IDispatch on this?? Anyway, to be sure.
		// by convention, IDispatch is done on the default interface
		return this->GetTypeInfoCount(pctInfo);
	}
	STDMETHODIMP CVariantDictionary::GetTypeInfo2(UINT itinfo, LCID lcid, ITypeInfo ** pptinfo) throw()
	{
		return this->GetTypeInfo(itinfo, lcid, pptinfo);
	}
	STDMETHODIMP CVariantDictionary::GetIDsOfNames2 ( REFIID riid ,LPOLESTR * rgszNames,UINT cNames,LCID lcid,DISPID * dispid) throw()
	{
		return this->GetIDsOfNames(riid, rgszNames, cNames, lcid, dispid);
	}
	STDMETHODIMP CVariantDictionary::Invoke2(DISPID dispid,REFIID refid ,LCID lcid,WORD wflags ,DISPPARAMS * dispparams,VARIANT * pvarResult,EXCEPINFO * exceptInfo,UINT * puargErr) throw()
	{
		return this->Invoke(dispid, refid, lcid, wflags, dispparams, pvarResult, exceptInfo, puargErr);
	}
	STDMETHODIMP CVariantDictionary::get_Item2(VARIANT VarKey, VARIANT *pvar) throw()
	{
		return get_Item(VarKey, pvar);
	}
    STDMETHODIMP CVariantDictionary::put_Item2(VARIANT VarKey,VARIANT pvar) throw()
	{
		return put_Item(VarKey, pvar);
	}
    STDMETHODIMP CVariantDictionary::putref_Item2(VARIANT VarKey, VARIANT pvar) throw()
	{
		return putref_Item(VarKey, pvar);
	}
    STDMETHODIMP CVariantDictionary::get_Key2(VARIANT VarKey, VARIANT *pvar) throw()
	{
		return get_Key(VarKey, pvar);
	}
	STDMETHODIMP CVariantDictionary::get_Count2(int *cStrRet) throw()
	{
		return get_Count(cStrRet);
	}
	STDMETHODIMP CVariantDictionary::get__NewEnum2(IUnknown **ppEnumReturn) throw()
	{
		return _NewEnum(ppEnumReturn);
	}
    STDMETHODIMP CVariantDictionary::Remove2(VARIANT VarKey) throw()
	{
		return Remove(VarKey);
	}
	STDMETHODIMP CVariantDictionary::RemoveAll2( void) throw()
	{
		return RemoveAll();
	}


// CVariantDictionary
STDMETHODIMP CVariantDictionary::get_Count(int* pVal) throw()
{
	*pVal = (int)_dictionary.size();
	return S_OK;
}

STDMETHODIMP CVariantDictionary::get_Item(VARIANT varKey, VARIANT* pVal) throw()
{
	//LONG El=0, Position=0;
	HRESULT hr = S_OK;
	CComVariant keycopy;
	//make sure to have a BSTR because, it might be passed VT_BYREF as well (e.g. VB6 likes this)
	hr = keycopy.ChangeType(VT_BSTR, &varKey);

	auto pos = _dictionary.find(keycopy.bstrVal);
	if (pos == _dictionary.end())
	{
		pVal->vt = VT_EMPTY;
		pVal->bstrVal = nullptr;
		return S_OK;
	}

// must be IStream because we said so
	if (SUCCEEDED(hr))
	{
		VARTYPE origType = pos->second.vt;
		if (origType == VT_UNKNOWN || origType == VT_DISPATCH) 
		{
			// first test whether or not this already was unpacked by testing if it is a VT_UI1 | VT_ARRAY type
			auto pos2 = _isserialized.find(varKey.bstrVal);
			bool isSerialized = pos2 != _isserialized.end() && pos2->second;

			// already persisted? Just AddReff and return the value
			if (!isSerialized) 
			{
				logModule.Write(L"Already DeSerialized %s", varKey.bstrVal);
				pos->second.punkVal->QueryInterface(&pVal->punkVal);
				pVal->vt = origType;
			}
			else
			{
				CComQIPtr<IStream> l_pIStr(pos->second.punkVal) ;
				hr = l_pIStr->Seek(SEEK_NULL, STREAM_SEEK_SET, nullptr);
				pos->second.Clear();//remove the IStream, it is not the object itself
				hr = OleLoadFromStream(l_pIStr, IID_IUnknown, (void**)&pVal->punkVal);
				bool doPersist2 = false;
				if (hr == E_NOINTERFACE) //IPersistStream not found!
				{
					doPersist2 = true;
					//set the ptr sizeof(CLSID) back because the GetClassId advanced 16 bytes in the stream
					LARGE_INTEGER dbMove;					
					dbMove.QuadPart= -(long long)sizeof(CLSID) ;
					hr = l_pIStr->Seek(dbMove, STREAM_SEEK_CUR, nullptr);
					hr = OleLoadFromStream2(l_pIStr, IID_IUnknown, (void**)&pVal->punkVal);
				}
				logModule.Write(L"OleLoadFromStream%s %x", doPersist2 ? L"2": L"", hr);
				pos2->second = false; //don't deserialize next time
				if (hr == S_OK)
				{					
					// put the unpersisted object reference back in the variant.
					pVal->punkVal->QueryInterface(&pos->second.punkVal);
					pos->second.vt = pVal->vt = origType;
				}
				else
					pVal->vt = VT_EMPTY;
			}
		}
		else
		{
			hr = ::VariantCopy(pVal, &pos->second);
			if (hr != S_OK) logModule.Write(L"VariantCopy %x", hr);
		}
	}
	if (FAILED(hr))
	{
		Error(L"get_Item failed", GetObjectCLSID(), hr);
	}
	//if (FAILED(hr)) ReportComError2(hr, L"get_Item");
	return hr;
}

STDMETHODIMP CVariantDictionary::put_Item(VARIANT varKey, VARIANT newVal) throw()
{
    //LONG El = 0, Position = 0;
	HRESULT hr = S_OK;
	if (this->readOnly == TRUE)
	{	
		hr =  E_FAIL;
		Error(ERROR_READONLY, GetObjectCLSID(), hr);
		return hr;
	}

	VARIANT vDeref;
	//VariantCopyInd smokes on this, cannot handle it.
	if ((newVal.vt & (VT_BYREF | VT_ARRAY)) == (VT_BYREF | VT_ARRAY))
	{
		vDeref.vt = newVal.vt - VT_BYREF;
		vDeref.parray = *newVal.pparray;
	}
	else if ((newVal.vt & (VT_VARIANT | VT_BYREF)) == (VT_VARIANT | VT_BYREF))
		vDeref = *newVal.pvarVal;
	else
		vDeref = newVal;

	CComVariant keycopy;	
	hr = keycopy.ChangeType(VT_BSTR, &varKey);
	if (FAILED(hr))
	{
		return hr;
	}
	auto pos = _dictionary.find(keycopy.bstrVal);
	if (pos == _dictionary.end())
	{		
		VARIANT v = { 0 };
		//add it with an empty value and find it again
		logModule.Write(L"add key %s", keycopy.bstrVal);
		_dictionary.insert(pair<CComBSTR, CComVariant>(keycopy.bstrVal, v));
		pos = _dictionary.find(keycopy.bstrVal);
	}
	if (vDeref.vt == VT_DISPATCH)
	{
		if (vDeref.pdispVal == nullptr) 
			hr = E_POINTER;
		else
		{
			CComVariant varResolved;
			DISPPARAMS  dispParamsNoArgs = {0};
			if (vDeref.pdispVal->Invoke(DISPID_VALUE, IID_NULL, LOCALE_SYSTEM_DEFAULT,
					DISPATCH_PROPERTYGET | DISPATCH_METHOD,
					&dispParamsNoArgs, &varResolved, nullptr, nullptr) == S_OK) 
			{
				//recursion...
				hr = put_Item(varKey, varResolved);
			}
		}
	}	
	/*else if (vDeref.vt == VT_RECORD || vDeref.vt == VT_USERDEFINED)
	{
		hr = E_INVALIDARG;
		ATL::AtlSetErrorInfo(CLSID_CVariantDictionary, L"user defined types are not supported, convert to byte array before instead", 0, nullptr, IID_INWCVariantDictionary, hr, _AtlBaseModule.GetResourceInstance());
			
	}*/
	else
	{
		pos->second = vDeref;
	}
	
    blnDirty = TRUE;
	return hr;
}
// dotned always prefers putref instead of 
// put_Item
STDMETHODIMP CVariantDictionary::putref_Item(VARIANT varKey, VARIANT newVal) throw()
{
	HRESULT hr = S_OK;
	VARIANT* valueArray = nullptr;
	CComVariant keycopy;
	hr = keycopy.ChangeType(VT_BSTR, &varKey);

	logModule.Write(L"putref_Item %s", keycopy.bstrVal);
	if (this->readOnly == TRUE)
	{	
		hr =  E_FAIL;		
		Error(ERROR_READONLY,GetObjectCLSID(), hr); 
		return hr;
	}

	VARTYPE origType = newVal.vt;
	if ((origType & VT_DISPATCH ) == VT_DISPATCH || (origType & VT_UNKNOWN) == VT_UNKNOWN)
	{
		auto pos = _dictionary.find(keycopy.bstrVal);
		if (pos == _dictionary.end())
		{
			logModule.Write(L"add key %s", keycopy.bstrVal);
			VARIANT v = { VT_EMPTY };
			_dictionary.insert(pair<CComBSTR, CComVariant>( keycopy.bstrVal, v));
			pos = _dictionary.find(keycopy.bstrVal);
		}
		valueArray = &pos->second;		

		VARIANT vDeref = {0};
	
		if ((origType & (VT_VARIANT | VT_BYREF)) == (VT_BYREF | VT_VARIANT))
		{	
			vDeref =  *newVal.pvarVal;
			logModule.Write(L"putref_Item dereffed object variant");
			vDeref.vt = origType & ~VT_BYREF;
		}
		else
			vDeref = newVal;
		
		CComQIPtr<IPersistStream> l_ptestForCapability(vDeref.pdispVal);
		CComQIPtr<IPersistStreamInit> l_ptestForCapability2(vDeref.pdispVal);
		if (l_ptestForCapability == nullptr && l_ptestForCapability2 == nullptr)
		{
			hr = E_NOINTERFACE;
		}
		else	
		{
			pos->second = vDeref;
			//vDeref.pdispVal->QueryInterface(&valueArray->pdispVal);
			//if ((origType & VT_BYREF) == VT_BYREF) 
			//	origType &= ~VT_BYREF;
			//valueArray->vt = origType;
		}

		if (FAILED(hr))
		{
			Error(L"Object does not support IPersistStream", GetObjectCLSID(), hr);
			logModule.Write(L"Object does not support IPersistStream");
		}
	}
	else
	{
		hr = E_POINTER;
		Error(L"This variable is not an object", GetObjectCLSID(), hr);
		logModule.Write(L"putref_Item not VT_DISPATCH or VT_UNKNOWN but %d", origType);
	}
	return hr;
}

STDMETHODIMP CVariantDictionary::get_Key(VARIANT VarKey, VARIANT* pVal) throw()
{
	HRESULT hr = S_OK;
	CComVariant KeyCopy;
	hr = KeyCopy.ChangeType(VT_I4, &VarKey);
	
	if (SUCCEEDED(hr))
	{
		auto KeyIndex = KeyCopy.uintVal;

		if ((KeyIndex < 1) || (KeyIndex > _dictionary.size() + 1))		
			hr = E_INVALIDARG;
		int ct = 0; //terrible loop map has no index, so find it
		for (auto idx = _dictionary.begin(); idx != _dictionary.end(); ++idx)
		{
			if (++ct == KeyIndex)
			{
				return CComVariant(idx->first).Detach(pVal);				
			}
		}
	}
	return hr;
}

STDMETHODIMP CVariantDictionary::_NewEnum(IUnknown** pVal) throw()
{
	HRESULT hr = S_OK;
	ULONG size = _dictionary.size();
	SAFEARRAY * psaKeyEnum = ::SafeArrayCreateVector(VT_BSTR, 0, size);
	if (psaKeyEnum == nullptr)
			return E_OUTOFMEMORY;
	

	// create a copy of the keys
	// VB does not support IEnumString :<
	BSTR * mp; //= reinterpret_cast<BSTR*>(psaKeyEnum->pvData);
	::SafeArrayAccessData(psaKeyEnum, (void**)&mp);
	int cx = 0;
	for (auto k = _dictionary.begin(); k != _dictionary.end(); ++k)
	{
		mp[cx++] = k->first.Copy();
	}
	CComObject<CCEnum> *m_enum;

	hr = CComObject<CCEnum>::CreateInstance(&m_enum);

	m_enum->Init(psaKeyEnum);
	hr = m_enum->QueryInterface(IID_IUnknown, (void**)pVal);
	return hr;
}

STDMETHODIMP CVariantDictionary::Remove(VARIANT varKey) throw()
{
	HRESULT hr = S_OK;
	CComVariant varcopy;
	hr = varcopy.ChangeType(VT_BSTR, &varKey);
	if (FAILED(hr))
	{
		return hr;
	}	
	
	if (SUCCEEDED(hr)) 
	{
		bool isObject = false;
		auto pos = _dictionary.find(varcopy.bstrVal);
		if (pos != _dictionary.end())
		{
			isObject = pos->second.vt == VT_UNKNOWN || pos->second.vt == VT_DISPATCH;
			logModule.Write(L"remove key %s", pos->first);
			_dictionary.erase(pos);
			//this->CleanAndEraseKey(varcopy.bstrVal);
		}
		if (isObject)
		{
			auto pos = _isserialized.find(varcopy.bstrVal);
			if (pos != _isserialized.end())
			{
				((CComBSTR2&)pos->first).Empty();
				_isserialized.erase(pos);
			}
		}
		blnDirty = TRUE;			
	}
	return hr;
}

STDMETHODIMP CVariantDictionary::RemoveAll(void) throw()
{	
	logModule.Write(L"RemoveAll");
	/*for (auto pos = _isserialized.begin(); pos != _isserialized.end(); ++pos)
	{
		((CComBSTR2&)pos->first).Empty();		
	}*/
	_isserialized.clear();
	/*for (auto pos = _dictionary.begin(); pos != _dictionary.end(); ++pos)
	{
		((CComBSTR2&)pos->first).Empty();
		pos->second.Clear();
	}*/
	_dictionary.clear(); //destructors ~CComBSTR etc are called indeed
	return S_OK;
}


STDMETHODIMP CVariantDictionary::LocalLoad(IStream* pStream, const DWORD lSize) throw()
{
	HRESULT hr = S_OK;
	INT cx = 0;
	CComVariant varTemp;
	if (lSize == 0) return E_INVALIDARG;
	if (pStream == nullptr) return hr;
	INT m_lVDictElements = 0;
	logModule.Write(L"Loading from stream");	
	
	CComBSTR stemp;
		
	if (FAILED(hr))	
		logModule.Write(L"Could not QI IID_IStream reference %x", hr );
	else
	{
		ReadProperty(pStream, &stemp, &varTemp);
		if (stemp != L"Els")
		{
			logModule.Write(L"The streams seems to be corrupt");
			return E_FAIL;
		}

		m_lVDictElements= varTemp.intVal;
		logModule.Write(L"LocalLoad %d elements", m_lVDictElements);		
		if ((m_lVDictElements < 0) || (m_lVDictElements > MAXSHORT)) 
		{
			logModule.Write(L"Too many elements");
			return E_FAIL;
		}

		this->RemoveAll();
		
		if (hr == S_OK && m_lVDictElements > 0) 
		{
			for (cx = m_lVDictElements - 1; cx >= 0 && hr == S_OK; cx--)
			{
				BSTR key = nullptr;
				VARIANT val = { 0 };
				hr = ReadProperty(pStream, &key, &val);
				CComBSTR k;
				CComVariant v;
				k.Attach(key);
				v.Attach(&val);
				_dictionary.insert(std::pair<CComBSTR, CComVariant>(k, v));
			}		
		}
	}

//CHECK
	//RELEASE(pStream);

	return hr;
}

STDMETHODIMP CVariantDictionary::isDirty(BOOL *retval) throw()
{
	*retval = blnDirty;
	return S_OK;
}

STDMETHODIMP CVariantDictionary::WriteProperty(IStream *pStream, const BSTR strPropName, const VARIANT* TheVal) throw()
{
   
	// write the complete string including the leading 4 bytes
	HRESULT hr = WriteString(pStream, strPropName);
	//variant type
	VARTYPE vtype = TheVal->vt;
	if (SUCCEEDED(hr))
	{
		DWORD bogus = 0;
		ULARGE_INTEGER keepPointer = { 0 };		
		pStream->Seek(SEEK_NULL, STREAM_SEEK_CUR, &keepPointer);

		//reserve space
		hr = pStream->Write(&bogus, sizeof(DWORD), nullptr);
		hr = pStream->Write(&vtype, sizeof(VARTYPE), nullptr);
		hr = WriteValue(pStream, TheVal, vtype, strPropName);
		
		//get current position again but after having written the data (WriteValue)
		LARGE_INTEGER curPos = { 0 };
		hr = pStream->Seek(SEEK_NULL, STREAM_SEEK_CUR, (ULARGE_INTEGER*)&curPos);

		//go back to the nexteoffset location and write the 'nextoffset' pointer
		LARGE_INTEGER kp;
		kp.LowPart = keepPointer.LowPart;
		kp.HighPart = 0;		
		pStream->Seek(kp, STREAM_SEEK_SET, nullptr);
		pStream->Write(&curPos.LowPart, sizeof(DWORD), nullptr);

		//restore where we were		
		pStream->Seek(curPos, STREAM_SEEK_SET, nullptr);
	}
	logModule.Write(L"WriteProperty propname=%s, type=%d, result=%x", strPropName, vtype, hr);	
/*    ' 1- Len4 PropName var
    ' 2- Offset 4
    ' 4- Variant
    
    'OR----
    
    ' 1- Len4 PropName var
    ' 2- Offset 4
    ' 3- vType 2
    ' 4- Variant - String / Variant Object / Array
    
    'IF -- Variant Array
    '
*/
	return hr;
}
// Writes a Variant, including complex types to the memory stream
STDMETHODIMP CVariantDictionary::WriteValue(IStream *pStream,
	const VARIANT* TheVal,  VARTYPE vtype, const BSTR strKeyName) throw()
{

	LONG cBytes = 0,
		cx = 0 , lMemSize = 0, lElements = 0,  
		cDims = 0,
		ElSize = 0;

	HRESULT hr = S_OK;
	PVOID psadata = nullptr;
	SAFEARRAY *psa = nullptr;

	if ((vtype & VT_ARRAY) == VT_ARRAY) 
	{
        /*'write the bounds
        'psa = TheVal->parray
        'slightly tricky in VB but it works anyway, without dangers since the garbage collector
        ' won't recognize that psa contains an array. It just thinks it to be a 32bit number*/
        if ((vtype & VT_BYREF) == VT_BYREF) 
		{
            psa= *TheVal->pparray;
            vtype ^= VT_BYREF; //mask out
		}
		else
			psa = TheVal->parray;
	
        // WriteValue --> must <-- support recursive lookups. If we donnot include the
        // type readvalue won't jump to the array section
		ElSize = ::SafeArrayGetElemsize(psa);
		cDims = ::SafeArrayGetDim(psa);
		CComHeapPtr<SAFEARRAYBOUND> psaBound;
		psaBound.Allocate(cDims);
		
		ARRAY_DESCRIPTOR descriptor;
		descriptor.type = vtype;
		descriptor.ElemSize = ElSize;
		descriptor.Dims = cDims;
		hr = pStream->Write(&descriptor, sizeof(ARRAY_DESCRIPTOR), nullptr);
      	
		VARTYPE vcopy = vtype & ~VT_ARRAY;
		if (vcopy == VARENUM::VT_UNKNOWN || vcopy == VARENUM::VT_ERROR || vcopy == VARENUM::VT_VARIANT)
		{
			//because e.g. Redim v(1,1) TypeName(v) == "Variant()"
			CComBSTR bogusAssemblyTypeForDotNet(L"System.Object"); //
			WriteString(pStream, bogusAssemblyTypeForDotNet);
		}
		else //todo jagged array??
		{
			WriteString(pStream, nullptr);
		}
		if (psa == nullptr)//even if it is a null array, we need to write System.Object
		{
			goto exit;
		}
        //an array should have at least one element Otherwise it's type would be VT_NULL or VT_EMPTY
        //and never get at this execution point
        lMemSize = 1;
		//if (cDims > 1) DebugBreak();
		logModule.Write(L"writing array type=%d, cDims %d, ElSize %d", vtype & ~VT_ARRAY, cDims, ElSize);

		for (cx = 1;  cx <= cDims; cx++)
		{	
			LONG bounds[2];
			hr = ::SafeArrayGetLBound(psa, cx, &bounds[1]);
			hr = ::SafeArrayGetUBound(psa, cx, &bounds[0]); 
			bounds[0] = bounds[0] - bounds[1] + 1;
            hr = pStream->Write(bounds, sizeof(SAFEARRAYBOUND), nullptr);
			psaBound[cx - 1].lLbound = bounds[1];
			psaBound[cx - 1].cElements = bounds[0];
			lMemSize *= bounds[0];	
		}

        lElements = lMemSize;
        lMemSize *= ElSize;
		logModule.Write(L"writing array els(%d) elsize(%d)", lElements, ElSize);		
		
		
        if ((vcopy == VT_I1) ||
			(vcopy == VT_UI1) || (vcopy == VT_I2) || (vcopy == VT_I4) || (vcopy == VT_R4) ||
			(vcopy == VT_R8) || (vcopy == VT_CY) ||  (vcopy == VT_DATE) || (vcopy == VT_BOOL) || (vcopy == VT_I8) 
			)
            //wow, write the whole block at once. Simple, not?
		{
			if (lMemSize > 0 )
			{
				hr = ::SafeArrayAccessData(psa, &psadata);
				if (hr == S_OK) 
				{
					hr = pStream->Write (psadata, lMemSize, nullptr);
					::SafeArrayUnaccessData(psa);
				}
			}
		}
		else if ((vcopy == VT_BSTR)  || (vcopy == VT_DECIMAL))
		{
			if (lElements > 0) 
			{
				hr = ::SafeArrayAccessData(psa, &psadata);
				if (hr == S_OK)
				{
					//the BSTR allocation area is contigious.
					logModule.Write(L"Writing vt = %d array length=%d", vcopy, lElements);
					int backup = logModule.get_Logging(); // disable for the moment
					logModule.set_Logging(0);
					int els = 0;
					if (vcopy == VT_BSTR)
					{
						auto myarray = static_cast<BSTR*>(psadata);						
						for (; els < lElements && hr == S_OK; els++)
							hr = WriteString(pStream, myarray[els]);
					}
					else if (vcopy == VT_DECIMAL)
					{
						auto myarray = static_cast<VARIANT*>(psadata); // ugly, but true
						for (; els < lElements && hr == S_OK; els++)
							hr = WriteValue(pStream, &myarray[els], VT_DECIMAL, strKeyName);
					}
					logModule.set_Logging(backup);
					logModule.Write(L"written VT_BSTR array length=%d %x", els, hr);
					::SafeArrayUnaccessData(psa);
				}
			}
		}
		//write a variant array of type VT_VARIANT		
		else if (vcopy == VARENUM::VT_VARIANT)
		{
			// the VARIANT allocation area is contigious, but on Windows X64, 
			// each element is 24 in size, instead of 16! So, the carry over indice 
			// in combination with SafeArrayPtrOfIndex built had to be made
			if  (lElements > 0 && psa != nullptr) 
			{
				
				CComHeapPtr<LONG> rgIndices;
				rgIndices.Allocate(cDims);
				::SafeArrayLock(psa);
				for (LONG x = 0; x < cDims; x++)
				{
					rgIndices[x] = psaBound[x].lLbound;
				}
				
				LONG dimPointer = 0, // next dimension will first be incremented
				  findEl = 0;
				int backup = logModule.get_Logging(); // disable for the moment
				logModule.set_Logging(0);
				for(;;)
				{
					if (rgIndices[dimPointer] < 
						(LONG)psaBound[dimPointer].cElements + psaBound[dimPointer].lLbound)
					{
						VARIANT* pVar = nullptr; 
						//logModule.Write(L"Indices %d,%d,%d", rgIndices[0], rgIndices[1], rgIndices[2]);
						hr = SafeArrayPtrOfIndex(psa, rgIndices, (void**)&pVar);
						if (FAILED(hr))
						{
							logModule.Write(L"FATAL: SafeArrayPtrOfIndex failed %x", hr);	
							SafeArrayUnlock(psa);
							return hr;
						}						
						vtype = pVar->vt;
						hr = pStream->Write (&vtype, sizeof(VARTYPE), nullptr);		
						//----- recursive call ----- keep an eye on this
						hr = WriteValue(pStream, pVar, vtype, strKeyName);

						rgIndices[dimPointer]++; 
						//end of loop
						if (++findEl == lElements)
						{
							break;
						}
					}
					//carry stuff
					else
					{
						//magic
						while (++dimPointer <= cDims)
						{
							if (rgIndices[dimPointer] < ((LONG)psaBound[dimPointer].cElements + psaBound[dimPointer].lLbound - 1))
							{
								rgIndices[dimPointer]++;
								break;
							}
						}
						
						//reset previous cols to initial lowerbound from left to 
						// most right carry position
						for (LONG z = 0; z < dimPointer; z++)
							rgIndices[z] = psaBound[z].lLbound;
						dimPointer= 0;			
					}
					::SafeArrayUnlock(psa);
				}				
				logModule.set_Logging(backup);
				logModule.Write(L"written VT_VARIANT array length=%d %x", findEl, hr);
			} // if not zero elements
		}        
		else
		{
            if (psa == nullptr)
                //handle nullptr pointer array
                hr = pStream->Write(&psa, sizeof(nullptr), nullptr);
            else
                hr = E_INVALIDARG;         
		}
		
	}
	else
	{
       // empty and null {0,1} need not be written
		//mask out using xor
		logModule.Write(L"Writing simple variant type=%d", vtype);
		switch (vtype & (VT_ARRAY - 1))
		{
			case VT_NULL: case VT_EMPTY:
				cBytes = 0;
				break;
			case VT_I1: case VT_UI1:
				cBytes = sizeof(BYTE);
				break;
			case VT_I2:	case VT_UI2: case VT_BOOL:
				cBytes = sizeof(VARIANT_BOOL);
				break;
			case VT_I4:	case VT_UI4: case VT_R4: case VT_INT: case VT_UINT:	case VT_ERROR:
				cBytes = sizeof(SCODE);
				break;
			case VT_I8:	case VT_UI8: case VT_CY: case VT_R8: case VT_DATE:
				cBytes = sizeof(LONGLONG);
				break;
			//case VT_RECORD: //mainly only with .NET structs
			//	{					
			//		// create a shorter cut
			//		IRecordInfo * pRecordInfo = TheVal->pRecInfo;

			//		//prepare our own structure for persistence
			//		wire_UDT nfo = {0};
			//		
			//		if (pRecordInfo == nullptr)
			//			hr = E_POINTER;
			//		// get the recordsize
			//		if (hr == S_OK) 
			//			hr = TheVal->pRecInfo->GetSize(&nfo.clSize);

			//		if (hr == S_OK)	
			//			hr = CoGetMarshalSizeMax(&nfo.ifaceSize, 
			//				IID_IRecordInfo,
			//				TheVal->pRecInfo,
			//				MSHCTX_NOSHAREDMEM,
			//				nullptr,
			//				MSHLFLAGS_NORMAL);

			//			logModule.Write(L"GetMarshalSizeMax %x %d", hr, nfo.ifaceSize);

			//		//put our stuff on the line
			//		if (hr != S_OK) 
			//		{
			//			nfo.clSize =
			//			nfo.ifaceSize = 0;
			//		}
			//		pStream->Write(&nfo, sizeof(wire_UDT), nullptr);
			//		
			//		if (hr == S_OK)
			//		{
			//			HGLOBAL hglob = GlobalAlloc(GMEM_MOVEABLE, nfo.ifaceSize);
			//			CComPtr<IStream> pstream;
			//			if (hglob != nullptr)
			//				hr = CreateStreamOnHGlobal(hglob, TRUE, &pstream);
			//			else hr = E_OUTOFMEMORY;
			//			if (hr == S_OK)
			//				hr = CoMarshalInterface(pstream, 
			//					IID_IRecordInfo, 
			//					pRecordInfo, 
			//					MSHCTX_NOSHAREDMEM, 
			//					nullptr,
			//					MSHLFLAGS_NORMAL);						
			//			//copy to main stream
			//			if (hr == S_OK) 
			//			{
			//				pStream->Write(GlobalLock(hglob), nfo.ifaceSize, nullptr);
			//				GlobalUnlock(hglob);
			//			}
			//		}

			//		logModule.Write(L"CoMarshalInterface %x", hr);
			//		// now copy the raw record data
			//		if (hr == S_OK)
			//			hr = pStream->Write(TheVal->pvRecord, nfo.clSize, nullptr);
			//		
			//	}	
			//	cBytes=0;
			//	break;
			case VT_DECIMAL:
				//correct because decimal eats the whole variant !
				cBytes = sizeof(DECIMAL);
				hr = pStream->Write(TheVal, cBytes, nullptr);
				break;
			case VT_BSTR:

				hr = WriteString(pStream, TheVal->bstrVal);
				cBytes = SysStringByteLen(TheVal->bstrVal);
				break;
			case VT_DISPATCH: //fall through VT_UNKNOWN
			case VT_UNKNOWN:
			{		

				CComPtr<IStream> l_pIStr ;
				// it is an unpersisted object, not yet packed as IStream
				// we'll do that first.
				auto pos = _isserialized.find(strKeyName);
				bool isSerialized = pos != _isserialized.end() && pos->second;
				if (!isSerialized)
				{
					CComQIPtr<IPersistStream> pPersist(TheVal->punkVal);
					hr = CreateStreamOnHGlobal(nullptr, TRUE, &l_pIStr);
					bool noPersist2 = false;
					if (hr == S_OK)
					{
						if (pPersist == nullptr)
						{
							noPersist2 = true;
							CComQIPtr<IPersistStreamInit> pPersist2(TheVal->punkVal);
							if (pPersist2 == nullptr)
							{
								hr = E_NOINTERFACE;
							}
							hr = OleSaveToStream2(pPersist2, l_pIStr);
						}
						else
						{
							hr = ::OleSaveToStream(pPersist, l_pIStr);										
						}
						logModule.Write(L"OleSaveToStream%s %x", noPersist2 ? L"2" : L"", hr);
					}					
					
				} //isSerialized
				else
				{
					//it is already serialized to IStream, just get the pointer
					hr = TheVal->punkVal->QueryInterface(&l_pIStr);
				}
				if (hr == S_OK)
				{
					//get size of stream and write the size

					hr = l_pIStr->Commit(STGC_DEFAULT);
					STATSTG pstatstg = { 0 };
					hr = l_pIStr->Stat(&pstatstg, STATFLAG_NONAME);
					cBytes = pstatstg.cbSize.LowPart;
					logModule.Write(L"Streamsize %d", cBytes);
					hr = pStream->Write(&cBytes, sizeof(pstatstg.cbSize.LowPart), nullptr);
					// copy the persisted object as bytestream to the main stream		
					l_pIStr->Seek(SEEK_NULL, STREAM_SEEK_SET, nullptr);
					hr = l_pIStr->CopyTo(pStream, pstatstg.cbSize, nullptr, nullptr);
					if (FAILED(hr))
					{
						logModule.Write(L"FATAL: cannot copy stream %x", hr);
					}
				}
				else
				{
					// indicate zero bytes
					cBytes = 0;
					pStream->Write (&cBytes, sizeof(DWORD), nullptr);
				}
				
				break;
			}
			default:
				hr = E_INVALIDARG;
		}
exit: // sorry
		if (cBytes > 0 && vtype != VT_BSTR && vtype != VT_UNKNOWN && vtype != VT_DISPATCH && vtype != VT_DECIMAL)
			pStream->Write(&TheVal->bVal, cBytes, nullptr);
		logModule.Write(L"Simple variant size=%d", cBytes);
	}

	return hr;
}

STDMETHODIMP CVariantDictionary::ReadProperty(IStream* pStream, _Inout_ BSTR *strPropName, VARIANT* TheVal) throw()
{

/*   'for each named property SEARCH on it
    'the keepPointer always should point to a valid pointer of the next element
    'if length (strPropName) == 0 then we just get the next pointer and fill up the propertyname*/
	HRESULT hr = ReadString(pStream, strPropName);    
    
	if (hr == S_OK)
	{
			//assign sufficient space divide by 2 since the size is written using bytes instead of chars (unicode)
		// read next offset this offset pointer is written en read but not
		// used currently
		DWORD nextOffset = 0;
		hr = pStream->Read(&nextOffset, sizeof(nextOffset), nullptr);

		VARTYPE vtype;
		hr = pStream->Read (&vtype, sizeof(VARTYPE), nullptr);
		//if (vtype == (VT_ARRAY | VT_UNKNOWN) || vtype == (VT_ARRAY | VT_ERROR))
		//{			
		//	ULARGE_INTEGER curPos = { 0 };
		//	pStream->Seek(SEEK_NULL, STREAM_SEEK_CUR, &curPos);
		//	DWORD noBytes = (nextOffset - curPos.LowPart) ;
		//	//hr = ReadUnsupportedValue(pStream, TheVal, noBytes);
		//	TheVal->vt = vtype; //will crash VBScript			
		//}
		//else
		//{
			hr = ReadValue(pStream, TheVal, vtype);		
		//}
		if (vtype == VT_DISPATCH || vtype == VT_UNKNOWN && hr == S_OK)
		{
			BSTR prop = *strPropName;
			_isserialized.insert(pair<CComBSTR, bool>(prop, true)); 
		}
		logModule.Write(L"ReadProperty propname=%s, type=%d, result=%x", *strPropName, vtype, hr);
	}
	return hr;
}
///<summary>
///reads a VARIANT value from the stream (pStream). Arrays are recusively processed
///</summary>
///<param name="pStream">asdf</param>
///<param name="TheValue">asdf</param>
///<param name="vtype">VT variant type</param>
///<returns>a HRESULT</returns>
STDMETHODIMP CVariantDictionary::ReadValue(IStream * pStream, VARIANT* TheValue, VARTYPE vtype) throw()
{
    LONG cBytes =0,
		ElSize =0,
		cDims =0,
		lElements =0,
		lMemSize =0;
    VARTYPE lType =VT_EMPTY;
	// we reassign the value
	::VariantClear(TheValue);
	ZeroMemory(TheValue, sizeof(VARIANT));
	
	PVOID psadata= nullptr;
	HRESULT hr = S_OK;
	INT els = 0;
	if ((vtype & VT_ARRAY) == VT_ARRAY)
	{
		ARRAY_DESCRIPTOR descriptor;
		pStream->Read(&descriptor, sizeof(ARRAY_DESCRIPTOR), nullptr);
		vtype = descriptor.type;
		ElSize = descriptor.ElemSize;
		cDims = descriptor.Dims;
		lType = vtype; // 'cache the old vtype we need it to set the type of vartype
		vtype ^= VT_ARRAY; // mask out
		auto isJagged = false;
		
		//WARNING on Windows x64 and x86, the variant size array differs		
		auto doGetType = (vtype == VARENUM::VT_UNKNOWN || vtype == VARENUM::VT_ERROR || vtype == VARENUM::VT_VARIANT); //VT_VARIANT because we have object arrays with a specific type?
		if (doGetType)
		{
			CComBSTR BogusVariantType;
			ReadString(pStream, &BogusVariantType);
			// now we have a problem
			if (!BogusVariantType.StartsWith(L"System.Object"))
			{
				logModule.Write(L"FATAL: Cannot support .NET typed arrays %s", BogusVariantType);
				return E_INVALIDARG;
			}
		}
		else
		{
			CComBSTR BogusVariantType;
			ReadString(pStream, &BogusVariantType);
			isJagged = BogusVariantType == L"__jagged";
			if (isJagged)
			{
				BogusVariantType.Empty();
				ReadString(pStream, &BogusVariantType); // e.g. System.Int32[][] this has no use for OLEAuto
			}
		}
		if (ElSize == 0 || cDims == 0)
		{
			logModule.Write(L"Array is empty");
			goto exit;
		}


		//reconstruct the number of elements and size
        lMemSize = 1;
		//CTempBuffer<SAFEARRAYBOUND, 128, CComAllocator> safebound(cDims);
		CComHeapPtr<SAFEARRAYBOUND> safebound;
		safebound.Allocate(cDims);
		logModule.Write(L"reading array type=%d cDims %d", vtype, cDims);
		//int bx=cDims;
		for (LONG cx = 0; cx < cDims; cx++)// cDims - 1; cx != 0; cx--)
		{
			if ( pStream->Read (&safebound[cx], sizeof(SAFEARRAYBOUND), nullptr)== S_OK)
			{
				lMemSize *= safebound[cx].cElements;
			}
			logModule.Write(L"lBound %d, cElements %d", safebound[cx].lLbound, safebound[cx].cElements);
		}
		auto psa = ::SafeArrayCreate(vtype, cDims, safebound);		
		
		if (psa == nullptr) 
		{
			goto exit;			
		}
        lElements = lMemSize;	
        lMemSize *= ElSize;
		logModule.Write(L"array memsize %d els %d", lMemSize, lElements);       
		if (
			(vtype == VT_UI1) || (vtype == VT_I2) || (vtype == VT_I4) || (vtype == VT_R4) || (vtype== VT_R8)
			|| (vtype == VT_CY) || (vtype == VT_DATE) 
			|| (vtype == VT_BOOL) || (vtype == VT_I8) || (vtype == VT_I1)
			)
		{
			//retrieve pointer to first address
			hr = ::SafeArrayAccessData(psa, &psadata);
			if (hr == S_OK) 
			{
				pStream->Read(psadata, lMemSize, nullptr);
				::SafeArrayUnaccessData(psa);
			}
		}
		else if ((vtype== VT_BSTR || vtype == VT_DECIMAL) && lElements > 0)
		{
			hr = ::SafeArrayAccessData(psa, &psadata);
			if (hr == S_OK)
			{
				logModule.Write(L"VT_BSTR array %d", lElements);
				int backup = logModule.get_Logging(); // disable for the moment
				logModule.set_Logging(0);
				if (vtype == VT_BSTR)
				{
					auto btemp = static_cast<BSTR*>(psadata);
					for (els = 0; els < lElements && hr == S_OK; els++)
						hr = ReadString(pStream, &btemp[els]);
				}
				else if (vtype == VT_DECIMAL)
				{
					auto btemp = static_cast<VARIANT*>(psadata); //ugly but true
					for (els = 0; els < lElements && hr == S_OK; els++)
						hr = ReadValue(pStream, &btemp[els], VT_DECIMAL);
				}

				::SafeArrayUnaccessData(psa);
				logModule.set_Logging(backup);
			}
			
		}					
		else if (vtype == VARENUM::VT_VARIANT && lElements > 0)
		{		
			CTempBuffer<SAFEARRAYBOUND> psaBound(cDims);
			//LONG rgIndices[4] = {0};
			CTempBuffer<LONG> rgIndices(cDims);

			LONG dimPointer = 0; // next dimension will first be incremented
			LONG findEl = 0, ubound, lbound;
			::SafeArrayLock(psa);
			for (LONG x = 0; x < cDims; x++)
			{
				::SafeArrayGetLBound(psa, x + 1, &lbound);
				::SafeArrayGetUBound(psa, x + 1, &ubound);
				psaBound[x].cElements = ubound - lbound + 1;
				psaBound[x].lLbound = rgIndices[x] = lbound;
			}
			int backup = logModule.get_Logging(); // disable for the moment
			logModule.set_Logging(0);
			for(;;)
			{
				if (rgIndices[dimPointer] < 
					(LONG)psaBound[dimPointer].cElements + psaBound[dimPointer].lLbound) 
				{
					VARIANT* pVar ;
					hr = ::SafeArrayPtrOfIndex(psa, rgIndices, (void**)&pVar);
					//logModule.Write(L"Indices %d,%d,%d", rgIndices[0], rgIndices[1], rgIndices[2]);
					if (FAILED(hr))
						logModule.Write(L"FATAL: SafeArrayPtrOfIndex failed %x", hr);

					if (hr == S_OK)
					{
						vtype = pVar->vt;							
						hr = pStream->Read(&vtype, sizeof(VARTYPE), nullptr);			
						if (hr == S_OK) 
							//----- recursive call ----- keep an eye on this
							hr = ReadValue(pStream, pVar, vtype);
					}
					rgIndices[dimPointer]++; 
					if (++findEl == lElements)
					{
						break;
					}
				}
				//carry stuff
				else
				{	
					//magic
					while (++dimPointer <= cDims)
					{
						if (rgIndices[dimPointer] < ((LONG)psaBound[dimPointer].cElements + psaBound[dimPointer].lLbound-1))
						{
							rgIndices[dimPointer]++;
							break;
						}
					}						
					
					//reset previous cols to initial lowerbound from left to 
					// most right carry position
					for (LONG z = 0; z < dimPointer; z++)
						rgIndices[z] = psaBound[z].lLbound;

					dimPointer= 0;			
				}
			}
			logModule.set_Logging(backup);
			::SafeArrayUnlock(psa);
			logModule.Write(L"VT_VARIANT array elements %d %x", findEl, hr);
		}
		
		TheValue->parray= psa;
		TheValue->vt = lType;
	}
	//not an array just a single variable.
	else
	{
        TheValue->vt = vtype;
		switch (vtype & (VT_ARRAY - 1))
		{
			case VT_NULL: case VT_EMPTY:
				cBytes = 0;
				break;
			case VT_I1: case VT_UI1:
				cBytes = sizeof(BYTE);
				break;
			case VT_I2:	case VT_UI2: case VT_BOOL:
				cBytes = sizeof(VARIANT_BOOL);
				break;
			case VT_I4:	case VT_UI4: case VT_R4: case VT_INT: case VT_UINT: case VT_ERROR:
				cBytes = sizeof(SCODE);
				break;
			case VT_I8:	case VT_UI8: case VT_CY: case VT_R8: case VT_DATE:
				cBytes = sizeof(LONGLONG);
				break;
			case VT_DECIMAL:				
				cBytes = sizeof(DECIMAL);
				hr = pStream->Read(TheValue, cBytes, nullptr);
				break;
			//case VT_RECORD: // can happen within .NET
			//	{
			//		wire_UDT nfo;
			//		hr = pStream->Read(&nfo, sizeof(wire_UDT), nullptr);
			//		
			//		if (nfo.ifaceSize != 0) 
			//		{
			//			HGLOBAL hglob = GlobalAlloc(GMEM_MOVEABLE, nfo.ifaceSize);
			//			if (hglob != nullptr )
			//			{
			//				logModule.Write(L"stream size=%d, recordsize %d", nfo.ifaceSize, nfo.clSize);
			//				hr = pStream->Read(GlobalLock(hglob), nfo.ifaceSize, nullptr);												
			//				GlobalUnlock(hglob);
			//				IStream*pTemp;
			//				hr = CreateStreamOnHGlobal(hglob, TRUE, &pTemp);
			//				if (hr == S_OK)
			//				{
			//					TheValue->pRecInfo = nullptr;
			//					hr = CoUnmarshalInterface(pTemp, 
			//						IID_IRecordInfo, 
			//						(void**)&TheValue->pRecInfo);
			//					pTemp->Release();
			//				}
			//				logModule.Write(L"CoUnmarshalInterface %x", hr);
			//			}
			//		}

			//		//allocate memory -not- using our own memory allocator!
			//		if (hr == S_OK) 
			//		{
			//			logModule.Write(L"pRecInfo->RecordCreate");
			//			TheValue->pvRecord = TheValue->pRecInfo->RecordCreate();				
			//			hr = pStream->Read(TheValue->pvRecord, nfo.clSize, nullptr);
			//		}
			//		//if error skip to next variable
			//		else
			//		{
			//			//hr = pStream->Seek(nfo.clSize, STREAM_SEEK_CUR, nullptr);
			//			goto error;
			//		}
			//	}
			//	cBytes = 0;
			//	break;
			case VT_BSTR:
				hr = ReadString(pStream, &TheValue->bstrVal);
				cBytes = SysStringByteLen(TheValue->bstrVal);
				break;
			case VT_DISPATCH: //fall through VT_UNKNOWN
			case VT_UNKNOWN:
			{
				//Puts the IID_IStream interface as a type of the TheValue->punkVal
				hr = pStream->Read (&cBytes, sizeof(cBytes), nullptr);
				if (cBytes > 0 && SUCCEEDED(hr))
				{
					HGLOBAL hGlob = ::GlobalAlloc(GMEM_MOVEABLE, cBytes);
					hr = pStream->Read (::GlobalLock(hGlob), cBytes, nullptr);
					::GlobalUnlock(hGlob);
					IStream * pTemp;
					hr = ::CreateStreamOnHGlobal(hGlob, TRUE, &pTemp);
					if (hr == S_OK)
					{
						pTemp->QueryInterface(IID_IUnknown, (void**)&TheValue->punkVal);
						pTemp->Release();

					}
					else
					{
						TheValue->vt = VT_EMPTY;
						hr = S_FALSE;
					}
				}
				else
				{
					TheValue->vt = VT_EMPTY;
					hr = S_FALSE;//indicate no data
				}
				break;
			}  
			default:
				hr = E_INVALIDARG;
		}
		exit:
		logModule.Write(L"Done variant type=%d, size=%d", vtype, cBytes);
		if (cBytes > 0 && vtype != VT_BSTR && vtype != VT_UNKNOWN && vtype != VT_DISPATCH && vtype != VT_DECIMAL) 
			hr = pStream->Read(&TheValue->bVal, cBytes, nullptr);
	}
//error:
	if (FAILED(hr)) 	
	{
		VariantClear(TheValue);
		ZeroMemory(TheValue, sizeof(VARIANT));
	}

	return hr;
}

STDMETHODIMP CVariantDictionary::LocalContents(DWORD * lSize, IStream **pSequentialStream) throw()
{
	HRESULT hr = S_OK;
	VARIANT cVarTemp = { VT_EMPTY };
	
	CComBSTR stemp;
	CComObject<CStream>* cseqs;
	CComObject<CStream>::CreateInstance(&cseqs);
	ULARGE_INTEGER ul;
	ul.QuadPart = 128;

	cseqs->SetSize(ul);
	cseqs->QueryInterface(IID_IStream, (void**)pSequentialStream);
	
	if (hr == S_OK)
	{		
		INT m_lVDictElements = _dictionary.size();
		logModule.Write(L"LocalContents Elements %d", m_lVDictElements);
		cVarTemp.vt = VT_I4;
		cVarTemp.intVal = m_lVDictElements;
		stemp = L"Els";
		hr = WriteProperty(*pSequentialStream, stemp, &cVarTemp);
		cVarTemp.intVal = 0;	
	
		//reverse loopthrough
		for (auto key = _dictionary.crbegin(); key != _dictionary.crend(); ++key)
		{
			hr = WriteProperty(*pSequentialStream, key->first, &key->second);			
		} ;
		

		// get current seek pointer, ie, the current size.
		//		hr = (*pSequentialStream)->Length(lSize);
		STATSTG pstatstg = { 0 };
		(*pSequentialStream)->Stat(&pstatstg, STATFLAG_NONAME);
		*lSize = pstatstg.cbSize.LowPart;

		LARGE_INTEGER li;
		li.QuadPart = 0;
		hr = (*pSequentialStream)->Seek(li, STREAM_SEEK_SET, nullptr);
	}

	//(*pSequentialStream)->Release();
	logModule.Write(L"localContents %x", hr);
	return hr;
}

STDMETHODIMP CVariantDictionary::get_Exists(VARIANT vKey, VARIANT_BOOL* pVal) throw()
{

	CComVariant keycopy;
	HRESULT hr = keycopy.ChangeType(VT_BSTR, &vKey);
	if (SUCCEEDED(hr))
	{
		auto pos = _dictionary.find(keycopy.bstrVal);
		*pVal = (pos == _dictionary.end()) ? VARIANT_FALSE : VARIANT_TRUE;
	}
	return hr;
}
STDMETHODIMP CVariantDictionary::get_VarType(VARIANT vKey, SHORT *vType) throw()
{
	
	CComVariant keycopy;
	HRESULT hr = keycopy.ChangeType(VT_BSTR, &vKey);
	if (FAILED(hr))
	{
		return hr;
	}
	auto pos = _dictionary.find(keycopy.bstrVal);

	*vType = VT_EMPTY;

	if (pos != _dictionary.end())
	{
		*vType = pos->second.vt;
	}
	return hr;
}
STDMETHODIMP CVariantDictionary::get_CaseSensitive(VARIANT_BOOL* ) throw()
{
	return E_NOTIMPL;
}

STDMETHODIMP CVariantDictionary::put_CaseSensitive(VARIANT_BOOL ) throw()
{
	
	return E_NOTIMPL;
}


STDMETHODIMP CVariantDictionary::Persist(VARIANT vKey) throw()
{
	HRESULT hr = S_OK;
	logModule.Write(L"Persist %s", vKey.bstrVal);
	if (this->readOnly == TRUE)
	{	
		hr =  E_FAIL;
		//ReportComError2(hr, ERROR_READONLY);
		Error(ERROR_READONLY, GetObjectCLSID());
		return hr;
	}

	VARTYPE origType;
	
	CComVariant keycopy;
	keycopy.ChangeType(VT_BSTR, &vKey);
	auto f = _dictionary.find(keycopy.bstrVal);

	if (f == _dictionary.end())
	{
		hr = E_INVALIDARG;
		Error(L"Key not found", GetObjectCLSID());
		return hr;
	}
	
	origType = f->second.vt;
	if ((origType & VT_DISPATCH ) == VT_DISPATCH || (origType & VT_UNKNOWN) == VT_UNKNOWN)
	{
		auto pos = _isserialized.find(keycopy.bstrVal);
		bool isSerialized = pos != _isserialized.end() && pos->second;
		// it is an unpersisted object, not yet packed as IStream
		// do that first then...
		// convert the object to a valid instance of IStream
		if (!isSerialized) 
		{
			pos->second = true;			
			CComPtr<IStream> l_pIStr;
			hr = CreateStreamOnHGlobal(nullptr, TRUE, &l_pIStr);					
			CComQIPtr<IPersistStream> pPersist(f->second.punkVal);
			
			bool doPersist = false;
			if (pPersist == nullptr)
			{
				CComQIPtr<IPersistStreamInit> pPersist2(f->second.punkVal);
				doPersist = true;
				hr = OleSaveToStream2(pPersist2, l_pIStr);				
			}
			else
			{
				hr = OleSaveToStream(pPersist, l_pIStr);				
			}
			logModule.Write(L"OleSaveToStream%s %x", doPersist? L"2" : L"0", hr);
			//clear the unpersisted object
			f->second.ClearToZero();
			if (l_pIStr != nullptr)
			{
				//f->second = l_pIStr;
				l_pIStr.QueryInterface(&f->second.punkVal);
				f->second.vt = origType;
			}
			else
				f->second.vt = VT_EMPTY;

		}
	}
	else
		this->Error(L"Invalid Data Type. It should be persistable!", GUID_NULL, E_INVALIDARG);
	
	return hr;
}

STDMETHODIMP CVariantDictionary::put_Readonly(VARIANT_BOOL newVal) throw()
{
	this->readOnly = newVal == VARIANT_TRUE ? TRUE : FALSE;
	return S_OK;
}

/***
 *writes a string in utf-8 compressed format
 *
 ***/
STDMETHODIMP CVariantDictionary::WriteString(IStream *pStream, BSTR TheVal) throw()
{
	HRESULT hr = S_OK;

	UINT lTempSize = ::SysStringLen(TheVal);
	UINT test  = 0;
#ifdef DEBUG
	UINT testlen = (UINT)wcslen(TheVal);
	if (testlen != lTempSize) 
	{
		logModule.Write(L"wcslen(TheValue != SysStringLen(TheVal) %d, %d\n", testlen, lTempSize);
		return E_FAIL;
	}
#endif
	
	//WideCharToMultiByte includes the terminating \0
	// lTempSize must be + 1 because of the terminating 0!
	if (lTempSize > 0)
	{
		UINT byteswritten = ::WideCharToMultiByte(CP_UTF8, 0, TheVal, lTempSize + 1, nullptr, 0, nullptr, nullptr);
		if (byteswritten == 0)
			hr = ATL::AtlHresultFromLastError();
		else
		{
			if (byteswritten > m_dwMultiLen)
			{
				hr = AllocPSTR(&m_lpstrMulti, byteswritten, &m_dwBufSize);
				m_dwMultiLen = byteswritten;
				logModule.Write(L"bufsize %d, bytes written %d in AllocPSTR %x", m_dwBufSize, byteswritten, hr);
				if (FAILED(hr))
				{
					test = 0;
					pStream->Write(&test, sizeof(UINT), nullptr);
					return hr;
				}
			}
			UINT test = ::WideCharToMultiByte(CP_UTF8, 0, TheVal, lTempSize + 1, (PSTR)m_lpstrMulti, byteswritten, nullptr, nullptr);
			if (test > 0)
			{
				test--; //exclude terminating zero
				hr = pStream->Write(&test, sizeof(UINT), nullptr);
				if (hr == S_OK)
					hr = pStream->Write(m_lpstrMulti, test, nullptr);
				logModule.Write(L"WriteString Bytes %d", test);
			}
		}
	}
	else //empty string
	{
		test = 0; 
		hr = pStream->Write(&test, sizeof(UINT), nullptr);
	}
	return hr;
}

/*
 * decompresses a string (BSTR) from utf-8 format and also tests for old (legacy) utf-16 format
 */
STDMETHODIMP STDMETHODCALLTYPE CVariantDictionary::ReadString(IStream *pStream, BSTR *retval) throw()
{
	HRESULT hr = S_OK;
	UINT lTempSize = 0;
	// size excludes the terminating zero!
	hr = pStream->Read(&lTempSize, sizeof(UINT), nullptr);
	if (hr == S_OK)
	{
		//TODO: find how vbNullString is written to the native Session
		// because vbNullString != "" (string with zero length)
		ATLASSERT(lTempSize < 1000000);
		if (lTempSize == 0)
		{
			::SysReAllocStringLen(retval, nullptr, (UINT) 0);
			logModule.Write(L"ReadString empty");
		}
		else if (lTempSize > 0) 
		{
			//warning! This is a multibyte encoded string!
			//realloc
			if (lTempSize > m_dwMultiLen)
			{
				hr = AllocPSTR(&m_lpstrMulti, lTempSize, &m_dwBufSize)	;
				m_dwMultiLen = lTempSize;
			}	
			if (hr == S_OK)
			{
				hr = pStream->Read(m_lpstrMulti, lTempSize, nullptr);
				//because we specify the exact length, writtenbytes is excluding the terminating 0
				UINT writtenbytes = ::MultiByteToWideChar(CP_UTF8, 0, (PSTR)m_lpstrMulti, lTempSize, nullptr, 0);	
				if (writtenbytes == 0)
					hr = ATL::AtlHresultFromLastError();
				else
				{
					if (::SysReAllocStringLen(retval, nullptr, writtenbytes) != FALSE)
						::MultiByteToWideChar(CP_UTF8, 0, (PSTR)m_lpstrMulti, lTempSize, *retval, writtenbytes);
					else
						hr = E_OUTOFMEMORY;
				}//succeed utf-8 to utf-16
				//}//isunicode
				logModule.Write(L"ReadString %d, %d, %x", lTempSize, writtenbytes, hr);
			} //enough memory
			else
			{
				logModule.Write(L"AllocPSTR failed %x", hr);
			}
		}
		
	} //read size OK
	return hr;
}