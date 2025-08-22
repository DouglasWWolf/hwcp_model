#pragma once
#include "udpserver.h"

class CJsonServer : public CUdpServer
{
public:
    virtual std::string onMessage(const char* msg);
};
