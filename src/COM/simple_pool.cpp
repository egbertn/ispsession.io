// Copyright (c) 2014 Luca Marturana. All rights reserved.
// Licensed under Apache 2.0, see LICENSE for details
#include "stdafx.h"
#include "include/redis3m/simple_pool.h"
#include "include/redis3m/command.h"

using namespace redis3m;

connection::ptr_t simple_pool::get()
{
    connection::ptr_t ret;

    {
        //std::lock_guard<std::mutex> lock(access_mutex);
		_access_mutex.Enter();
        std::set<connection::ptr_t>::iterator it = connections.begin();
        if (it != connections.end())
        {
            ret = *it;
            connections.erase(it);
        }

    }

    if (!ret)
    {
      /*  if (!_path.empty())
        {
            ret = connection::create_unix(_path);
        }
        else
        {*/
            ret = connection::create(_host, _port);
        //}
        // Setup connections selecting db
			if (!_password.empty())
			{
				auto response = ret->run(command("AUTH") << _password );
				if (response.type() == reply::type_t::_ERROR)
				{					
					ret.reset();
				}
			}
			if (_database != 0 && ret)
			{
				auto response = ret->run(command("SELECT") << _database);
			}
    }
	_access_mutex.Leave();
    return ret;
}


void simple_pool::put(connection::ptr_t conn)
{
    if (conn->is_valid())
    {
        //std::lock_guard<std::mutex> lock(access_mutex);
		_access_mutex.Enter();
        connections.insert(conn);
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
