// Copyright (c) 2014 Luca Marturana. All rights reserved.
// Licensed under Apache 2.0, see LICENSE for details
// Some modifications, added some fixes, times out within 5 seconds
#include "stdafx.h"
#include "connection.h"
//#include "..\..\src\src\Win32_Interop\Win32_FDAPI.h"
#include "..\..\src\deps\hiredis\hiredis.h"

using namespace redis3m;
//bool connectFUNC3(char* ipaddr)
//{
//
//    struct sockaddr_in server = { 0 };
//    server.sin_family = AF_INET;
//    server.sin_addr.s_addr = inet_addr(ipaddr);
//    server.sin_port = htons(5577);
//
//    // ipaddr valid?
//    if (server.sin_addr.s_addr == INADDR_NONE)
//        return false;
//
//    auto sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//    if (sock == INVALID_SOCKET)
//        return false;
//
//    // put socked in non-blocking mode...
//    u_long block = 1;
//    if (ioctlsocket(sock, FIONBIO, &block) == SOCKET_ERROR)
//    {
//        close(sock);
//        return false;
//    }
//
//    if (connect(sock, (struct sockaddr*) & server, sizeof(server)) == SOCKET_ERROR)
//    {
//        if (WSAGetLastError() != WSAEWOULDBLOCK)
//        {
//            // connection failed
//            close(sock);
//            return false;
//        }
//
//        // connection pending
//
//        fd_set setW, setE;
//
//        FD_ZERO(&setW);
//        FD_SET(sock, &setW);
//        FD_ZERO(&setE);
//        FD_SET(sock, &setE);
//
//        timeval time_out = { 0 };
//        time_out.tv_sec = 5;
//        time_out.tv_usec = 0;
//
//        int ret = select(0, NULL, &setW, &setE, &time_out);
//        if (ret <= 0)
//        {
//            // select() failed or connection timed out
//            close(sock);
//            if (ret == 0)
//                WSASetLastError(WSAETIMEDOUT);
//            return false;
//        }
//
//        if (FD_ISSET(sock, &setE))
//        {
//            // connection failed
//            int err = 0;
//            int sz = sizeof(err);
//            int erropt = SO_ERROR;
//            getsockopt(sock, SOL_SOCKET, erropt, &err, &sz);
//            close(sock);
//            WSASetLastError(err);
//            return false;
//        }
//    }
//
//    // connection successful
//
//    // put socked in blocking mode...
//    block = 0;
//    if (ioctlsocket(sock, FIONBIO, &block) == SOCKET_ERROR)
//    {
//        close(sock);
//        return false;
//    }
//
//    return true;
//}
connection::connection(const std::string& host, const unsigned port/*, const std::string& password*/)
{
    timeval tv = { 5,0 };//because of a bug in hiridis 50 microseconds are 5 seconds
    /*redisOptions opt = { 0 };
    REDIS_OPTIONS_SET_TCP(&opt, host.c_str(), port);
    opt.options |= REDIS_OPT_NONBLOCK;
    opt.timeout = &tv;
    c = redisConnectWithOptions(&opt);
    if (c == nullptr || c->err != REDIS_OK)
    {
        redisFree(c);
        throw unable_to_connect();
    }
    redisFree(c);
    opt.options = 0;
    c = redisConnectWithOptions(&opt);*/
    c = redisConnectWithTimeout(host.c_str(), port, tv);
    
    if (c->err != REDIS_OK)
    {
        redisFree(c);
        throw unable_to_connect();
    }

    redisEnableKeepAlive(c);
    
}

//connection::connection(const std::string& path)
//{
//    c = redisConnectUnix(path.c_str());
//    if (c->err != REDIS_OK)
//    {
//        redisFree(c);
//        throw unable_to_connect();
//    }
//   
//    redisEnableKeepAlive(c);
//
//	
//}

connection::~connection()
{
	if (is_valid())
	{		
        try {
            redisFree(c);
        }
        catch (...)
        {
            // should not happen in redis.conf it should have
            //timeout 0 and  tcp-keepalive 300
            
        }
	}
	c->err = REDIS_ERR;
}

void connection::append(const std::vector<std::string> &commands)
{
    std::vector<const char*> argv;
    argv.reserve(commands.size());
    std::vector<size_t> argvlen;
    argvlen.reserve(commands.size());

    for (std::vector<std::string>::const_iterator it = commands.begin(); it != commands.end(); ++it) {
        argv.push_back(it->c_str());
        argvlen.push_back(it->size());
    }

    int ret = redisAppendCommandArgv(c, static_cast<int>(commands.size()), argv.data(), argvlen.data());
    if (ret != REDIS_OK)
    {
        throw transport_failure();
    }
}

reply connection::get_reply()
{
    redisReply *r;
    int error = redisGetReply(c, reinterpret_cast<void**>(&r));
    if (error != REDIS_OK)
    {
        throw transport_failure();
    }
    reply ret(r);
    freeReplyObject(r);

	if (ret.type() == reply::type_t::_ERROR &&
		(ret.str().find("READONLY") == 0) )
    {
        throw slave_read_only();
    }
    return ret;
}

std::vector<reply> connection::get_replies(unsigned int count)
{
    std::vector<reply> ret;
    for (unsigned int i=0; i < count; ++i)
    {
        ret.push_back(get_reply());
    }
    return ret;
}

bool connection::is_valid() const
{
    return c->err == REDIS_OK;
}
