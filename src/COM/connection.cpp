// Copyright (c) 2014 Luca Marturana. All rights reserved.
// Licensed under Apache 2.0, see LICENSE for details
// Some modifications, added some fixes, times out within 5 seconds
#include "stdafx.h"
#include "include\redis3m\connection.h"
#include "include\hiredis\hiredis.h"

using namespace redis3m;

connection::connection(const std::string& host, const unsigned port/*, const std::string& password*/)
{
	timeval tv = { 5,0 };
    c = redisConnectWithTimeout(host.c_str(), port, tv);
    if (c->err != REDIS_OK)
    {
        redisFree(c);
        throw unable_to_connect();
    }

}

connection::connection(const std::string& path)
{
    c = redisConnectUnix(path.c_str());
    if (c->err != REDIS_OK)
    {
        redisFree(c);
        throw unable_to_connect();
    }
	
}

connection::~connection()
{
	if (is_valid())
	{		
		redisFree(c);		
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
