// CSessionDL.h : Declarations of the data layer for ISP Session 5.0
// may 3, 2004 <- first version. At that time, still in Visual Sourcesafe
// updated January 13, 2009 varbinar(max) optimization
// updated January 11 2016 to use Redis 2.6.12 or higher
#pragma once
#include "tools.h"
#include "CSession.h"
#include "CStream.h"


//initializes from SystemTime
struct DBTIMESTAMP 
{
	DBTIMESTAMP()
	{
		SYSTEMTIME st;
		::GetSystemTime(&st);
		year = st.wYear;
		month = st.wMonth;
		day = st.wDay;
		hour = st.wHour;
		minute = st.wMinute;
		second = st.wSecond;
		fraction = st.wMilliseconds;

	}
	SHORT  year;
	USHORT month;
	USHORT day;
	USHORT hour;
	USHORT minute;
	USHORT second;
	ULONG  fraction;
};
struct PersistMetaDataApp
{
public:
	PersistMetaDataApp() :
		sizeofMeta(sizeof(PersistMetaDataApp)),
		m_Expires(525600)	

	{
		memset(&m_LastUpdated, sizeof(DBTIMESTAMP), 0);
	}
	LONG sizeofMeta; //4
	DBTIMESTAMP m_LastUpdated; //20
	LONG m_Expires; //525600 = one year in minutes

};
struct PersistMetaData
{
public:
	PersistMetaData() :
		sizeofMeta(sizeof(PersistMetaData)),//also versions this stuff
		m_Expires(20),
		m_ReEntrance(VARIANT_FALSE),
		m_Liquid(VARIANT_FALSE),
		m_zLen(0)

	{
	}
	LONG sizeofMeta; //4
	DBTIMESTAMP m_LastUpdated; //20
	LONG m_Expires; //24
	VARIANT_BOOL m_ReEntrance; //26
	VARIANT_BOOL m_Liquid; //28
	LONG m_zLen; //32

};
// trim from start
//todo: fire and forget? Nice improvement
// binary data like this "$6\r\nfoobar\r\n" (without the quotes). The bytes need no encoding or escaping.
// however, all strings are binary safe so escaping it as 'binary' is not necessary


class _CpSessionStatsAccessor 
{
public:
	LONG m_SessionCount;	
	LONG m_AvgStateLength;
	LONG m_MaxStateLength;
	LONG m_MinStateLength;
	FLOAT m_AvgCompressionRate;
	LONG m_AvgSessionDuration;
	LONG m_CountExpired;
	
	LONG m_AverageRequestTime;
	LONG m_totalRequestTime;
	LONG m_MaxRequestTime;
	LONG m_TotalRequests;
};


class CpSessionStats :
	public _CpSessionStatsAccessor
{
public:

	HRESULT __stdcall OpenRowset(const simple_pool::ptr_t &pool, const PBYTE  m_AppKeyState, const PBYTE sessionID) throw()
	{
		HRESULT hr = S_OK;
		//TODO: specify specific table for session which contains stats
		//APP_KEY
		//SessionCount
		//AvgStateLength
		//MinStateLength
		//MaxStateLength
		//AvgCompressionRate
		//AvgSessionDuration
		//CountExpired
        return hr;
    }
};
class _ApplicationAccessor
{
public:
	_ApplicationAccessor() :
		IsNULL(FALSE),
		m_blobLength(0),		
		m_pStream(NULL)
	{
		
		memset(m_timest, 0, sizeof(DBTIMESTAMP));
	}

