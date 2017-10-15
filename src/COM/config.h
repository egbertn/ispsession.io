#pragma once
#include <xmllite.h>
#include <map>


using namespace ATL;

class ConfigurationManager
{
private:
	static const int DELAYTICKS = 1000;
	std::map<CComBSTR, CComBSTR> _map;
	time_t _ftLastCheck;
	CComPtr<IXmlReader> _xmlReader;
	CComPtr<IMalloc> _malloc;
	HRESULT CheckTimeOut();
	//ansi version!
	CComBSTR _szFilePath;
	
	void Init();

public:
	ConfigurationManager();
	ConfigurationManager(const BSTR configFile);
	std::wstring& AppSettings(const std::wstring key, PCWSTR defaultValue = NULL);
	BSTR AppSettings(const BSTR key, PCWSTR defaultValue = NULL);
	time_t GetFileTime();
	~ConfigurationManager();
};