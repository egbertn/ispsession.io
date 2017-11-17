#include "stdafx.h"
//TODO: 15-10-2017 make a REAL headerfile for CSessionDL because the split functions are not included at all in secundary includes.
#include "CSessionDL.h"
#include "ConfigurationManager.h"
#include "application.h"
#include "CStream.h"
#include "tools.h"
#include "CEnum.h"


STDMETHODIMP NWCApplication::OnStartPage(IUnknown *aspsvc) throw()
{
	logModule.Write(L"Application:OnStartPage");
	if (aspsvc == nullptr)
	{
		return E_POINTER;
	}
	HRESULT hr = aspsvc->QueryInterface(&m_pScriptContext);
	if (FAILED(hr))
	{
		this->Error(L"Could not get ASP Scripting context", this->GetObjectCLSID(), hr);
		return hr;
	}
	if (FAILED(m_pScriptContext->get_Request(&m_piRequest))) return E_FAIL;
	if (FAILED(m_pScriptContext->get_Response(&m_piResponse))) return E_FAIL;
	if (FAILED(m_pScriptContext->get_Server(&m_piServer))) return E_FAIL;
	hr = ReadConfigFromWebConfig();
	if (FAILED(hr))
	{
		return hr;
	}
	
	hr = InitializeDataSource();
	m_OnStartPageCalled = TRUE;

	if (FAILED(hr))
	{
		m_piResponse->Write(CComVariant(L"Application: Redis Server is not running."));
		hr = E_FAIL;
	}
//#ifndef Demo
//	if (licenseOK == false)
//	{
//		m_piResponse->Write(CComVariant(L"The license for this server is invalid. Please contact ADC Cure for an updated license at information@adccure.nl"));
//	}
//#endif
	return hr;

}
STDMETHODIMP NWCApplication::OnEndPage() throw()
{
	HRESULT hr = PersistApplication();
	logModule.Write(L"Application:OnEndPage");
	return hr;
}
STDMETHODIMP NWCApplication::IsDirty(BOOL* pRet) throw()
{
	*pRet = FALSE;
	for(auto k = _dictionary.begin(); k != _dictionary.end(); ++k)
	{
		if (k->second.IsDirty == TRUE || k->second.IsNew == TRUE)
		{
			*pRet = TRUE;
			break;
		}
	}
	return S_OK;
}
STDMETHODIMP NWCApplication::PersistApplication() throw()
{
	HRESULT hr = S_OK;
	if (m_bErrState == TRUE)
	{
		return S_OK;
	}
	BOOL blnIsDirty;
	hr = IsDirty(&blnIsDirty);
	logModule.Write(L"PersistApplication err=%d, dirty=%d", m_bErrState, blnIsDirty);
	
	DWORD lSize = 0;

	if (blnIsDirty == TRUE)
	{	
		auto totalRequestTimeMS = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - m_startSessionRequest).count();
		hr = CApplicationDL::ApplicationSave(pool, (PUCHAR)&m_AppKey, this, lSize, m_dbTimeStamp, (LONG)totalRequestTimeMS);		

		logModule.Write(L"CApplicationDL::ApplicationSave  size(%d) time(%d), hr(%x)", lSize, totalRequestTimeMS, hr);

	}
	
	return hr;
}
STDMETHODIMP NWCApplication::get_Value(BSTR Key, VARIANT* pVal) throw()
{
	HRESULT hr = S_OK;
	auto pos = _dictionary.find(Key);
	if (pos == _dictionary.end())
	{
		pVal->vt = VT_EMPTY;
		pVal->bstrVal = nullptr;
		return S_OK;
	}
	VARTYPE origType = pos->second.val.vt;
	if (origType == VT_UNKNOWN || origType == VT_DISPATCH)
	{
		// first test whether or not this already was unpacked by testing if it is a VT_UI1 | VT_ARRAY type
	
		bool isSerialized = pos->second.IsSerialized == TRUE;

		// already persisted? Just AddReff and return the value
		if (!isSerialized)
		{
			logModule.Write(L"Already DeSerialized %s", Key);
			pos->second.val.punkVal->QueryInterface(&pVal->punkVal);
			pVal->vt = origType;
		}
		else
		{
			CComQIPtr<IStream> l_pIStr(pos->second.val.punkVal);
			hr = l_pIStr->Seek(SEEK_NULL, STREAM_SEEK_SET, nullptr);
			pos->second.val.Clear();//remove the IStream, it is not the object itself
			hr = OleLoadFromStream(l_pIStr, IID_IUnknown, (void**)&pVal->punkVal);
			bool doPersist2 = false;
			if (hr == E_NOINTERFACE) //IPersistStream not found!
			{
				doPersist2 = true;
				//set the ptr sizeof(CLSID) back because the GetClassId advanced 16 bytes in the stream
				LARGE_INTEGER dbMove;
				dbMove.QuadPart = -(long long)sizeof(CLSID);
				hr = l_pIStr->Seek(dbMove, STREAM_SEEK_CUR, nullptr);
				hr = OleLoadFromStream2(l_pIStr, IID_IUnknown, (void**)&pVal->punkVal);
			}
			logModule.Write(L"OleLoadFromStream%s %x", doPersist2 ? L"2" : L"", hr);
			pos->second.IsSerialized = FALSE; //don't deserialize next time
			if (hr == S_OK)
			{
				// put the unpersisted object reference back in the variant.
				pVal->punkVal->QueryInterface(&pos->second.val.punkVal);
				pos->second.val.vt = pVal->vt = origType;
			}
			else
				pVal->vt = VT_EMPTY;
		}
	}	
	else
	{
		hr = ::VariantCopy(pVal, &pos->second.val);
		if (hr != S_OK) logModule.Write(L"VariantCopy %x", hr);
	}
	if (FAILED(hr))
	{
		Error(L"get_Item failed", GetObjectCLSID(), hr);
	}
	return hr;
}

