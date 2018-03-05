#include "stdafx.h" 
#include "adclogmodule.h" 
#include "tools.h"

// Copyright 2006 ADC Cure
// this code is no free public source.

LoggingModule::LoggingModule(void) throw() : 
	m_LoggingEnabled(0)	
{
}
void LoggingModule::set_TempLocation(int location) throw()
{
	m_tempLocation = location;
}
//accepts a folder or filename and looks into if it has Append rights
bool LoggingModule::HasWriteAccess(PCWSTR fileToCheck, ACCESS_MASK mask = (FILE_APPEND_DATA | SYNCHRONIZE)) throw() //write includes append and create
{
	ATL::CSecurityDesc s;
	auto result = AtlGetSecurityDescriptor(fileToCheck, SE_OBJECT_TYPE::SE_FILE_OBJECT, &s, DACL_SECURITY_INFORMATION, true);
	if (result == false)
	{
		return false;
	}
	CDacl pDacl;
	s.GetDacl(&pDacl);
	/*DWORD maxSid = SECURITY_MAX_SID_SIZE;
	PSID buf = ::LocalAlloc(LMEM_FIXED, maxSid);
	
	
	BOOL resultSid = ::CreateWellKnownSid(WELL_KNOWN_SID_TYPE::WinBuiltinIUsersSid, NULL, buf, &maxSid);
	PWSTR stringSid = NULL;
	resultSid = ::ConvertSidToStringSidW(buf, &stringSid);*/
	
	//::LocalFree(buf);		
	TRUSTEEW trustee = { nullptr, NO_MULTIPLE_TRUSTEE,
		TRUSTEE_FORM::TRUSTEE_IS_NAME,
		TRUSTEE_TYPE::TRUSTEE_IS_USER,  L"CURRENT_USER"}; //stringSid
	

	ACCESS_MASK accessRights;
	DWORD checkRights = ::GetEffectiveRightsFromAclW((PACL)pDacl.GetPACL(), &trustee, &accessRights);
	//::LocalFree(stringSid);
	if (checkRights != ERROR_SUCCESS)
	{	
		return false;
	}
	
	return (accessRights & mask) == mask;
}
void LoggingModule::set_Logging(int enable) throw()
{ 
	m_LoggingEnabled = enable;
	
	if ((m_LoggingEnabled & 1) == 1)
	{	
		//find out if we can log in the current directory or otherwise have to use \%SystemRoot%\Temp
		CComBSTR path;
		m_logFileName.Attach(GetModulePath());
		path.Attach(FileStripFile(m_logFileName));
		m_logFileName.Attach(FileStripPath(m_logFileName));	
		if (m_logFileName.IsEmpty())
		{
			OutputDebugStringW(L"Cannot log, LogFileName invalid");
		}
		m_logFileName.Append(L".LOG");
		if (!HasWriteAccess(path))//check path, because a file that does not exist, has no DACL, duh
		{
			//strip right length
			m_logFileName.SetLength((unsigned int)wcslen(m_logFileName));
			//	m_MutexName.Format(L"Global\\%s", m_logFileName.m_str);
			CComBSTR buf(MAX_PATH);

			UINT bufLen = m_tempLocation == 0 ? ::GetSystemWindowsDirectoryW(buf, MAX_PATH) : ::GetTempPathW(MAX_PATH, buf);

			if (bufLen == 0)
			{
				buf.Format(L"error %d\n", GetLastError());
				OutputDebugStringW(buf);
				m_logFileName.Empty();
				m_LoggingEnabled ^= 1;
				return;
			}
			else
			{
				//strip right length again
				buf.SetLength(bufLen);
				buf += L"\\TEMP\\";
				buf += m_logFileName;
				// now m_LogFileName looks like C:\Windows\Temp\myDll.Log
				m_logFileName.Attach(buf.Detach());
			}
		}
		//it's ok to log in Current Directory
		else
		{
			path.Append(L"\\");
			m_logFileName.Insert(0, path);			
		}

		//Create EveryOne in a language independent way
		//SID_IDENTIFIER_AUTHORITY sia = { SECURITY_WORLD_SID_AUTHORITY };
		//CSid everyone(sia, 1, SECURITY_WORLD_RID);

		//// we must use a Security Descriptor, otherwise other w3wp.exe processes cannot
		//// inherit the mutex!
		//CSecurityDescriptor secDescr;
		//// ATL does not initialize this object at construction??
		//HRESULT hr = secDescr.Initialize();
		//if (hr == S_OK) hr = secDescr.Allow((PSID)everyone.GetPSID(), SYNCHRONIZE);
		//if (FAILED(hr))
		//{
		//	buf.Format(L"secDescr.Allow failed with %x\n", hr);
		//	OutputDebugStringW(buf);
		//	m_LoggingEnabled ^= 1;
		//	return;
		//}
		//SECURITY_ATTRIBUTES SecAttrs = { sizeof(SecAttrs), secDescr, TRUE };
		// create the mutex without owning it.
		// if another process (w3wp) already did the creation
		// we just ignore the error and m_hMutex stays nill
//		m_hMutex.Create(&SecAttrs, FALSE, m_MutexName);
		OpenFile();
	}
	else
		Close();
}
int LoggingModule::get_Logging() throw()
{
	return m_LoggingEnabled;
}
LoggingModule::~LoggingModule(void) throw()
{
	this->Close();
}
///<sumarray> releases resources</summary>
void LoggingModule::Close(bool deleteLogFile) throw()
{
	m_file.Close();
//	m_hMutex.Close();

	if (deleteLogFile && !m_logFileName.IsEmpty())
		_wremove(m_logFileName);
	m_logFileName.Empty();
}

