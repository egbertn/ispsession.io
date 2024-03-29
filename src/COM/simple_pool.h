// Copyright (c) 2014 Luca Marturana. All rights reserved.
// Licensed under Apache 2.0, see LICENSE for details

#pragma once

#include "utils/noncopyable.h"
#include "connection.h"
#include <set>
#include <memory>
#include <mutex>
#include <atlsync.h>
namespace redis3m
{
	static std::vector<connection::ptr_t> connections;
	static ATL::CCriticalSection _access_mutex;
	//static ATL::CHandle _timer;
    static HANDLE _timer = nullptr;
	//static ATL::CHandle _threadHandle;
    static HANDLE _threadHandle = nullptr;
	/*DWORD(WINAPI *PTHREAD_START_ROUTINE)(
		LPVOID lpThreadParameter
		);*/
	static DWORD __stdcall  TimerThread(void* param);
	static bool __stdcall TimerAPCProc() noexcept;
	/**
 * @brief Manages a pool of connections to a single Redis server
 */
class simple_pool: utils::noncopyable
{
public:
    typedef std::shared_ptr<simple_pool> ptr_t;
    REDIS3M_EXCEPTION(too_much_retries)

    static inline ptr_t create(const std::string& host="localhost", unsigned int port=6379, const std::string& password=nullptr)
    {
        return ptr_t(new simple_pool(host, port, password));
    }
    
    static inline ptr_t create_unix(const std::string& path)
    {
        return ptr_t(new simple_pool(path));
    }

    template<typename Ret>
    /**
     * @brief Execute a block of code passing a connection::ptr_t
     * if something fails, like broken connection, it will automatically
     * retry with an another one
     * @param f function to run, C++11 lambdas are perfect
     * @param retries how much retries do
     * @return
     */
    Ret run_with_connection(std::function<Ret(connection::ptr_t)> f,
                            unsigned int retries=5)
    {
        while (retries > 0)
        {
            try
            {
                connection::ptr_t c = get();
                Ret r = f(c);
                put(c);
                return r;
            } catch (const connection_error& ex)
            {
                --retries;
            }
        }
        throw too_much_retries();
    }

    /**
     * @brief Get a working connection
     * @return
     */
    connection::ptr_t get();

    /**
     * @brief Put back a connection for reuse
     * @param conn
     */
    void put(connection::ptr_t conn);

	
		
    //void run_with_connection(std::function<void(connection::ptr_t)> f, unsigned int retries=5);

    /**
     * @brief Set default database, all connection will be initialized selecting
     * this database.
     * @param value
     */
    inline void set_database(unsigned int value) { _database = value; }

private:
    simple_pool(const std::string& path);
    simple_pool(const std::string& host, unsigned int port, const std::string& password);
	
    std::string _path;
    std::string _host;
	std::string _password;
	bool _useSSL;
    unsigned int _port;
    unsigned int _database;
};

template<>
void simple_pool::run_with_connection(std::function<void(connection::ptr_t)> f,
                            unsigned int retries);
}