STDMETHODIMP NWCApplication::put_Value(BSTR key, VARIANT newVal) throw()
{
	HRESULT hr = S_OK;
	auto pos = _dictionary.find(key);
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

	if (pos == _dictionary.end())
	{
		ElementModel v = { 0 };
		v.IsNew =
		v.IsDirty = TRUE;//both should be true
		//add it with an empty value and find it again
		logModule.Write(L"add key %s", key);

		_dictionary.insert(pair<CComBSTR, ElementModel>(key, v));
		pos = _dictionary.find(key);
	}
	if (vDeref.vt == VT_DISPATCH)
	{
		if (vDeref.pdispVal == nullptr)
			hr = E_POINTER;
		else
		{
			CComVariant varResolved;
			DISPPARAMS  dispParamsNoArgs = { 0 };
			if (vDeref.pdispVal->Invoke(DISPID_VALUE, IID_NULL, LOCALE_SYSTEM_DEFAULT,
				DISPATCH_PROPERTYGET | DISPATCH_METHOD,
				&dispParamsNoArgs, &varResolved, nullptr, nullptr) == S_OK)
			{
				//recursion...
				hr = put_Value(key, varResolved);
			}
		}
	}
	else
	{
		pos->second.val = vDeref;
	}
	return hr;
}

STDMETHODIMP NWCApplication::putref_Value(BSTR key, VARIANT newVal) throw()
{
	HRESULT hr = S_OK;
	VARIANT* valueArray = nullptr;
	logModule.Write(L"putref_Item %s", key);
	VARTYPE origType = newVal.vt;

	if ((origType & VT_DISPATCH) == VT_DISPATCH || (origType & VT_UNKNOWN) == VT_UNKNOWN)
	{
		VARIANT vDeref = { 0 };

		if ((origType & (VT_VARIANT | VT_BYREF)) == (VT_BYREF | VT_VARIANT))
		{
			vDeref = *newVal.pvarVal;
			logModule.Write(L"putref_Item dereffed object variant");
			vDeref.vt = origType & ~VT_BYREF;
		}
		else
			vDeref = newVal;
		auto pos = _dictionary.find(key);

		if (pos == _dictionary.end())
		{
			logModule.Write(L"add key %s", key);
			ElementModel v = { 0 };
			v.val = vDeref;
			v.IsDirty = v.IsNew = TRUE;
			_dictionary.insert(pair<CComBSTR, ElementModel>(key, v));
			pos = _dictionary.find(key);
		}
		valueArray = &pos->second.val;

		
		CComQIPtr<IPersistStream> l_ptestForCapability(vDeref.pdispVal);
		CComQIPtr<IPersistStreamInit> l_ptestForCapability2(vDeref.pdispVal);
		if (l_ptestForCapability == nullptr && l_ptestForCapability2 == nullptr)
		{
			hr = E_NOINTERFACE;
		}
		else
		{
			pos->second.val = vDeref;
		}

		if (FAILED(hr))
		{
			Error(L"Object does not support IPersistStream", GetObjectCLSID(), hr);
			logModule.Write(L"Object does not support IPersistStream");
		}
	}
	else //putref should receive an object reference/instance
	{
		hr = E_POINTER;
		Error(L"This variable is not an object", GetObjectCLSID(), hr);
		logModule.Write(L"putref_Item not VT_DISPATCH or VT_UNKNOWN but %d", origType);
	}
	return hr;
}
STDMETHODIMP NWCApplication::get_Key(INT Index, BSTR* pVal) throw()
{
	HRESULT hr = S_OK;
	if (SUCCEEDED(hr))
	{
		if ((Index < 1) || (Index > _dictionary.size() + 1))
			hr = E_INVALIDARG;
		INT ct = 0; //terrible loop map has no index, so find it
		for (auto idx = _dictionary.begin(); idx != _dictionary.end(); ++idx)
		{
			if (++ct == Index)
			{
				return idx->first.CopyTo(pVal);
			}
		}
	}
	return hr;
}

