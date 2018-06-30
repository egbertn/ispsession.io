// CApplication Copyright ADC Cure 2017-2018
#include "stdafx.h"
//TODO: 15-10-2017 make a REAL headerfile for CSessionDL because the split functions are not included at all in secundary includes.
#include "CSessionDL.h"
#include "ConfigurationManager.h"
#include "application.h"
#include "CStream.h"
#include "tools.h"
#include "CEnum.h"

STDMETHODIMP CApplication::HasOnStartPageBeenCalled() throw()
{

	if (m_OnStartPageCalled == FALSE)
	{
		auto hr =  E_FAIL;
		Error(L"Cannot be configured as a singleton (application cache) or just with CreateObject. Server.CreateObject must be used", GetObjectCLSID(), hr);
		return hr;
	}
	return S_OK;
}
//IIS stuff
STDMETHODIMP CApplication::OnStartPage(IUnknown *aspsvc) throw()
{

	logModule.Write(L"Application:OnStartPage");
	if (aspsvc == nullptr)
	{
		return E_POINTER;
	}
#ifdef EXPIREAT 
	DATE MAXTIME = EXPIREAT;
#endif
	CComPtr<IScriptingContext> m_pScriptContext;
	CComPtr<IServer> m_piServer;

	HRESULT hr = aspsvc->QueryInterface(&m_pScriptContext);
	if (FAILED(hr))
	{
		this->Error(L"Could not get ASP Scripting context", this->GetObjectCLSID(), hr);
		return hr;
	}
	if (FAILED(m_pScriptContext->get_Server(&m_piServer))) return E_FAIL;
	
	if (FAILED(hr))
	{
		return hr;
	}
	
	hr = InitializeDataSource(m_piServer);


	if (FAILED(hr))
	{
		hr = E_FAIL;
		Error(L"Application: Redis Server is not running or invalid connection string in web.Config.", this->GetObjectCLSID(), hr);
	}	
	
	else
	{
		m_OnStartPageCalled = TRUE;
	}
#ifndef Demo
	if (m_licenseOK == false)
	{
		CComPtr<IResponse> pResponse;
		m_pScriptContext->get_Response(&pResponse);
		pResponse->Write(CComVariant(L"The Cache Module License for this server is invalid. Please contact ADC Cure for an updated license at informationATadccure.nl"));
	}
#else

	CComBSTR strTemp;
	int t;
	double tmpdate;

	tmpdate = Now();
	CComPtr<IResponse> pResponse;
	m_pScriptContext->get_Response(&pResponse);
	strTemp = L"NWCTools.CSession DEMO Compatible ISP Session Application Cache expired! We would welcome your order at <a href=\"https://ispsession.io/purchase\">order here</a>";
	if (tmpdate > MAXTIME)
	{
		pResponse->Write(CComVariant(strTemp));
	}
	t = strTemp.Length();
	t = t / t;
	if (t == 0)
	{
		return E_ABORT;
	}
#endif
	return hr;

}

//IIS stuff
STDMETHODIMP CApplication::OnEndPage() throw()
{
	HRESULT hr = PersistApplication();
	logModule.Write(L"Application:OnEndPage");
	return hr;
}


