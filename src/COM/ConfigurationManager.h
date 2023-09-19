#pragma once
#include <xmllite.h>
#include <map>


using namespace ATL;

class ConfigurationManager
{
private:
	static const int DELAYTICKS = 1000;
	std::map<wstring, wstring> _map;
	time_t _ftLastCheck;
	CComPtr<IXmlReader> _xmlReader;
	CComPtr<IMalloc> _malloc;
	HRESULT CheckTimeOut() noexcept;
	//ansi version!
	CComBSTR _szFilePath;
	
	void Init() noexcept;

public:
	ConfigurationManager() noexcept;
	ConfigurationManager(const BSTR configFile) noexcept;
	std::wstring AppSettings(const std::wstring& key, PCWSTR defaultValue = nullptr) noexcept;
	time_t GetFileTime() noexcept;
	~ConfigurationManager() noexcept;
};