STDMETHODIMP NWCApplication::get_Count(PINT pVal) throw()
{
	*pVal = (int)_dictionary.size();
	return S_OK;
}
STDMETHODIMP NWCApplication::get_KeyExists(BSTR Key, VARIANT_BOOL* pVal) throw()
{
	auto pos = _dictionary.find(Key);
	*pVal = pos == _dictionary.end() ? VARIANT_FALSE : VARIANT_TRUE;
	return S_OK;
}

STDMETHODIMP NWCApplication::get_KeyType(BSTR Key, VARTYPE* pVal) throw()
{
	auto pos = _dictionary.find(Key);
	*pVal = pos->second.val.vt;
	return S_OK;
}
STDMETHODIMP NWCApplication::_NewEnum(IUnknown **ppRet) throw()
{
	HRESULT hr = S_OK;
	ULONG size = (ULONG)_dictionary.size();
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
	hr = m_enum->QueryInterface(IID_IUnknown, (void**)ppRet);
	return hr;
}
STDMETHODIMP NWCApplication::RemoveKey(BSTR Key) throw()
{
	HRESULT hr = S_OK;
	auto pos = _dictionary.find(Key);
	if (pos != _dictionary.end())
	{
		_dictionary.erase(pos);
		//avoid duplicates the vector is not a unique dictionary
		auto found = std::find_if(_removed.begin(), _removed.end(),
			[=](PWSTR  &l){ return StrCmpIW( Key, l)==0; });
		if (found != _removed.end())
		{
			_removed.push_back(Key);
		}
		logModule.Write(L"remove key %s", Key);		
	}
	
	return hr;
}
STDMETHODIMP NWCApplication::RemoveAll() throw()
{
	for (auto k = _dictionary.begin(); k != _dictionary.end(); ++k)
	{
		auto found = std::find_if(_removed.begin(), _removed.end(),
			[=](PWSTR  &l){ return StrCmpIW(k->first, l) == 0; });
		if (found != _removed.end())
		{
			_removed.push_back(k->first);
		}
		_dictionary.erase(k);
	}
	return S_OK;
}

STDMETHODIMP  NWCApplication::LockKey(BSTR Key) throw()
{//will perform a PUT [Guid]:lock 1 command to redis 2.2+
	
	
	CComBSTR appkey(m_AppKey);
	CComBSTR bytesAppKey;
	bytesAppKey.Attach(appkey.ToByteString());
	//https://github.com/jacket-code/redlock-cpp/blob/master/LockExample.cpp
	//dlm->Lock((PSTR)bytesAppKey, 100000, myLock);
	CComBSTR appKey(m_AppKey);

	return S_OK;	
}