STDMETHODIMP CApplication::IsDirty(BOOL* pRet) throw()
{
	auto hr = HasOnStartPageBeenCalled();
	if (FAILED(hr))
	{
		return hr;
	}
	*pRet = FALSE;
	if (_removed.size() > 0)
	{
		*pRet = TRUE;
		return S_OK;
	}
	for(auto k = _dictionary.begin(); k != _dictionary.end(); ++k)
	{
		if (k->second.IsDirty == TRUE || k->second.IsNew == TRUE || k->second.ExpireAt > 0)
		{
			*pRet = TRUE;
			break;
		}
	}
	return hr;
}
STDMETHODIMP CApplication::PersistApplication() throw()
{
	HRESULT hr = S_OK;
	if (m_bErrState == TRUE)
	{
		return S_OK;
	}
	std::string str;
	BOOL blnIsDirty;
	hr = IsDirty(&blnIsDirty);
	logModule.Write(L"PersistApplication err=%d, dirty=%d", m_bErrState, blnIsDirty);
	

	if (blnIsDirty == TRUE)
	{	
		auto totalRequestTimeMS = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - m_startSessionRequest).count();
		CComPtr<IKeySerializer> database = this;
		hr = CApplicationDL::ApplicationSave(pool, m_AppKey, database,  m_dbTimeStamp, (LONG)totalRequestTimeMS);		

		logModule.Write(L"CApplicationDL::ApplicationSave  time(%d), hr(%x)", totalRequestTimeMS, hr);

	}
	
	return hr;
}
STDMETHODIMP CApplication::get_Value(BSTR Key, VARIANT* pVal) throw()
{
	auto hr = HasOnStartPageBeenCalled();
	if (FAILED(hr))
	{
		return hr;
	}
	if (Key == nullptr || ::SysStringLen(Key) == 0)
	{
		Error(L"Key may not be empty or null", this->GetObjectCLSID(), E_INVALIDARG);
		return E_INVALIDARG;
	}
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
		// first test whether or not this already was unpacked b
		
		if (pos->second.IsSerialized == TRUE)
		{		
			hr = ConvertVStreamToObject(pos->second);
		}		
		if (hr == S_OK)
		{
			pos->second.val.punkVal->QueryInterface(&pVal->punkVal);
			pVal->vt = origType;
		}
	}	
	else
	{
		hr = pos->second.val.CopyTo(pVal);
		if (hr != S_OK) logModule.Write(L"VariantCopy %x", hr);
	}
	if (FAILED(hr))
	{
		Error(L"get_Item failed", GetObjectCLSID(), hr);
	}
	return hr;
}

