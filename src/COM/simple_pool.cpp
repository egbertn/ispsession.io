// Copyright (c) 2014 Luca Marturana. All rights reserved.
// Licensed under Apache 2.0, see LICENSE for details
// Added some pooling logics. e.g. a mutex was added to create thread safety
// Added timing logic (TimerThread), after X (say 20) seconds, a connection is removed from the pool.

#include "stdafx.h"
#include "include/redis3m/simple_pool.h"
#include "include/redis3m/command.h"
#include <thread>
#include "tools.h"
using namespace redis3m;
#define _SECOND 10000000
#define KEEPCONNECTION_IN_POOL_SEC 20
DWORD __stdcall  redis3m::TimerThread(void* param)	
{

	_timer.Attach(::CreateWaitableTimer(NULL, TRUE, NULL));

	LARGE_INTEGER liDueTime;

	liDueTime.QuadPart = -10 * _SECOND;

	::SetWaitableTimer(_timer, &liDueTime, 2000, NULL, NULL, 0);
	for(;;)
	{
		if (::WaitForSingleObject(_timer, INFINITE) != WAIT_OBJECT_0)
		{
			auto err = ::GetLastError();//should be 6
			break;
		}
		else
		{
			TimerAPCProc();
		}
	} 
	return 0;
}
//we must clean up connections before Redis disconnects itself
// todo find out if we can optimize this
// so, what's the problem Redisfree(c) in Connection.cpp delivers a system exception if Redis closed it already
void __stdcall redis3m::TimerAPCProc() noexcept
{
	_access_mutex.Enter();
	if (connections.size() > 0)
	{
		try {
			std::vector<int> toErase;
			int vectorPos = 0;
			for (auto x = connections.cbegin(); x != connections.cend(); ++x)
			{
				auto con = *x;

				if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - con->_startSessionRequest).count() > KEEPCONNECTION_IN_POOL_SEC)
				{
					toErase.push_back(vectorPos);
					//connections.erase(x++);// the ++ seems to work, do not change

					logModule.Write(L"Removed Redis conn from pool, left over %d", connections.size());
				}
				vectorPos++;
			}
			for (auto x = toErase.cbegin(); x != toErase.cend(); ++x)
			{
				connections.erase(connections.cbegin() + (*x));

			}
		}
		catch (...)
		{
			logModule.Write(L"Cleaning Connection pool failed");
		}

	}
	
	else
	{
		auto success = ::CancelWaitableTimer(_timer);
		_timer.Close();
		_threadHandle.Close();
		logModule.Write(L"Canceling timer thread %d", success);
	}
	_access_mutex.Leave();
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
		auto handle  = ::CreateThread(NULL, NULL, TimerThread, NULL, NULL, NULL);
		_threadHandle.Attach(handle);
		logModule.Write(L"Created TimerThread %d", handle);
		//std::thread trh(TimerThread);
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
			if (!_password.empty())
			{
				auto response = ret->run(command("AUTH") (_password));
				if (response.type() == reply::type_t::_ERROR)
				{
					logModule.Write(L"AUTH failed");
					ret.reset();
					throw "AUTH failed";
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
			ret->_startSessionRequest = std::chrono::system_clock::now();
		}
		catch (connection_error ex)
		{
			logModule.Write(L"connection_error occurred %x", s2ws(ex.what()).c_str());
			throw ex.what();
		}
	}
	
	return ret;
}


void simple_pool::put(connection::ptr_t conn)
{
    if (conn->is_valid())
    {
    	_access_mutex.Enter();
        connections.push_back(conn);
		_access_mutex.Leave();
    }
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
