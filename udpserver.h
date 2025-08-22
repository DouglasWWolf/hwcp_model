#pragma once
#include <stdint.h>
#include "netutil.h"

class CUdpServer
{
public:

    // Call this to start the server.  This does not return
    void    start(int32_t localPort, int32_t remotePort);

    // This is used to send messages back thru the conduit
    void    send(std::string msg);

    // This gets called whenever a message is received.  Override it.
    virtual std::string onMessage(const char* msg);

protected:

    addrinfo_t  remote_;
    uint32_t    sendersd_;
    char        msgBuffer_[66000];
};