STDMETHODIMP CApplication::put_Value(BSTR key, VARIANT newVal) throw()
{
	auto hr = HasOnStartPageBeenCalled();
	if (FAILED(hr))
	{
		return hr;
	}
	if (key == nullptr || ::SysStringLen(key) == 0)
	{
		Error(L"Key may not be empty or null", this->GetObjectCLSID(), E_INVALIDARG);
		return E_INVALIDARG;
	}
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
		ElementModel v;
		v.IsNew = TRUE;
		
		//add it with an empty value and find it again
		logModule.Write(L"add key %s", key);

		_dictionary.insert(pair<CComBSTR, ElementModel>(key, v)); //element is copied by value, so a VariantCopy is done
		pos = _dictionary.find(key);
	}
	else if (pos->second.IsNew == FALSE)
	{
		pos->second.IsDirty = TRUE;//going to change
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

STDMETHODIMP CApplication::putref_Value(BSTR key, VARIANT newVal) throw()
{
	auto hr = HasOnStartPageBeenCalled();
	if (FAILED(hr))
	{
		return hr;
	}
	if (key == nullptr || ::SysStringLen(key) == 0)
	{
		Error(L"Key may not be empty or null", this->GetObjectCLSID(), E_INVALIDARG);
		return E_INVALIDARG;
	}
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
			ElementModel v;
			_dictionary.insert(pair<CComBSTR, ElementModel>(key, v));

			pos = _dictionary.find(key);
			pos->second.IsNew = TRUE;
		}
		else
		{
			pos->second.IsDirty = TRUE;//going to change
			pos->second.IsSerialized = FALSE; // do not to forget to undo, otherwise it stays 'serialized'
		}
		
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
		hr = E_INVALIDARG;
		Error(L"This variable is not an object", GetObjectCLSID(), hr);
		logModule.Write(L"putref_Item not VT_DISPATCH or VT_UNKNOWN but %d", origType);
	}
	return hr;
}
STDMETHODIMP CApplication::get_Key(INT Index, BSTR* pVal) throw()
{
	auto hr = HasOnStartPageBeenCalled();
	if (FAILED(hr))
	{
		return hr;
	}
	if (SUCCEEDED(hr))
	{
		if ((Index < 1) || (Index > _dictionary.size() + 1))
		{
			hr = E_INVALIDARG;
			Error(L"Key Index out of range, must be within 1 and .Count() value", this->GetObjectCLSID(), hr);
			return hr;
		}
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

STDMETHODIMP CApplication::get_Count(PINT pVal) throw()
{
	auto hr = HasOnStartPageBeenCalled();
	if (FAILED(hr))
	{
		return hr;
	}
	*pVal = (int)_dictionary.size();
	return hr;
}


STDMETHODIMP CApplication::get_Exists(BSTR Key, VARIANT_BOOL* pVal) throw()
{
	if (Key == nullptr || ::SysStringLen(Key) == 0)
	{
		Error(L"Key may not be empty or null", this->GetObjectCLSID(), E_INVALIDARG);
		return E_INVALIDARG;
	}
	auto pos = _dictionary.find(Key);
	*pVal = pos == _dictionary.end() ? VARIANT_FALSE : VARIANT_TRUE;
	return S_OK;
}

STDMETHODIMP CApplication::get_KeyType(BSTR Key, SHORT* pVal) throw()
{
	auto hr = HasOnStartPageBeenCalled();
	if (FAILED(hr))
	{
		return hr;
	}
	if (Key == nullptr || ::SysStringLen(Key) == 0)
	{
		Error(L"Key may not be empty or null", this->GetObjectCLSID(), E_INVALIDARG);
		return E_INVALIDARG;
	}
	auto pos = _dictionary.find(Key);
	*pVal = pos->second.val.vt;
	return hr;
}
STDMETHODIMP CApplication::_NewEnum(IUnknown **ppRet) throw()
{
	auto hr = HasOnStartPageBeenCalled();
	if (FAILED(hr))
	{
		return hr;
	}
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
STDMETHODIMP CApplication::Remove(BSTR Key) throw()
{
	auto hr = HasOnStartPageBeenCalled();
	if (FAILED(hr))
	{
		return hr;
	}
	if (Key == nullptr || ::SysStringLen(Key) == 0)
	{
		hr = E_INVALIDARG;
		Error(L"Key may not be empty or null", this->GetObjectCLSID(), hr);
		return hr;
	}
	auto pos = _dictionary.find(Key);
	if (pos != _dictionary.end())
	{
		_dictionary.erase(pos);
		//avoid duplicates the vector is not a unique dictionary
		CComBSTR ansiKey;
		ansiKey.Attach(Key);//steal
		auto k = ansiKey.ToString();
		ansiKey.Detach();//give back

		auto found = std::find_if(_removed.begin(), _removed.end(),
			[=](string  &l){ 
			return  l.compare(k) == 0; });
		if (found == _removed.end())
		{
			_removed.push_back(k);
			
		}
		logModule.Write(L"remove key %s", Key);		
	}
	
	return hr;
}
STDMETHODIMP CApplication::RemoveAll() throw()
{
	auto hr = HasOnStartPageBeenCalled();
	if (FAILED(hr))
	{
		return hr;
	}
	
	for (auto k = _dictionary.begin(); k != _dictionary.end(); )
	{
		// avoid duplicate entries in the _removed vector
		auto ansiKey = k->first.ToString();
		
		auto found = std::find_if(_removed.begin(), _removed.end(),
			[=](string  &l){
			return l.compare(ansiKey) == 0; });
		if (found == _removed.end())
		{
			_removed.push_back(ansiKey);
		}
		_dictionary.erase(k++);
	}
	return hr;
}

STDMETHODIMP  CApplication::LockKey(BSTR Key) throw()
{//will perform a PUT [Guid]:lock 1 command to redis 2.2+
	
	auto hr = HasOnStartPageBeenCalled();
	if (FAILED(hr))
	{
		return hr;
	}
	CComBSTR appkey(m_AppKey);
	auto  bytesAppKey = appkey.ToString();
	
	//https://github.com/jacket-code/redlock-cpp/blob/master/LockExample.cpp
	//dlm->Lock((PSTR)bytesAppKey, 100000, myLock);
	//CComBSTR appKey(m_AppKey);

	return hr;	
}

STDMETHODIMP CApplication::UnlockKey(BSTR key) throw()
{
	return E_NOTIMPL;
}

STDMETHODIMP CApplication::ExpireKeyAt(BSTR Key, INT ms) throw()
{
	auto hr = HasOnStartPageBeenCalled();
	if (FAILED(hr))
	{
		return hr;
	}
	auto pos = _dictionary.find(Key);
	if (pos != _dictionary.end())
	{
		pos->second.ExpireAt = ms;
		//avoid duplicates the vector is not a unique dictionary
		
		logModule.Write(L"remove key %s", Key);
	}
	return hr;
}




// Opens a DB Connection and initialises the Dictionary with the binary contents
STDMETHODIMP CApplication::InitializeDataSource(IServer* m_piServer) throw()
{
	PCWSTR location = L"InitializeDataSource";
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
		logModule.Write(L"logic %s, phys %s", configFile.m_str, retVal);
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
		logModule.Write(L"searched web.Config up to: (%s) none found %x", retVal.m_str, hr);
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
	}
	ConfigurationManager config(retVal);

	//dlm->AddServerUrl("", port); //TODO: get con string
	m_startSessionRequest = std::chrono::system_clock::now();
	CComBSTR bstrProp ( L"DataSource"), strConstruct;
	auto prefix = L"ispsession_io:";
	bstrProp.Insert(0, prefix);

	strConstruct = _wgetenv(bstrProp);
	
	if (strConstruct.IsEmpty()) // not in environment, then web.Config
	{
		strConstruct.Attach(config.AppSettings(bstrProp));
	}
	if (strConstruct.IsEmpty())
	{
		hr = HRESULT_FROM_WIN32(ERROR_CANTREAD);
		Error(L"Invalid web.Config datasource not found", this->GetObjectCLSID(), hr);
		return hr;
	}

	if (SUCCEEDED(hr))
	{
		auto success = CSessionDL::OpenDBConnection(std::wstring(strConstruct, strConstruct.Length()), pool);
		if (!success)
		{
			hr = E_FAIL;
		}
	}

	GUID license = { 0 };

	bstrProp = L"CacheLicense";
	bstrProp.Insert(0, prefix);
	bstrProp.Attach(config.AppSettings(bstrProp));
	if (!bstrProp.IsEmpty())
	{
		setstring((const PUCHAR)&license, bstrProp);
	}
	bstrProp = L"ClassicCsession.LIC";
	bstrProp.Insert(0, prefix);
	bstrProp.Attach(config.AppSettings(bstrProp));
	CComBSTR strLicensedFor;
	if (bstrProp.Length() != 0)
	{
		strLicensedFor.Attach(bstrProp.Detach());//move
	}

	m_licenseOK = LicentieCheck(&license, strLicensedFor);

#ifndef Demo	
	/*if (licenseOK == false)
	{
		hr = CLASS_E_NOTLICENSED;
		Error(L"No valid License Found", CLSID_NWCSession, hr);
		return hr;
	}*/
	
#endif

	bstrProp = L"APP_KEY";
	bstrProp.Insert(0, prefix);
	bstrProp.Attach(config.AppSettings(bstrProp));

	logModule.Write(L"AppKey: (%s)", bstrProp.m_str);

	if (setstring(reinterpret_cast<PUCHAR>(&m_AppKey), bstrProp) == FALSE)
	{
		hr = E_INVALIDARG;
		this->Error(L"APP_KEY missing", this->GetObjectCLSID(), hr);
	}

	if (FAILED(hr))
	{
		//ReportComError2(hr, location);
		m_bErrState = TRUE;
	}
	else
	{
		CComPtr<IKeySerializer> database = this;
		CApplicationDL applicationDl;
		hr = applicationDl.ApplicationGet(pool, m_AppKey, database);
	}
	
	
	return hr;
}
STDMETHODIMP CApplication::ReadString(std::istream& pStream, BSTR *retval) throw()
{
	HRESULT hr = S_OK;
	UINT lTempSize = 0;
	
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
			auto result = EnsureBuffer(lTempSize);
			
			if (hr == S_OK)
			{
				
				pStream.read((char*)m_lpstrMulti.m_pData, lTempSize);
				//because we specify the exact length, writtenbytes is excluding the terminating 0
				UINT writtenbytes = ::MultiByteToWideChar(CP_UTF8, 0, (PSTR) m_lpstrMulti.m_pData, lTempSize, nullptr, 0);
				if (writtenbytes == 0)
					hr = ATL::AtlHresultFromLastError();
				else
				{
					if (::SysReAllocStringLen(retval, nullptr, writtenbytes) != FALSE)
						::MultiByteToWideChar(CP_UTF8, 0, (PSTR)m_lpstrMulti.m_pData, lTempSize, *retval, writtenbytes);
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
STDMETHODIMP CApplication::ReadValue(std::istream& pStream, VARIANT* TheValue, VARTYPE vtype) throw()
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
		pStream.seekg(0, ios::end);
		auto streamlen = pStream.tellg();
		pStream.seekg(0, ios::beg);
		logModule.Write(L"array memsize %d els %d, istream size %d", lMemSize, lElements, streamlen);
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
		
		else if ((vtype == VARENUM::VT_VARIANT || vtype == VT_BSTR || vtype == VT_DECIMAL) && lElements > 0)
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
			logModule.set_TempLocation(0);
			for (;;)
			{
				if (rgIndices[dimPointer] <
					(LONG)psaBound[dimPointer].cElements + psaBound[dimPointer].lLbound)
				{
					switch (vtype)
					{
					case VT_DECIMAL:
					case VT_VARIANT:
						VARIANT * pVar;
						hr = ::SafeArrayPtrOfIndex(psa, rgIndices, (void**)&pVar);
						//logModule.Write(L"Indices %d,%d,%d", rgIndices[0], rgIndices[1], rgIndices[2]);
						if (FAILED(hr))
							logModule.Write(L"FATAL: SafeArrayPtrOfIndex failed %x", hr);

						if (hr == S_OK)
						{
							if (vtype == VT_VARIANT)
							{
								VARTYPE vt2;
								pStream.read((char*)&vt2, sizeof(VARTYPE));
								hr = ReadValue(pStream, pVar, vt2);
							}
							else if (vtype == VT_DECIMAL)
							{
								hr = ReadValue(pStream, pVar, VT_DECIMAL);
							}
						}
						break;
					case VT_BSTR:
						BSTR * pBstr;
						hr = ::SafeArrayPtrOfIndex(psa, rgIndices, (void**)&pBstr);
						hr = ReadString(pStream, pBstr);
						break;
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
STDMETHODIMP CApplication::WriteString(BSTR TheVal, IStream* pStream) throw()
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
			
			hr = EnsureBuffer(byteswritten);
			
		    test = ::WideCharToMultiByte(CP_UTF8, 0, TheVal, lTempSize + 1, (PSTR)m_lpstrMulti.m_pData, byteswritten, nullptr, nullptr);
			if (test > 0)
			{
				test--; //exclude terminating zero				
				pStream->Write(&test, sizeof(test), nullptr);
				pStream->Write(m_lpstrMulti.m_pData,  test, nullptr);
				logModule.Write(L"A: WriteString Bytes %d", test);
			}
		}
	}
	else //empty string
	{
		test = 0;
		pStream->Write((char*)&test, sizeof(UINT), nullptr);
	}
	return hr;
}
STDMETHODIMP CApplication::ConvertVStreamToObject(ElementModel &var) throw()
{
	HRESULT hr = S_OK;
	CComQIPtr<IStream> l_pIStr(var.val.punkVal);
	hr = l_pIStr->Seek(SEEK_NULL, STREAM_SEEK_SET, nullptr);
	var.val.punkVal->Release();//remove the IStream, it is not the object itself
	hr = OleLoadFromStream(l_pIStr, IID_IUnknown, (void**)&var.val.punkVal);
	bool doPersist2 = false;
	
	if (hr == E_NOINTERFACE) //IPersistStream not found!
	{
		doPersist2 = true;
		//set the ptr sizeof(CLSID) back because the GetClassId advanced 16 bytes in the stream
		LARGE_INTEGER dbMove;
		dbMove.QuadPart = -(long long)sizeof(CLSID);
		hr = l_pIStr->Seek(dbMove, STREAM_SEEK_CUR, nullptr);
		hr = OleLoadFromStream2(l_pIStr, IID_IUnknown, (void**)&var.val.punkVal);
	}
	logModule.Write(L"ConvertVStreamToObject%s %x", doPersist2 ? L"2" : L"", hr);
	var.IsSerialized= FALSE; //don't deserialize next time
	
	
	return hr;
}
STDMETHODIMP CApplication::ConvertObjectToStream( VARIANT &var) throw()
{
	if ((var.vt != VT_UNKNOWN && var.vt != VT_DISPATCH)|| var.punkVal == nullptr)
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
			var.vt = VT_UNKNOWN;
		}
	}
	return hr;
}


STDMETHODIMP CApplication::WriteValue(VARTYPE vtype, VARIANT& TheVal, IStream* pStream) throw()
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
		pStream->Write((char*)&descriptor, sizeof(ARRAY_DESCRIPTOR), nullptr);

		VARTYPE vcopy = vtype & ~VT_ARRAY;
		if (vcopy == VARENUM::VT_UNKNOWN || vcopy == VARENUM::VT_ERROR || vcopy == VARENUM::VT_VARIANT)
		{
			//because e.g. Redim v(1,1) TypeName(v) == "Variant()"
			CComBSTR bogusAssemblyTypeForDotNet(L"System.Object"); //
			WriteString(bogusAssemblyTypeForDotNet, pStream);
		}
		else //todo jagged array??
		{
			WriteString(nullptr, pStream);
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
			pStream->Write(bounds, sizeof(SAFEARRAYBOUND), nullptr);
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
					pStream->Write((char*)psadata, lMemSize, nullptr);
					::SafeArrayUnaccessData(psa);
				}
			}
		}
		else if ((vcopy == VT_BSTR) )
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
							hr = WriteString(myarray[els], pStream);
					}
					
					logModule.set_Logging(backup);
					logModule.Write(L"written VT_BSTR array length=%d %x", els, hr);
					::SafeArrayUnaccessData(psa);
				}
			}
		}
		//write a variant array of type VT_VARIANT		
		else if (vcopy == VARENUM::VT_VARIANT || vcopy == VT_DECIMAL)
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
						if (vtype == VT_VARIANT)
						{

							pStream->Write(&pVar->vt, sizeof(VARTYPE), nullptr);
							//----- recursive call ----- keep an eye on this
							
						}
						hr = WriteValue(vtype, *pVar, pStream);
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
				}
				::SafeArrayUnlock(psa);
				logModule.set_Logging(backup);
				logModule.Write(L"written VT_VARIANT array length=%d %x", findEl, hr);
			} // if not zero elements
		}
		else
		{
			if (psa == nullptr)
				//handle nullptr pointer array
				pStream->Write(0, sizeof(int), nullptr);
			else
				hr = E_INVALIDARG;
		}

	}
	else
	{
		// empty and null {0,1} need not be written
		//mask out using xor
		
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
			pStream->Write((char*)&TheVal, cBytes, nullptr);
			break;
		case VT_BSTR:

			hr = WriteString(TheVal.bstrVal, pStream);
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
				hr = pStream->Write(&cBytes, sizeof(pstatstg.cbSize.LowPart), nullptr);
				// copy the persisted object as bytestream to the main stream		
				l_pIStr->Seek(SEEK_NULL, STREAM_SEEK_SET, nullptr);
				hr = l_pIStr->CopyTo(pStream, pstatstg.cbSize, nullptr, nullptr);
				if (hr == E_NOTIMPL) //ISequentialStream does not support it, or just not impl.
				{
					ULARGE_INTEGER cb;
					cb.QuadPart = cBytes;
					hr = ISequentialStream_Copy(l_pIStr, pStream, cb, nullptr, nullptr);
				}
			    if (FAILED(hr))
				{
					logModule.Write(L"FATAL: cannot copy stream %x", hr);
				
					Error(L"While trying to serialize this object an error ocurred", this->GetObjectCLSID(), hr);
				}
			}
			else
			{
				// indicate zero bytes
				cBytes = 0;
				pStream->Write((char*)&cBytes, sizeof(DWORD), nullptr);
			}

			break;
		}
		default:
			hr = E_INVALIDARG;
		}
	exit: // sorry
		if (cBytes > 0 && vtype != VT_BSTR && vtype != VT_UNKNOWN && vtype != VT_DISPATCH && vtype != VT_DECIMAL)
			pStream->Write((char*)&TheVal.bVal, cBytes, nullptr);
		logModule.Write(L"Simple variant type =%d, size=%d", vtype, cBytes);
	}

	return hr;
}


