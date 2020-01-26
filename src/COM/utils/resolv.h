// Copyright (c) 2014 Luca Marturana. All rights reserved.
// Licensed under Apache 2.0, see LICENSE for details

#pragma once

#include "exception.h"
#include <string>
#include <vector>
#include "..\deps\hiredis\sockcompat.h"
#include "..\deps\hiredis\hiredis.h"
namespace redis3m
{
namespace resolv
{
   REDIS3M_EXCEPTION(cannot_resolve_hostname)
   std::vector<std::string> get_addresses(const std::string &hostname);
}
}
