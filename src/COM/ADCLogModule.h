#pragma once
// Copyright 2006 ADC Cure
// this code is no free public source.
#include <atlfile.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
//#include <atlsync.h>
#include <locale>

using namespace std;
class LoggingModule
{
public:
	LoggingModule() noexcept;
	~LoggingModule() noexcept;
	//releases resources
	void Close(bool deleteLogFile = false) noexcept;
	void Write(PCWSTR pszFormat, ...) noexcept;
	void set_Logging(int enable) noexcept;
	// 0 = %windir%\Temp, 1=%userprofile%\Local Settings\Temp
	void set_TempLocation(int location) noexcept;
	int get_Logging() noexcept;
	bool OpenFile() noexcept;

private:
	//not wfstream because it has problems opening a shared file
	bool HasWriteAccess(PCWSTR fileToCheck, ACCESS_MASK mask) noexcept;
	CAtlFile m_file;
	//0 disabled 1 = to file, 2 = to Debug 3 = both
	int m_LoggingEnabled; 
	int m_tempLocation;
	//CMutex m_hMutex;	
	CComBSTR m_MutexName ;	
	CComBSTR m_logFileName;	
};