// this function writes the debugging information to the logfile
// it synchronizes on a mutex so all processes go through this bottleneck
// and will 'fight' for file access
// be sure not to enable such a thing in production
void LoggingModule::Write(PCWSTR pszFormat, ...) throw()
{
	bool noFileAccess = false;
	CComBSTR m_fmt;
//	CMutex mutResult;
	if (m_LoggingEnabled == 0)
		return;
	else if ((m_LoggingEnabled & 1) == 1)
	{
		
		//BOOL result = mutResult.Open(SYNCHRONIZE, FALSE, m_MutexName);
		//if (result == TRUE)
		//{			
		//	DWORD dwWaitResult = ::WaitForSingleObject( 
		//			mutResult,   // handle to mutex
		//			10L);   // 10 ms second
	 //
		//	switch (dwWaitResult) 
		//	{
		//		// The thread got mutex ownership.
		//	case WAIT_OBJECT_0: break; //ok
		//	// Cannot get mutex ownership due to time-out.
		//	case WAIT_TIMEOUT:
		//	// Got ownership of the abandoned mutex object.
		//	case WAIT_ABANDONED: 
		//		OutputDebugStringW(L"error on LoggingModule::Write OpenMutex\n");
		//		noFileAccess = true;
		//		mutResult.Release();
		//		mutResult.Close();
		//		break;
		//	}
		//}
		//else
		//{
		///*	m_fmt.Format(L"coult not create mutex to %s err(%d)\n", m_logFileName, GetLastError());
		//	OutputDebugStringW(m_fmt);*/
		//	
		//	noFileAccess = true;
		//}
	}
	CComBSTR m_bstrTrace;
	va_list ptr;
	va_start(ptr, pszFormat);		
	m_bstrTrace.Format(pszFormat, ptr);
	va_end(ptr);
	if ((m_LoggingEnabled & 4) == 4 && !m_bstrTrace.StartsWith(L"db:") )
	{
		return;
	}
	SYSTEMTIME st ;
	GetLocalTime(&st);
	//prepend debugstring with thread and time.
	// eg 1040{tab}10:40:50:0599{tab}msg
	m_fmt.Format(L"%d\t%02d:%02d:%02d:%04d\t", GetCurrentThreadId(),
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	bool appendCrLf = !m_bstrTrace.EndsWith(L"\n");
	m_bstrTrace.Insert(0, m_fmt);

	if ((m_LoggingEnabled & 2) == 2)
	{
		if (appendCrLf)			
			m_bstrTrace.Append(L'\n');
		OutputDebugStringW(m_bstrTrace);
		
	}
	// write only to the file if we have access
	if (m_file != NULL && !noFileAccess && (m_LoggingEnabled & 1) == 1)
	{
		ULONGLONG nCurLen;
		m_file.GetPosition(nCurLen);
		CComBSTR ansi;

		if (nCurLen > 1024  * 1024 * 1024)
		{
			m_file.Close();
			std::locale utf8_locale(std::locale(), new std::codecvt_utf8<wchar_t>);
			std::wifstream file(m_logFileName, ios_base::binary);
			file.imbue(utf8_locale);			
			std::vector<std::wstring> linev;

			wchar_t buffer[10000] = { 0 };
			while (file.good())
			{
				file.getline(buffer, 10000);
				linev.push_back(buffer);
			}
			file.close();
			try
			{
				//recreate file
				std::wofstream file2(m_logFileName, ios_base::out);

				file2.imbue(utf8_locale);

				std::vector<std::wstring>::size_type sz = linev.size();
				sz--;
				for (std::vector<std::wstring>::size_type i = sz / 2; i <= sz; i++)
				{
					file2 << linev[i];
				}
				file2.close();
			}
			catch(std::exception ex)
			{ 
				//ignore, maybe next log it will succeed
			}
			OpenFile();	
		}
		
		if (appendCrLf)
			m_bstrTrace.Append(L"\r\n", 2);
		
		ansi.Attach((m_bstrTrace.ToByteString()));
		m_file.Write(ansi.m_str, ansi.ByteLength());
		//clear buffer so we can be sure the buffer advances
	//	m_file.Flush();
		//if (mutResult != NULL)
		//{
		//	//release ownership
		//	mutResult.Release();			
		//}
	}	
	
	}
	// opens file for append and prepends a BOM if needed
	bool LoggingModule::OpenFile() throw()
	{
		if (m_file == nullptr && !m_logFileName.IsEmpty())
		{
			HRESULT hr = m_file.Create(m_logFileName,
				FILE_APPEND_DATA | SYNCHRONIZE,
				FILE_SHARE_READ,
				OPEN_ALWAYS);
		
			if (SUCCEEDED(hr))
			{
				ULONGLONG sz;
				if (SUCCEEDED(m_file.GetSize(sz)))
				{
					//it's a unicode-8 logfile
					if (sz == 0)
					{
						BYTE PreAmble[] = { 0xEF, 0xBB, 0xBF };
						m_file.Write(PreAmble, sizeof(PreAmble));
					}
					//m_file.Seek(sz);
				}
				return true;
			}
			else
			{
				CComBSTR buf;
				buf.Format(L"Could not create %s, error %x\n", m_logFileName, hr);
				OutputDebugStringW(buf);
				// do not bother to write to the logfile
				m_LoggingEnabled ^= 1;			
			}
		}
		return false;
}
LoggingModule logModule;