	BOOL IsNULL;
	LONG m_blobLength;
	BYTE m_timest[8];
	DBTIMESTAMP m_LastUpdated;
	CComObject<CStream>  *m_pStream;
	

};
class CApplicationDL:
	public _ApplicationAccessor
{
public:
	static HRESULT __stdcall ApplicationSave(const simple_pool::ptr_t &pool,
		PUCHAR appKey,
		IStream * pStream,
		LONG Expires,
		PBYTE previousLastUpdated, //timestamp 8 BYTES never zero!,
		//TODO: totalRequestTime must be added in an unsorted list in REDIS this can be used as statistics array
		LONG totalRequestTime
		)
	{
		const UINT bufLen = 0x1000;
		unsigned char bytes[bufLen];
		ULONG read2 = 0;
		ULONG didRead = 0;
		if (pStream == nullptr || appKey == nullptr)
		{
			return E_POINTER;
		}
		
		STATSTG statst = { 0 };
		pStream->Stat(&statst, STATFLAG_NONAME);
		auto read = statst.cbSize.LowPart;
		auto appkey = HexStringFromMemory((PBYTE)appKey, sizeof(GUID));
		
		std::string ansi;
		ansi.reserve(sizeof(GUID) * 2 + 1);
		ansi.append(appkey);
	

		std::string buf;
		PersistMetaDataApp meta;
	
		buf.append((PCSTR)&meta, meta.sizeofMeta);
		//write binary safe string
		HRESULT hr = S_OK;
		do
		{
			hr = pStream->Read(bytes, bufLen, &read2);
			didRead += read2;
			if (read2 > 0)
			{
				buf.append((PCSTR)bytes, (size_t)read2);
			}
		} while (hr == S_OK && didRead < read);
		hr = S_OK; //reset S_FALSE to S_OK
		auto conn = pool->get();
		auto reply = conn->run(command("SET")(ansi)(buf)("EX")(Expires * 60));

		pool->put(conn);
		//std::wstring wstr = s2ws(ansi); //TODO: must all be ansi
		logModule.Write(L"insert %s", std::wstring(ansi.begin(), ansi.end()).c_str());

	   hr = reply.type() == reply::type_t::STATUS && reply.str() == "OK" ? S_OK : E_FAIL;
		return hr;
	}

    HRESULT __stdcall ApplicationGet(const simple_pool::ptr_t &pool, const PUCHAR appKey )
	{
		auto appkey = HexStringFromMemory(appKey, sizeof(GUID));		

		std::string ansi;
		ansi.reserve(sizeof(GUID) * 2 + 1);
		ansi.append(appkey);

		auto conn = pool->get();
		if (conn == redis3m::connection::ptr_t()) // authentication happens DURING pool-get
		{
			return E_ACCESSDENIED;
		}
		auto repl = conn->run(command("GET")(ansi));


		auto result = S_OK;
		switch (repl.type())
		{
		case reply::type_t::NIL:
			result = S_FALSE;
			m_blobLength = 0;
			IsNULL = TRUE;
			break;
		case reply::type_t::STATUS:
		case reply::type_t::_ERROR:
			m_blobLength = 0;
			logModule.Write(L"ApplicationGet failed because %s", std::wstring(repl.str().begin(), repl.str().end()).c_str());
			result = E_FAIL;
			break;
		case reply::type_t::STRING:

			int buf = 0x1000;
			std::string str = repl.str();
			m_blobLength = repl.strlen();
			PersistMetaData meta;
			//against all sanity. But justified, we do not modify the string, just read from it
			memcpy(&meta, (void*)str.data(), meta.sizeofMeta);
			m_LastUpdated = meta.m_LastUpdated;
			
			int baseX = meta.sizeofMeta;
		
			IsNULL = m_blobLength == meta.sizeofMeta ? TRUE : FALSE;
			if (IsNULL == FALSE)
			{
				CComObject<CStream>::CreateInstance(&m_pStream);
				m_pStream->AddRef();
				//start blobLength = 128
				for (int x = m_blobLength - baseX; x > 0; x -= buf)
				{	//if 128 > 0x1000 ? buf : m_blobLength = 128
					ULONG BytesToWrite = (x > buf ? buf : x);
					m_pStream->Write((void*)&str.data()[baseX], BytesToWrite, nullptr);
					baseX += BytesToWrite;
				}
				LARGE_INTEGER nl = { 0 };
				m_pStream->Seek(nl, STREAM_SEEK::STREAM_SEEK_SET, nullptr);
			}
			break;
		}
		pool->put(conn);
		return result;
	}	
	//destructor
	~CApplicationDL()
	{
		if (m_pStream != nullptr)
		{
			m_pStream->Release();
			m_pStream = nullptr;
		}
	}
};