STDMETHODIMP NWCApplication::UnlockKey(BSTR key) throw()
{
	
	return S_OK;
}

STDMETHODIMP NWCApplication::ExpireKeyAt(BSTR Key, INT ms) throw()
{
	auto pos = _dictionary.find(Key);
	if (pos != _dictionary.end())
	{
		pos->second.ExpireAt = ms;
		//avoid duplicates the vector is not a unique dictionary
		
		logModule.Write(L"remove key %s", Key);
	}
	return S_OK;
}

STDMETHODIMP NWCApplication::ReadConfigFromWebConfig() throw()
{
	HRESULT hr = S_OK;
	CComBSTR retVal, configFile(L"/web.Config");
	///traverse back to root if necessary, note, UNC paths are not advisable
	CComBSTR root;
	bool exists = false;
	hr = m_piServer->MapPath(configFile, &root);
	configFile.Insert(0, L".");
	//IIS Setting 'enable parent paths' must be enabled
	for (;;)
	{
		retVal.Empty();
		hr = m_piServer->MapPath(configFile, &retVal);
		if (FAILED(hr))
		{
			break;
		}
		if ((exists = FileExists(retVal)) == true)
		{
			break;
		}
		logModule.Write(L"logic %s, phys %s", configFile, retVal);
		//avoid going beyond the root of this IIS website
		if (CComBSTR::Compare(retVal, root, true, false, false) == 0)
		{
			break;
		}
		if (configFile.StartsWith(L"./"))
		{
			configFile.Remove(0, 2);
		}
		configFile.Insert(0, L"../");
	}
	if (exists == FALSE)
	{
		exists = FileExists(root);
		if (exists == TRUE)
		{//last resort
			retVal.Attach(root.Detach());
			hr = S_OK;
		}
	}
	if (exists == FALSE || FAILED(hr))
	{
		logModule.Write(L"Application: searched web.Config up to: (%s) none found %x", retVal, hr);
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
	}
	ConfigurationManager config(retVal);
	const PWSTR prefix = L"ispsession_io:";

	CComBSTR bstrProp = L"APP_KEY";
	bstrProp.Insert(0, prefix);
	bstrProp.Attach(config.AppSettings(bstrProp));
	if (bstrProp.Length() > 0)
	{
		logModule.Write(L"AppKey: (%s)", bstrProp);
	}

#ifndef AppKeyNULL 
	if (setstring(reinterpret_cast<PUCHAR>(&m_AppKey), bstrProp) == FALSE)
		// we could do it without appkey it becomes GUID_NULL
		ZeroMemory(&m_AppKey, sizeof(GUID));
#endif	


	return hr;
}


