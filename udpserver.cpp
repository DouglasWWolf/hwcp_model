#include <iostream>
#include <memory>
#include <string>
#include <mutex>
#include <cstring>
#include "udpserver.h"

using std::string;

//=============================================================================
// makeServer() - Creates a UDP server on the specified port
//=============================================================================
static int makeServer(int port)
{
    uint32_t optval = 1;

    // Create the socket and complain if we can't
    int sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0)
    {
        std::cerr << "Failed while creating UDP socket\n";
        exit(1);        
    }

    // Ensure the socket can be reused
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(optval));

    // Build the address structure of the UDP server
    struct sockaddr_in serveraddr; 
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)port);

    // Bind the socket to the port
    if (bind(sd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
    {
        std::cerr << "Unable to bind UDP server to port " << port << "\n";
        exit(1);
    }

    // Return the socket descriptor to the caller
    return sd;
}
//=============================================================================


//=============================================================================
// This creates both the sender socket and the server socket.   It will never
// return unless something goes awry during initial socket creation
//=============================================================================
void CUdpServer::start(int32_t localPort, int32_t remotePort)
{
    // Create the addrinfo structure for sending UDP messages
    if (!NetUtil::get_server_addrinfo(SOCK_DGRAM, "0.0.0.0", remotePort,
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

    // Create a UDP server
    int sd = makeServer(localPort);

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
    std::cout << "Default message handler received: " << msg << "\n";
    return "This is our response";
}
//=============================================================================
