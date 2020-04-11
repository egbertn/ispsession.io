// Copyright (c) 2014 Luca Marturana. All rights reserved.
// Licensed under Apache 2.0, see LICENSE for details
// Added some pooling logics. e.g. a mutex was added to create thread safety
// Added timing logic (TimerThread), after X (say 20) seconds, a connection is removed from the pool.

#include "stdafx.h"
#include "simple_pool.h"
#include "command.h"
#include <thread>
#include "tools.h"
using namespace redis3m;
#define _SECOND 10000000
#define KEEPCONNECTION_IN_POOL_SEC 20

static DWORD __stdcall  redis3m::TimerThread(void*)
{
	auto handle = ::CreateWaitableTimerA(nullptr, TRUE, "ISP Session Timer");
	if (handle == nullptr)
	{
		auto error = ::GetLastError();
		logModule.Write(L"timer fails with %d", error);
		return 0;
	}
	if (_timer != nullptr)
	{
		logModule.Write(L"Should not happen timer was called while it existed");
		CloseHandle(handle);
		return 0;
	}
	logModule.Write(L"Created TimerThread %d", _threadHandle);
	_timer = handle;

	LARGE_INTEGER liDueTime;

	liDueTime.QuadPart = -2 * _SECOND;
	for(;;)
	{
		::SetWaitableTimer(_timer, &liDueTime, 0, nullptr, nullptr, 0);
		if (::WaitForSingleObject(_timer, INFINITE) != WAIT_OBJECT_0)
		{
			auto err = ::GetLastError();//should be 6
			break;
		}
		else
		{
			if (TimerAPCProc() == false)
			{
				break;
			}
		}
	}
	return 0;
}
bool isExpired(const shared_ptr<redis3m::connection> n) noexcept
{
	auto now = std::chrono::system_clock::now();
	return (std::chrono::duration_cast<std::chrono::seconds>(now - n->_startSessionRequest).count() > KEEPCONNECTION_IN_POOL_SEC);
}
//we must clean up connections before Redis disconnects itself
// todo find out if we can optimize this
// so, what's the problem Redisfree(c) in Connection.cpp delivers a system exception if Redis closed it already
static bool _stdcall redis3m::TimerAPCProc() noexcept
{
	bool result = true;
	_access_mutex.Enter();
	auto erased = false;
	if (connections.size() > 0)
	{
		try {
			
			/* s.erase(s.begin(), std::find_if(s.begin(), s.end(),
		std::not1(std::ptr_fun<int, int>(std::isspace)))); */
			for(;;)
			{
				auto f = std::find_if(connections.cbegin(), connections.cend(), isExpired);
				if (f == connections.end())
					break;

				erased = true;
				connections.erase(f);
			
			}
			if (erased)
				logModule.Write(L"Removed Redis conn from pool, left over %d", connections.size());		

		}
		catch (...)
		{
			logModule.Write(L"Cleaning Connection pool failed");
		}

	}
	
	if (connections.size() == 0 && erased && _timer != nullptr)
	{
		result = false;
		::CloseHandle(_timer);
		::CloseHandle(_threadHandle);
		_timer =
		_threadHandle = nullptr;
		//_timer.Close();
		//_threadHandle.Close();
		logModule.Write(L"TimerProc canceled");
		
	}
	_access_mutex.Leave();
	return result;
};

connection::ptr_t simple_pool::get()
{

	connection::ptr_t ret;

	_access_mutex.Enter();
	auto sz = connections.size();
	if (sz > 0)
	{		
		ret = connections.at(sz-1);
		ret->_startSessionRequest = std::chrono::system_clock::now();
		connections.pop_back();
		
	}
	if (_timer == nullptr)
	{
		auto handle = ::CreateThread(NULL, NULL, TimerThread, NULL, NULL, NULL);
		_threadHandle =handle;
	}
	_access_mutex.Leave();
	if (!ret)
	{
		/*  if (!_path.empty())
		  {
			  ret = connection::create_unix(_path);
		  }
		  else
		  {*/
		ret = redis3m::connection::ptr_t();
		//}
		// Setup connections selecting db
		try
		{
			ret = connection::create(_host, _port);
			ret->_startSessionRequest = std::chrono::system_clock::now();
			if (!_password.empty())
			{
				auto response = ret->run(command("AUTH") (_password));
				if (response.type() == reply::type_t::_ERROR)
				{
					logModule.Write(L"AUTH failed");
					ret.reset();
					return redis3m::connection::ptr_t();
				}
			}
			if (_database != 0 && ret != connection::ptr_t())
			{
				auto response = ret->run(command("SELECT")(_database));
				if (response.type() == reply::type_t::_ERROR)
				{
					logModule.Write(L"SELECT database %s", response.str());
				}
			}
			logModule.Write(L"Add Redis Conn to pool %d, %s, %d", _database, s2ws(_host).c_str(), _port);
		}
		catch (connection_error ex)
		{
			logModule.Write(L"connection_error occurred %x", s2ws(ex.what()).c_str());
			return redis3m::connection::ptr_t();
		}
	}
	
	return ret;
}


void simple_pool::put(connection::ptr_t conn)
{
	_access_mutex.Enter();
    if (conn->is_valid())
    {
        connections.push_back(conn);
    }
	_access_mutex.Leave();
}

simple_pool::simple_pool(const std::string &host, unsigned int port, const std::string& password):
    _host(host),
    _port(port),
    _database(0),
	_password(password)
{

}

simple_pool::simple_pool(const std::string &path):
    _path(path),
    _database(0)
{

}

template<>
void simple_pool::run_with_connection(std::function<void(connection::ptr_t)> f,
                                unsigned int retries)
{
    while (retries > 0)
    {
        try
        {
            connection::ptr_t c = get();
            f(c);
            put(c);
            return;
        } catch (const connection_error& )
        {
            --retries;
        }
    }
    throw too_much_retries();
}