// Opens a DB Connection and initialises the Dictionary with the binary contents
STDMETHODIMP NWCApplication::InitializeDataSource() throw()
{
	PCWSTR location = L"InitializeDataSource";
	int port = 5578;
	HRESULT hr = S_OK;
	//dlm->AddServerUrl("", port); //TODO: get con string
	m_startSessionRequest = std::chrono::system_clock::now();
	if (SUCCEEDED(hr))
	{
		auto success = CSessionDL::OpenDBConnection(std::wstring(m_strConstruct), pool);
		if (!success)
		{
			hr = E_FAIL;
		}
	}
	if (FAILED(hr))
	{
		ReportComError2(hr, location);
		m_bErrState = TRUE;
	}
	dlm = new CRedLock();
	CApplicationDL applicationDl;
	hr = applicationDl.ApplicationGet(pool, (PUCHAR)&m_AppKey);
	if (SUCCEEDED(hr))
	{
	//	((PLONGLONG)(&m_dbTimeStamp)) = (LONGLONG)(applicationDl.m_LastUpdated);
	}
	if (SUCCEEDED(hr) && applicationDl.IsNULL == FALSE && applicationDl.m_blobLength > 0)
	{
		CComObject<CStream>* cseqs;
		CComObject<CStream>::CreateInstance(&cseqs);
		cseqs->AddRef();
		const INT bufSize = 0x1000;
		ULONG read = 0;
		BYTE buf[bufSize]; //stack memory
		hr = S_OK;
		do
		{
			hr = applicationDl.m_pStream->Read(buf, bufSize, &read);
			if (read > 0)
			{
				cseqs->Write(buf, read, nullptr);
			}
		} while (read != 0 && hr == S_OK);

		hr = S_OK;
		LARGE_INTEGER nl = { 0 };
		cseqs->Seek(nl, STREAM_SEEK_SET, nullptr);
		/*hr = m_piVarDict->LocalLoad(cseqs, applicationDl.m_blobLength);
		if (FAILED(hr))
		{
			logModule.Write(L"loading Application failed %x", hr);
		}*/
		cseqs->Release();
	}
	return hr;
}
STDMETHODIMP NWCApplication::ReadString(std::istream& pStream, BSTR *retval) throw()
{
	HRESULT hr = S_OK;
	UINT lTempSize = 0;
	auto pRet = new std::string();
	
	// size excludes the terminating zero!
	pStream.read((char*)&lTempSize, sizeof(UINT));
	if (hr == S_OK)
	{
		//TODO: find how vbNullString is written to the native Session
		// because vbNullString != "" (string with zero length)
		ATLASSERT(lTempSize < 1000000);
		if (lTempSize == 0)
		{
			::SysReAllocStringLen(retval, nullptr, (UINT)0);
			logModule.Write(L"ReadString empty");
		}
		else if (lTempSize > 0)
		{
			//warning! This is a multibyte encoded string!
			//realloc
			if (lTempSize > m_lpstrMulti.capacity())
			{
				m_lpstrMulti.reserve((lTempSize / 512) * 512 + 2048);
				
				m_dwMultiLen = lTempSize;
			}
			if (hr == S_OK)
			{
				
				pStream.read((char*)m_lpstrMulti.data(), lTempSize);
				//because we specify the exact length, writtenbytes is excluding the terminating 0
				UINT writtenbytes = ::MultiByteToWideChar(CP_UTF8, 0, m_lpstrMulti.data(), lTempSize, nullptr, 0);
				if (writtenbytes == 0)
					hr = ATL::AtlHresultFromLastError();
				else
				{
					if (::SysReAllocStringLen(retval, nullptr, writtenbytes) != FALSE)
						::MultiByteToWideChar(CP_UTF8, 0, (PSTR)m_lpstrMulti.data(), lTempSize, *retval, writtenbytes);
					else
						hr = E_OUTOFMEMORY;
				}
				
				logModule.Write(L"ReadString %d, %d, %x", lTempSize, writtenbytes, hr);
			} //enough memory
			else
			{
				logModule.Write(L"ReadString failed %x", hr);
			}
		}

	} //read size OK
	return hr;

}
///<summary>
///reads a VARIANT value from the stream (pStream). Arrays are recusively processed
///</summary>
///<param name="pStream">asdf</param>
///<param name="TheValue">asdf</param>
///<param name="vtype">VT variant type</param>
///<returns>a HRESULT</returns>
STDMETHODIMP NWCApplication::ReadValue(std::istream& pStream, VARIANT* TheValue, VARTYPE vtype) throw()
{
	LONG cBytes = 0,
		ElSize = 0,
		cDims = 0,
		lElements = 0,
		lMemSize = 0;
	VARTYPE lType = VT_EMPTY;
	// we reassign the value
	::VariantClear(TheValue);
	ZeroMemory(TheValue, sizeof(VARIANT));

	PVOID psadata = nullptr;
	HRESULT hr = S_OK;
	INT els = 0;
	if ((vtype & VT_ARRAY) == VT_ARRAY)
	{
		ARRAY_DESCRIPTOR descriptor;
		pStream.read((char*)&descriptor, sizeof(ARRAY_DESCRIPTOR));
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
			pStream.read((char*)&safebound[cx], sizeof(SAFEARRAYBOUND));
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
			(vtype == VT_UI1) || (vtype == VT_I2) || (vtype == VT_I4) || (vtype == VT_R4) || (vtype == VT_R8)
			|| (vtype == VT_CY) || (vtype == VT_DATE)
			|| (vtype == VT_BOOL) || (vtype == VT_I8) || (vtype == VT_I1)
			)
		{
			//retrieve pointer to first address
			hr = ::SafeArrayAccessData(psa, &psadata);
			if (hr == S_OK)
			{
				pStream.read((char*)psadata, lMemSize);
				::SafeArrayUnaccessData(psa);
			}
		}
		else if ((vtype == VT_BSTR || vtype == VT_DECIMAL) && lElements > 0)
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
			for (;;)
			{
				if (rgIndices[dimPointer] <
					(LONG)psaBound[dimPointer].cElements + psaBound[dimPointer].lLbound)
				{
					VARIANT* pVar;
					hr = ::SafeArrayPtrOfIndex(psa, rgIndices, (void**)&pVar);
					//logModule.Write(L"Indices %d,%d,%d", rgIndices[0], rgIndices[1], rgIndices[2]);
					if (FAILED(hr))
						logModule.Write(L"FATAL: SafeArrayPtrOfIndex failed %x", hr);

					if (hr == S_OK)
					{
						vtype = pVar->vt;
						pStream.read((char*)&vtype, sizeof(VARTYPE));
						
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

					dimPointer = 0;
				}
			}
			logModule.set_Logging(backup);
			::SafeArrayUnlock(psa);
			logModule.Write(L"VT_VARIANT array elements %d %x", findEl, hr);
		}

		TheValue->parray = psa;
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
			pStream.read((char*)TheValue, cBytes);
			break;
		case VT_BSTR:
			hr = ReadString(pStream, &TheValue->bstrVal);
			cBytes = SysStringByteLen(TheValue->bstrVal);
			break;
		case VT_DISPATCH: //fall through VT_UNKNOWN
		case VT_UNKNOWN:
		{
			//Puts the IID_IStream interface as a type of the TheValue->punkVal
			pStream.read((char*)&cBytes, sizeof(cBytes));
			if (cBytes > 0)
			{
				HGLOBAL hGlob = ::GlobalAlloc(GMEM_MOVEABLE, cBytes);
				pStream.read((char*)::GlobalLock(hGlob), cBytes);
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
			pStream.read((char*)&TheValue->bVal, cBytes);
	}
	//error:
	if (FAILED(hr))
	{
		VariantClear(TheValue);
		ZeroMemory(TheValue, sizeof(VARIANT));
	}

	return hr;
}

/***
*writes a string in utf-8 compressed format
*
***/
STDMETHODIMP NWCApplication::WriteString(BSTR TheVal, std::string& pStream) throw()
{
	HRESULT hr = S_OK;

	UINT lTempSize = ::SysStringLen(TheVal);
	UINT test = 0;


	//WideCharToMultiByte includes the terminating \0
	// lTempSize must be + 1 because of the terminating 0!
	if (lTempSize > 0)
	{
		
		UINT byteswritten = ::WideCharToMultiByte(CP_UTF8, 0, TheVal, lTempSize + 1, nullptr, 0, nullptr, nullptr);
		if (byteswritten == 0)
			hr = ATL::AtlHresultFromLastError();
		else
		{
			if (byteswritten > m_lpstrMulti.capacity())
			{
				try
				{
					m_lpstrMulti.reserve((byteswritten / 512) * 512 + 1048);
				}
				catch (std::bad_alloc& e)
				{
					pStream.append((char*)&test, sizeof(UINT));
					return E_OUTOFMEMORY;
				}
				catch (std::length_error& e)
				{
					
					test = 0;
					pStream.append((char*)&test, sizeof(UINT));
					return E_OUTOFMEMORY;					
				}
				
				
			}
			m_lpstrMulti.resize(test+sizeof(UINT));
			UINT test = ::WideCharToMultiByte(CP_UTF8, 0, TheVal, lTempSize + 1, (PSTR)m_lpstrMulti.data() + sizeof(UINT), byteswritten, nullptr, nullptr);
			if (test > 0)
			{
				test--; //exclude terminating zero
				memcpy((void*)m_lpstrMulti.data(), &test, sizeof(test));
				pStream.append(m_lpstrMulti, 0, test + sizeof(test));
				logModule.Write(L"WriteString Bytes %d", test);
			}
		}
	}
	else //empty string
	{
		test = 0;
		pStream.append((char*)&test, sizeof(UINT));
	}
	return hr;
}
STDMETHODIMP NWCApplication::ConvertObjectToStream(VARIANT &var) throw()
{
	if (var.vt != VT_UNKNOWN || var.punkVal == nullptr)
	{
		return E_INVALIDARG;
	}
	
	CComQIPtr<IPersistStream> pPersist(var.punkVal);
	CComPtr<IStream> pStream;
	HRESULT hr = CreateStreamOnHGlobal(nullptr, TRUE, &pStream);
	bool noPersist2 = false;
	if (hr == S_OK)
	{
		if (pPersist == nullptr)
		{
			noPersist2 = true;
			CComQIPtr<IPersistStreamInit> pPersist2(var.punkVal);
			if (pPersist2 == nullptr)
			{
				hr = E_NOINTERFACE;
			}
			hr = OleSaveToStream2(pPersist2, pStream);
		}
		else
		{
			hr = ::OleSaveToStream(pPersist, pStream);
		}
		logModule.Write(L"OleSaveToStream%s %x", noPersist2 ? L"2" : L"", hr);
		if (hr == S_OK)
		{
			var.punkVal->Release();//release object instance
			pStream.QueryInterface(&var.punkVal);
		}
	}
	return hr;
}
STDMETHODIMP NWCApplication::SerializeKey(BSTR Key, std::string& binaryString) throw()
{
	HRESULT hr = S_OK;
	auto pos = _dictionary.find(Key);
	if (pos != _dictionary.end())
	{
		std::string retVal;
		// write the complete string including the leading 4 bytes
		HRESULT hr = WriteString(Key, binaryString);
		//variant type
		VARTYPE vtype = pos->second.val.vt;
		if (SUCCEEDED(hr))
		{	
			
			binaryString.append((char*)&vtype, sizeof(VARTYPE));
			if (pos->second.IsSerialized == FALSE)
			{
				hr = ConvertObjectToStream(pos->second.val);
			}
			hr = WriteValue(vtype, pos->second.val, binaryString);

			

			
		}
		logModule.Write(L"WriteProperty propname=%s, type=%d, result=%x", std::wstring(binaryString.begin(), binaryString.end()), vtype, hr);
		/*    ' 1- Len4 PropName var
		' 4- Variant
		'OR----

		' 1- Len4 PropName var
		' 2- vType 2
		' 3- Variant - String / Variant Object / Array

		'IF -- Variant Array
		'
		*/
		return hr;
	}
	return hr;
}

STDMETHODIMP NWCApplication::WriteValue(VARTYPE vtype, VARIANT& TheVal, std::string& binaryString) throw()
{

	LONG cBytes = 0,
		cx = 0, lMemSize = 0, lElements = 0,
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
			psa = *TheVal.pparray;
			vtype ^= VT_BYREF; //mask out
		}
		else
			psa = TheVal.parray;

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
		binaryString.append((char*)&descriptor, sizeof(ARRAY_DESCRIPTOR));

		VARTYPE vcopy = vtype & ~VT_ARRAY;
		if (vcopy == VARENUM::VT_UNKNOWN || vcopy == VARENUM::VT_ERROR || vcopy == VARENUM::VT_VARIANT)
		{
			//because e.g. Redim v(1,1) TypeName(v) == "Variant()"
			CComBSTR bogusAssemblyTypeForDotNet(L"System.Object"); //
			WriteString(bogusAssemblyTypeForDotNet, binaryString);
		}
		else //todo jagged array??
		{
			WriteString(nullptr, binaryString);
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

		for (cx = 1; cx <= cDims; cx++)
		{
			LONG bounds[2];
			hr = ::SafeArrayGetLBound(psa, cx, &bounds[1]);
			hr = ::SafeArrayGetUBound(psa, cx, &bounds[0]);
			bounds[0] = bounds[0] - bounds[1] + 1;
			binaryString.append((char*)bounds, sizeof(SAFEARRAYBOUND));
			psaBound[cx - 1].lLbound = bounds[1];
			psaBound[cx - 1].cElements = bounds[0];
			lMemSize *= bounds[0];
		}

		lElements = lMemSize;
		lMemSize *= ElSize;
		logModule.Write(L"writing array els(%d) elsize(%d)", lElements, ElSize);


		if ((vcopy == VT_I1) ||
			(vcopy == VT_UI1) || (vcopy == VT_I2) || (vcopy == VT_I4) || (vcopy == VT_R4) ||
			(vcopy == VT_R8) || (vcopy == VT_CY) || (vcopy == VT_DATE) || (vcopy == VT_BOOL) || (vcopy == VT_I8)
			)
			//wow, write the whole block at once. Simple, not?
		{
			if (lMemSize > 0)
			{
				hr = ::SafeArrayAccessData(psa, &psadata);
				if (hr == S_OK)
				{
					binaryString.append((char*)psadata, lMemSize);
					::SafeArrayUnaccessData(psa);
				}
			}
		}
		else if ((vcopy == VT_BSTR) || (vcopy == VT_DECIMAL))
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
							hr = WriteString(myarray[els], binaryString);
					}
					else if (vcopy == VT_DECIMAL)
					{
						auto myarray = static_cast<VARIANT*>(psadata); // ugly, but true
						for (; els < lElements && hr == S_OK; els++)
							hr = WriteValue(VT_DECIMAL, myarray[els], binaryString);
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
			if (lElements > 0 && psa != nullptr)
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
				for (;;)
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
						binaryString.append((char*)&vtype, sizeof(VARTYPE));
						//----- recursive call ----- keep an eye on this
						hr = WriteValue(vtype, *pVar, binaryString);

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
						dimPointer = 0;
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
				binaryString.append((char*)psa, sizeof(nullptr));
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
			
		case VT_DECIMAL:
			//correct because decimal eats the whole variant !
			cBytes = sizeof(DECIMAL);
			binaryString.append((char*)&TheVal, cBytes);
			break;
		case VT_BSTR:

			hr = WriteString(TheVal.bstrVal, binaryString);
			cBytes = SysStringByteLen(TheVal.bstrVal);
			break;
		case VT_DISPATCH: //fall through VT_UNKNOWN
		case VT_UNKNOWN:
		{

			CComPtr<IStream> l_pIStr;
			//it is already serialized to IStream, just get the pointer
			hr = TheVal.punkVal->QueryInterface(&l_pIStr);
			
			if (hr == S_OK)
			{
				//get size of stream and write the size

				hr = l_pIStr->Commit(STGC_DEFAULT);
				STATSTG pstatstg = { 0 };
				hr = l_pIStr->Stat(&pstatstg, STATFLAG_NONAME);
				cBytes = pstatstg.cbSize.LowPart;
				logModule.Write(L"Streamsize %d", cBytes);
				// copy the persisted object as bytestream to the main stream	and prefix with stream length	
				binaryString.append((char*)&cBytes, sizeof(pstatstg.cbSize.LowPart));
				unsigned char buf[2048];
				ULONG actualRead = 0;
				HRESULT hr2 = S_OK;
				while (hr2 == S_OK)
				{
					hr2 = l_pIStr->Read(buf, sizeof(buf), &actualRead);
					binaryString.append((char*)buf, actualRead);
				}			
				
			}
			else
			{
				// indicate zero bytes
				cBytes = 0;
				binaryString.append((char*)&cBytes, sizeof(DWORD));
			}

			break;
		}
		default:
			hr = E_INVALIDARG;
		}
	exit: // sorry
		if (cBytes > 0 && vtype != VT_BSTR && vtype != VT_UNKNOWN && vtype != VT_DISPATCH && vtype != VT_DECIMAL)
			binaryString.append((char*)&TheVal.bVal, cBytes);
		logModule.Write(L"Simple variant size=%d", cBytes);
	}

	return hr;
}
STDMETHODIMP NWCApplication::DeserializeKey(BSTR Key, std::string binaryString) throw()
{
	HRESULT hr = S_OK;
	return hr;
}