class  CSessionDL
{
public:
	// splits  e.g. "mycache.redis.cache.windows.net,abortConnect=false,ssl=true,password=..."
	// and performs 
	static bool __stdcall OpenDBConnection(const std::wstring constructor, simple_pool::ptr_t& retVal)
	{
		logModule.Write(L"Connection string %s", constructor.c_str());
		std::vector<std::wstring> arr;
		split(constructor, L',', arr);
		const auto arrSize = arr.size();
		std::string pw;
		std::wstring database;
		int databaseNo = 0;
		auto passwordPos = std::find_if(arr.begin(), arr.end(), [](const std::wstring& str) { 
			return str.find(L"password") == 0; }
		);
		std::vector<std::wstring> arr2;
		if (passwordPos != arr.end())
		{
			split(*passwordPos, '=', arr2, 1);
			trim(arr2.at(1));
			pw.assign(arr2.at(1).begin(), arr2.at(1).end());
			arr2.clear();
		}
		
		auto databasePos = std::find_if(arr.begin(), arr.end(), [](const std::wstring& str) { 
			return str.find(L"database") == 0 || str.find(L"defaultDatabase") == 0; }
		);
		if (databasePos != arr.end())
		{
			split(*databasePos, '=', arr2);
			trim(arr2.at(1));			
			databaseNo = std::stoi(arr2.at(1));
		}
		if (arrSize > 0)
		{
			std::vector<std::wstring> portandhost;
			split(arr.at(0), ':', portandhost);
			std::wstring host;
			std::wstring port;
			UINT portParsed = 6379U;
			host = portandhost.at(0);
			if (portandhost.size() > 1)
			{
				port = portandhost.at(1);
				portParsed = std::stoi(port);
			}
			for (int tryCount = 0; tryCount < 2; tryCount++)
			{
				try
				{
					//convert wstring to string by using begin() and end(), pointing at char*
					retVal = simple_pool::create(std::string(host.begin(), host.end()), portParsed, pw);
					if (databaseNo > 0)
					{
						logModule.Write(L"Set Database to %d", databaseNo);
						retVal->set_database(databaseNo);
					}
					return true;
				}
				catch (too_much_retries ex)
				{
					auto what = std::string(ex.what());
					logModule.Write(L"Retrying to connect because of %s", std::wstring(what.begin(), what.end()));
					Sleep(200);
				}
			}

		}
		for (unsigned int x = 0; x < arrSize; x++)
		{

		}
		return false;
	}

