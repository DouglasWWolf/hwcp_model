#pragma once
#include "json_server.h"

struct globals_t
{
    std::string configFile = "hwcp_model.conf";
    CJsonServer jsonServer;
};