/* IKeySerializer implementation*/
// trim from start

// todo: fire and forget ? Nice improvement
// binary data like this "$6\r\nfoobar\r\n" (without the quotes). The bytes need no encoding or escaping.
// however, all strings are binary safe so escaping it as 'binary' is not necessary
 STDMETHODIMP  CApplication::SerializeKeys(const std::vector<string> &keys, __in IKeySerializer* pDictionary, command& cmd, const string& appkeyPrefix) throw()
{

	string k(appkeyPrefix);
	auto prefixSize = appkeyPrefix.size();
	string baseString;
	CComObject<CStream>* cseqs;
	CComObject<CStream>::CreateInstance(&cseqs);
	CComPtr<IStream> stream;

	stream = cseqs;
	HRESULT hr = S_OK;
	CComBSTR bstrKey;
	LARGE_INTEGER set = { 0 };
	ULARGE_INTEGER newpos = { 0 };
	for (auto saddKey = 0; saddKey < keys.size(); ++saddKey)
	{
		ULARGE_INTEGER sze;
		sze.QuadPart = 128;
		stream->SetSize(sze);

		stream->Seek(set, STREAM_SEEK_SET, nullptr);


		bstrKey = keys[saddKey];
		hr = pDictionary->SerializeKey(bstrKey, stream);
		stream->Commit(STATFLAG_DEFAULT);//must be IStream thus 
		hr = stream->Seek(set, STREAM_SEEK_CUR, &newpos);
		auto cBytes = newpos.LowPart;
		logModule.Write(L"Serialize key %s %x len %d", bstrKey, hr, cBytes);
		BYTE buf[512] = { 0 };
		stream->SetSize(newpos);
		stream->Seek(set, STREAM_SEEK_SET, nullptr); //cut off to correct length
		ULONG read = 0;
		baseString.resize(0);
		while (hr == S_OK)
		{
			hr = stream->Read(buf, sizeof(buf), &read);
			if (read > 0)
			{
				baseString.append((char*)buf, read);
			}
		}
		if (SUCCEEDED(hr))
		{
			//only set redis keys to upper, not the serialized one
			k.resize(prefixSize);
			k.append(str_toupper(keys[saddKey]));
			cmd << k << baseString;
			hr = S_OK;
		}
	}
	return hr;

}
STDMETHODIMP CApplication::SerializeKey(BSTR Key, IStream* binaryString) throw()
{
	HRESULT hr = S_OK;
	binaryString->Seek(SEEK_NULL, STREAM_SEEK_SET, nullptr);
	auto pos = _dictionary.find(Key);
	if (pos != _dictionary.end())
	{
		std::string retVal;
		// write the complete string including the leading 4 bytes
	    hr = WriteString(Key, binaryString);
		//write variant type
		VARTYPE vtype = pos->second.val.vt;
		if (SUCCEEDED(hr))
		{			
			binaryString->Write((char*)&vtype, sizeof(VARTYPE), nullptr);
			if ((pos->second.val.vt == VT_UNKNOWN || pos->second.val.vt == VT_DISPATCH) && pos->second.IsSerialized == FALSE)
			{
				hr = ConvertObjectToStream(pos->second.val);
			}
			hr = WriteValue(vtype, pos->second.val, binaryString);
		}
		logModule.Write(L"WriteProperty propname=%s, type=%d, result=%x", Key, vtype, hr);
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
STDMETHODIMP CApplication::DeserializeKey(const std::string& binaryString) throw()
{
	HRESULT hr = S_OK;
	auto stream = std::stringstream(binaryString, ios_base::in || ios_base::binary);
	/*
	1. read the keyname
	2. read the VARTYPE (VarType)
	3. read the contents
	4. Set the key + value in the dictionary (maintaining original casing)
	*/
	//
	CComBSTR key;
	CComVariant val;
	VARTYPE vt = 0;
	hr = ReadString(stream, &key);

	if (SUCCEEDED(hr))
	{
		stream.read((char*)&vt, sizeof(VARTYPE));
		logModule.Write(L"Deserialized key %s, with type %d", key, vt);
		hr = ReadValue(stream, &val, vt);
		ElementModel m;
		_dictionary.insert(std::pair<CComBSTR, ElementModel>(key, m));
		auto pos = _dictionary.find(key);
		//pos->second.val = val;
		if (val.vt == VT_UNKNOWN || val.vt == VT_DISPATCH)
		{
			pos->second.IsSerialized = TRUE;
		}
		val.Detach(&pos->second.val);	
	}
	else
	{
		logModule.Write(L"DeserializeKey %d failed", hr);
	}
	return hr;
}

STDMETHODIMP CApplication::get_KeyStates(
	std::vector<string> &dirty_keys,
	std::vector<string> &new_keys,
	std::vector<string> &other_keys,
	std::vector<std::pair<string, INT>> &expire_keys,
	std::vector<string> &removed_keys) throw()
{
	//USES_CONVERSION;
	HRESULT hr = S_OK;
	dirty_keys.clear();
	new_keys.clear();
	other_keys.clear();
	expire_keys.clear();
	removed_keys.clear();
	for (auto k = _dictionary.begin(); k != _dictionary.end(); ++k)
	{
		auto ansi = (*k).first.ToString();
		
		if (ansi.empty())
		{
			logModule.Write(L"Severe error, empty key found");
			continue;
		}				
		
		if (k->second.IsNew == TRUE)
		{
			new_keys.push_back(ansi);
		}
		else if (k->second.IsDirty == TRUE)
		{
			dirty_keys.push_back(ansi);
		}
		else
		{
			other_keys.push_back(ansi);
		}
		if (k->second.ExpireAt > 0)
		{
			expire_keys.push_back(std::pair<string, INT>(ansi, k->second.ExpireAt));
		}
	}
	auto sz = _removed.size();
	for (auto x = 0; x < sz; ++x)
	{
		removed_keys.push_back(_removed[x]);
	}
	return hr;
}

/* END IKeySerializer*/

STDMETHODIMP CApplication::EnsureBuffer(INT newBuffer) throw()
{
	if (newBuffer > m_currentBufLen)
	{
		m_currentBufLen = (newBuffer / 512) * 512 + 1024;
		auto result = m_lpstrMulti.ReallocateBytes(m_currentBufLen);
		if (result == false)
		{
			return E_OUTOFMEMORY;
		}
	}

	return S_OK;
}