	static HRESULT __stdcall SessionSave(const simple_pool::ptr_t &pool,
			PUCHAR appKey,
			PUCHAR guid,
			//LONG LCIDPar,
			LONG ExpiresPar,
			BOOL ReEntrancePar,
			BOOL LiquidPar,
			GUID* guidNewPar,
			IStream * pStream,
			LONG zLenPar,
			PBYTE previousLastUpdated, //timestamp 8 BYTES never zero!,
			//TODO: totalRequestTime must be added in an unsorted list in REDIS this can be used as statistics array
			LONG totalRequestTime
			)
    {	
		auto hr = S_OK;
		auto sAppkey = HexStringFromMemory(appKey, sizeof(GUID));

		auto sGuid = HexStringFromMemory(guid, sizeof(GUID));
		std::string ansi;
		ansi.reserve(sizeof(GUID) * 2 + 1);
		ansi.append(sAppkey);
		ansi.append(":");
		ansi.append(sGuid);


		auto c = pool->get();
		//newguid? Then rename the key
		if (guidNewPar != nullptr)
		{
			//old			
			auto newKey = HexStringFromMemory((PUCHAR)guidNewPar, sizeof(GUID));
			std::string newKeyAnsi;
			newKey.reserve(sizeof(GUID) * 2 + 1);
			newKey.append(sAppkey);
			newKey.append(":");
			newKey.append(newKey);		
			
			auto reply = c->run(command("RENAME")(ansi)(newKey)); // http://www.redis.io/commands/rename
			//re-use
			ansi.assign(newKey);
		}
		// in fact, is not dirty, set only Expire (ping the session)
		if (pStream == nullptr)
		{
			auto reply = c->run(command("EXPIRE")(ansi)(ExpiresPar * 60L)); // http://www.redis.io/commands/expire is in seconds

			// could be both 0 or 1 (0 if it does not exist)
			hr = reply.type() == reply::type_t::INTEGER && reply.integer() == 1 ? S_OK : E_FAIL;			
		}
		else
		{
			ULONG read;
			STATSTG statst = { 0 };
			pStream->Stat(&statst, STATFLAG_NONAME);
			read = statst.cbSize.LowPart;
			if (read > 0)
			{
				const UINT bufLen = 0x1000;
				unsigned char bytes[bufLen];
				ULONG read2 = 0;
				ULONG didRead = 0;	
				//std::string theLen = std::to_string((unsigned int)read);
				//size_t numLen = ceil(log10((unsigned int)read + 1));
				PersistMetaData meta;
				std::string strBuf;			
				strBuf.reserve(meta.sizeofMeta + read);			
				meta.m_Expires = ExpiresPar;
				meta.m_Liquid = LiquidPar == TRUE ? VARIANT_TRUE : VARIANT_FALSE;
				meta.m_zLen = zLenPar;
				meta.m_ReEntrance = ReEntrancePar == TRUE ? VARIANT_TRUE : VARIANT_FALSE;
				memcpy(&meta.m_LastUpdated, previousLastUpdated, sizeof(DBTIMESTAMP));
				//it's a binary string. Looks weird, though
				strBuf.append((PCSTR)&meta, (size_t)meta.sizeofMeta);
				//write binary safe string
				hr = S_OK;
				do
				{
					hr = pStream->Read(bytes, bufLen, &read2);
					didRead += read2;
					if (read2 > 0)
					{
						strBuf.append((PCSTR)bytes, (size_t)read2);
					}
				} while (hr == S_OK && didRead < read);			
				hr = S_OK; //reset S_FALSE to S_OK
				auto repl = c->run(command("SET")(ansi)(strBuf) ("EX")(ExpiresPar*60));
				if (repl.type() == reply::type_t::_ERROR)
				{
					logModule.Write(L"Fail CpSessionSave because of %s", std::wstring(repl.str().begin(), repl.str().end()).c_str());
					hr = E_FAIL;
				}
			}			
		}		
		pool->put(c);
        return hr;
    }
	//returns S_OK if success. E_FAIL if the session does not exist
	HRESULT static __stdcall SessionDelete(const simple_pool::ptr_t pool, PUCHAR appKey, PUCHAR guid) throw()
	{
		auto c = pool->get();
		auto sAppkey = HexStringFromMemory(appKey, sizeof(GUID));
		auto sGuid = HexStringFromMemory(guid, sizeof(GUID));
		std::string ansi;
		ansi.reserve(sizeof(GUID) * 2 + 1);
		ansi.append(sAppkey);
		ansi.append(":");
		ansi.append(sGuid);
		auto reply = c->run(command("EXPIRE")(ansi)("0")); // not sure if 0 means 'eternal' so look for milliseconds
		pool->put(c);

		return reply.type() == reply::type_t::INTEGER && reply.integer() == (long long)1 ? S_OK : E_FAIL;
	}
	HRESULT static _stdcall SessionInsert(const simple_pool::ptr_t &pool,
		const GUID *appKey, const GUID *guid,
		const LONG Expires,
		const BOOL ReEntrance,
		const BOOL liquid) throw()
	{

		auto appkey = HexStringFromMemory((PBYTE)appKey, sizeof(GUID));
		auto skey = HexStringFromMemory((PBYTE)guid, sizeof(GUID));
		std::string ansi;
		ansi.reserve(sizeof(GUID) * 2 + 1);
		ansi.append(appkey);
		ansi.append(":");
		ansi.append(skey);

		std::string buf;
		PersistMetaData meta;
		std::string strBuf;
		meta.m_Expires = Expires;
		meta.m_Liquid = liquid == TRUE ? VARIANT_TRUE : VARIANT_FALSE;
		meta.m_ReEntrance = ReEntrance == TRUE ? VARIANT_TRUE : VARIANT_FALSE;
		buf.append((PCSTR)&meta, meta.sizeofMeta);

		auto conn = pool->get();
		auto reply = conn->run(command("SET")(ansi)(buf)("EX")(Expires * 60));

		pool->put(conn);
		//std::wstring wstr = s2ws(ansi); //TODO: must all be ansi
		logModule.Write(L"insert %s", std::wstring(ansi.begin(), ansi.end()).c_str());

		auto hr = reply.type() == reply::type_t::STATUS && reply.str() == "OK" ? S_OK : E_FAIL;
		return hr;
	}
};

