#include "stdafx.h"
#include "tools.h"
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include "config.h"
#include "ADCLogModule.h"

#pragma comment(lib, "xmllite.lib")

//#include <atlfile.h>
ConfigurationManager::ConfigurationManager(const BSTR configFile) throw()
{
	logModule.Write(L"Config %s", configFile);
	_szFilePath = configFile;
	time(&_ftLastCheck);
	Init();
}
ConfigurationManager::ConfigurationManager() throw()
{	
	time(&_ftLastCheck);
	_szFilePath.Attach(GetModulePath());	
	if (!_szFilePath.IsEmpty())
	{		
		_szFilePath.Append(L".config");
		logModule.Write(L"Config %s", _szFilePath);		
		Init();
	}	
}
void ConfigurationManager::Init() throw()
{
	if (!_szFilePath.IsEmpty())
	{
		HRESULT hr = CoGetMalloc(1, &_malloc);
		hr = CreateXmlReader(IID_IXmlReader, (void**)&_xmlReader, _malloc);
		if (FAILED(hr))
		{
			logModule.Write(L"XmlReader %x", hr);
		}
	}
}
time_t ConfigurationManager::GetFileTime() throw()
{	
	struct ::stat stResult;
	CComBSTR ansi(_szFilePath);
	ansi.Attach(ansi.ToByteString());
	
	auto result=	stat((char*)ansi.m_str, &stResult);		// get the attributes of afile.txt
	if (result != 0)
	{
		return 0;
	}	
	//tm* clock = gmtime(&stResult.st_mtime);	// Get the last modified time and put it into the time structure 
	return stResult.st_mtime;
}
BSTR ConfigurationManager::AppSettings(const BSTR key, PCWSTR defaultValue) throw()
{
	
	HRESULT hr = CheckTimeOut();
	if (FAILED(hr)) 
	{
		logModule.Write(L"Cannot parse config file because of (%x)", hr);
		return NULL;
	}
	CComBSTR find;
	find.Attach(key);
	
	auto found = _map.find(find);
	find.Detach();
	//ATLTRACE(L"AppSettings findKey found %d %s\r\n", found, key);
	//note: GetValueAt returns by reference, and does not Copy()	
	if (found != _map.end()) 
	{
		return found->second.Copy();
	}
	else if (defaultValue != NULL)
	{
		return ::SysAllocString(defaultValue);
	}
	return NULL;
	
}
ConfigurationManager::~ConfigurationManager() throw()
{
	_map.clear();
	_xmlReader.Release();
	_malloc.Release();
	_szFilePath.Empty();
}
HRESULT ConfigurationManager::CheckTimeOut() throw()
{
	
	auto curT = GetFileTime();
	if (curT == 0)
	{
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
	}
	PCWSTR pwzValue;
	auto memResult = ::difftime(curT, _ftLastCheck);
	if (memResult != 0.0F)
	{
		DWORD start = ::GetTickCount();
	
		HRESULT hr = S_OK;
		
		CComPtr<IStream> pStream;
		CComPtr<IXmlReaderInput> _readerInput;
		
		hr = ::SHCreateStreamOnFileEx(_szFilePath, STGM_READ | STGM_SHARE_DENY_NONE, FILE_ATTRIBUTE_NORMAL, FALSE,NULL, &pStream);

		if (SUCCEEDED(hr))
		{
			hr = ::CreateXmlReaderInputWithEncodingCodePage(pStream, _malloc, CP_UTF8, TRUE, NULL, &_readerInput);			
			hr = _xmlReader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit);	
			hr = _xmlReader->SetInput(_readerInput);
		}	
		else
		{
			return hr;
		}
		
		XmlNodeType nodeType = XmlNodeType::XmlNodeType_None;
		UINT lenValue;
		PCWSTR key;
		bool startCollecting  = false;
		while (S_OK == _xmlReader->Read(&nodeType) && hr == S_OK)
		{
			switch(nodeType) {
			case XmlNodeType::XmlNodeType_EndElement:
				
				//hr = pReader->GetDepth(&dept);
				hr = _xmlReader->GetLocalName(&pwzValue, NULL);
				if (startCollecting && _wcsicmp(pwzValue, L"appSettings") == 0)
				{
					//break loop
					hr = S_FALSE;
				}
				break;
			case XmlNodeType::XmlNodeType_Element:
				{
					// get element name such as option in <option value="11">
					hr = _xmlReader->GetLocalName(&pwzValue, NULL);
			
					if (FAILED(hr)) break;
					
					//iOrdinalCount++;
					if (startCollecting == false && _wcsicmp(pwzValue, L"appSettings") == 0)
					{
						startCollecting = true;

						hr = _xmlReader->MoveToAttributeByName(L"configSource", NULL);
						if (hr == S_OK)
						{
							hr = _xmlReader->GetValue(&pwzValue, NULL);
							
							logModule.Write(L"found configSource %s", pwzValue);								
							if (::PathIsRelativeW(pwzValue) == TRUE)
							{
								//TODO: call back to do a Server.MapPath
								_szFilePath.Attach(FileStripFile(_szFilePath));								
								_szFilePath.Append(L'\\');
								_szFilePath.Append(pwzValue);
							}
							else
							{
								_szFilePath = pwzValue;
							}
							_readerInput.Release();
							pStream.Release();
							return CheckTimeOut(); //recursion							
						}
						hr = S_OK;//reset otherwise loop stops
					}					
					else if (startCollecting && _wcsicmp(pwzValue, L"add") == 0)
					{
						
						hr = _xmlReader->MoveToAttributeByName(L"key", NULL);
						if (hr == S_OK)
						{
							hr = _xmlReader->GetValue(&pwzValue, &lenValue);
							//key.Append( pwzValue, lenValue);
							key = pwzValue;

							//ATLTRACE(L"found key %s %d\r\n", pwzValue, lenValue);
							hr = _xmlReader->MoveToAttributeByName(L"value", NULL);
							if (hr == S_OK)
							{
								_xmlReader->GetValue(&pwzValue, NULL);
								_map.insert(std::pair<CComBSTR, CComBSTR>(key, pwzValue));
							}
						}
					}					
				}
				break;
			}
		}
		if (SUCCEEDED(hr)) _ftLastCheck = curT;
		if (_xmlReader != NULL)
		{
			_xmlReader->SetInput(NULL);
		}
		logModule.Write(L"Parsing config... took %dms", ::GetTickCount() - start);
		//fs->Release();
		return S_FALSE;
		
	}

	return S_OK;
	
};

