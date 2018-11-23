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
#include <codecvt>
using namespace std;
class LoggingModule
{
public:
	LoggingModule() ;
	~LoggingModule();
	//releases resources
	void Close(bool deleteLogFile = false);
	void Write(PCWSTR pszFormat, ...);
	void set_Logging(int enable);
	// 0 = %windir%\Temp, 1=%userprofile%\Local Settings\Temp
	void set_TempLocation(int location);
	int get_Logging();
	bool OpenFile();

private:
	//not wfstream because it has problems opening a shared file
	bool HasWriteAccess(PCWSTR fileToCheck, ACCESS_MASK mask);
	CAtlFile m_file;
	//0 disabled 1 = to file, 2 = to Debug 3 = both
	int m_LoggingEnabled; 
	int m_tempLocation;
	//CMutex m_hMutex;	
	CComBSTR m_MutexName ;	
	CComBSTR m_logFileName;	
};