class _CpSessionGetAccessor 
{
public:
	_CpSessionGetAccessor(): 
		IsNULL(FALSE),
		m_blobLength(0),
		m_Expires(20),
		m_ReEntrance(VARIANT_FALSE),
		m_Liquid(VARIANT_FALSE),
		m_zLen(0),
		m_pStream(NULL)
	{
		memset(m_GUID, 0, sizeof(GUID));
		memset(m_timest, 0, sizeof(DBTIMESTAMP));
	}
	
	BOOL IsNULL;
	LONG m_blobLength;	
	BYTE m_GUID[16];	
	BYTE m_timest[8];
	DBTIMESTAMP m_LastUpdated;
	LONG m_Expires;
	VARIANT_BOOL m_ReEntrance;
	VARIANT_BOOL m_Liquid;		
	CComObject<CStream>  *m_pStream;
	LONG m_zLen;

};

class CpSessionGet: 
	public _CpSessionGetAccessor
{
public:
	//Gets the session returns S_FALSE if the session is empty
	HRESULT __stdcall OpenRowset(const simple_pool::ptr_t &pool,
		PBYTE m_App_KeyPar, PBYTE m_GUIDPar) throw()
	{
		auto appkey = HexStringFromMemory(m_App_KeyPar, sizeof(GUID));
		auto skey = HexStringFromMemory(m_GUIDPar, sizeof(GUID));
		std::string ansi;
		ansi.reserve(sizeof(GUID) * 2 + 1);
		ansi.append(appkey);
		ansi.append(":");
		ansi.append(skey);

		auto conn = pool->get();
		if (conn == redis3m::connection::ptr_t()) // authentication happens DURING pool-get
		{ 
			
			return E_ACCESSDENIED;
		}
		auto repl = conn->run(command("GET")(ansi));

	
		auto result = S_OK;
		switch (repl.type())
		{		
		case reply::type_t::NIL:
			result = S_FALSE;
			m_zLen = m_blobLength = 0;
			IsNULL = TRUE;
			break;
		case reply::type_t::STATUS:
		case reply::type_t::_ERROR:
			m_zLen = m_blobLength = 0;			
			logModule.Write(L"CpSessionGet failed because %s", std::wstring(repl.str().begin(), repl.str().end()).c_str());
			result = E_FAIL;
			break;
		case reply::type_t::STRING:

			int buf = 0x1000;
			std::string str = repl.str();
			m_blobLength = repl.strlen();
			PersistMetaData meta;
			//against all sanity. But justified, we do not modify the string, just read from it
			memcpy(&meta, (void*)str.data(), meta.sizeofMeta);
			m_LastUpdated = meta.m_LastUpdated;
			m_Expires = meta.m_Expires;
			m_Liquid = meta.m_Liquid;
			m_ReEntrance = meta.m_ReEntrance;
			int baseX = meta.sizeofMeta;
			m_zLen = meta.m_zLen;
			IsNULL = m_blobLength == meta.sizeofMeta ? TRUE : FALSE;
			if (IsNULL == FALSE)
			{
				CComObject<CStream>::CreateInstance(&m_pStream);
				m_pStream->AddRef();
				//start blobLength = 128
				for (int x = m_blobLength - baseX; x > 0; x -= buf)
				{	//if 128 > 0x1000 ? buf : m_blobLength = 128
					ULONG BytesToWrite = (x > buf ? buf : x);						
					m_pStream->Write((void*)&str.data()[baseX], BytesToWrite, nullptr);
					baseX += BytesToWrite;
				}
				LARGE_INTEGER nl = { 0 };
				m_pStream->Seek(nl, STREAM_SEEK::STREAM_SEEK_SET, nullptr);
			}
			break;
		}		
		pool->put(conn);
        return result;
	}
	~CpSessionGet()
	{
		if (m_pStream != nullptr)
		{
			m_pStream->Release();
			m_pStream = nullptr;
		}
	}
	
};
