#include <iostream>
#include <memory>
#include <string>
#include <mutex>
#include "udpserver.h"

using std::string;


//=============================================================================
// This creates both the sender socket and the server socket.   It will never
// return unless something goes awry during initial socket creation
//=============================================================================
void CUdpServer::start(int32_t localPort, int32_t remotePort)
{
    // Create the addrinfo structure for sending UDP messages
    if (!NetUtil::get_server_addrinfo(SOCK_DGRAM, "localhost", remotePort,
                                      AF_INET, &remote_))
    {
        std::cerr << "Failed to create addrinfo structure\n";
        return;
    }

    // Create the UDP sender socket
    sendersd_ = socket(remote_.family, remote_.socktype, remote_.protocol);

    // If that failed, tell the caller
    if (sendersd_ < 0)
    {   
        std::cerr << "Failed to create UDP socket\n";
        return;
    }


    // Fetch information about the local machine
    addrinfo_t server = NetUtil::get_local_addrinfo(SOCK_DGRAM, localPort, "localhost", AF_UNSPEC);

    // Create the UDP server socket
    int sd = socket(server.family, server.socktype, server.protocol);

    // If that failed, tell the caller
    if (sd < 0)
    {
        std::cerr << "Unable to create UDP server socket for port " << localPort << "\n";
        return;
    }

    // Bind the socket to the specified port
    if (bind(sd, server, server.addrlen) < 0) 
    {
        std::cerr << "Unable to bind UDP server to port " << localPort << "\n";
        return;
    }


    // We're going to sit and listen for incoming messages forever
    while (true)
    {
        // Wait for a UDP message to arrive
        int byteCount = recvfrom(sd, msgBuffer_, sizeof(msgBuffer_), 0, nullptr, nullptr);

        if (byteCount > 0)
        {
            // Nul-terminate the buffer for convenience;
            msgBuffer_[byteCount] = 0;

            // Handle the message and fetch the response
            string response = onMessage(msgBuffer_);

            // If there was a response, send it to the conduit
            if (!response.empty()) send(response);
        }
    }

}
//=============================================================================


//=============================================================================
// This is used to send messages back through the conduit.
//
// This is thread-safe
//=============================================================================
void CUdpServer::send(std::string msg)
{
    static std::mutex mtx;

    mtx.lock();
    ::sendto(sendersd_, msg.c_str(), msg.length(), 0, remote_, remote_.addrlen);
    mtx.unlock();
}
//=============================================================================



//=============================================================================
// This gets called whenever a message is received
//=============================================================================
std::string CUdpServer::onMessage(const char* msg)
{
    std::cout << "We received: " << msg << "\n";
    return "This is our response";
}
//=============================================================================
