#include <unistd.h>
#include <signal.h>
#include <cstring>
#include <string>
#include <iostream>
#include "history.h"
#include "config_file.h"

#include "udpserver.h"

using std::cout;
using std::cerr;
using std::string;

string configFile = "hwcp_model.conf";

struct 
{
    string  serviceName;
    string  conduitConf;
    int32_t conduitUdpPort;
    int32_t localUdpPort;
} config;


struct
{
    CUdpServer server;
} g;


void readOptions(const char** argv);
void readConfigFile(string filename);


//=============================================================================
// Execution begins here
//=============================================================================
int main(int argc, const char** argv)
{
    // Tell the world who (and what version) we are!
    cout << "hwcp_model " << SW_VERSION << "\n";

    // We want to ensure that writing to a closed pipe doesn't kill us!
    signal(SIGPIPE, SIG_IGN);

    // Read the command-line options
    readOptions(argv);

    // Read the configuration file
    readConfigFile(configFile);


    g.server.start(config.localUdpPort, config.conduitUdpPort);

    // We should never get here!
    exit(1);
    /*

    // Create a pipe between our thread and the "resetSocketThread"
    pipe(serverResetPipe);

    // Fetch the file descriptor of the read-side of the pipe
    int pipefd = serverResetPipe[0];

    // Create the addrinfo structure for sending UDP messages
    if (!NetUtil::get_server_addrinfo(SOCK_DGRAM, "localhost", 1, AF_INET, &addrinfo))
    {
        cerr << "Failed to create addrinfo structure\n";
        exit(1);        
    }

    // Create the UDP sendoer socket
    senderfd = socket(addrinfo.family, addrinfo.socktype, addrinfo.protocol);

    // If that failed, tell the caller
    if (senderfd < 0)
    {   
        cerr << "Failed to create UDP socket\n";
        exit(1);
    }


    // Launch the "resetSocketThread".   That thread will inform us (via the
    // pipe) whenever someone connects to the server it's running.   When this
    // happens, it's a sign that we should close our own server.
    std::thread th1(
                    resetSocketThread,
                    config.serverPort + 1,
                    serverResetPipe[1]
                );
    th1.detach();

    // Launch the UDP server thread
    std::thread th2(udpServerThread, config.udpPort);
    th2.detach();

    */
  
};
//=============================================================================





//=============================================================================
// Reads the command line options. 
// Sets global variables:
//   configFile
//=============================================================================
void readOptions(const char** argv)
{
    // Skip over the executable name
    ++argv;

    // Loop through the tokens on the command line...
    while (*argv)
    {
        const char* p = *argv++;
        
        // If this option begins with "--", reduce that to a single "-"
        if (p[0] == '-' && p[1] == '-') ++p;
        
        // Fetch the command line option as a string
        string option = p;

        // Is the user giving us the name of a config file?
        if (option == "-config" && *argv)
        {
            configFile = *argv++;
            continue;
        }

        // If we get here, its an invalid command line
        cerr << "Invalid option: " + option + "\n";
        exit(1);
    }
}
//=============================================================================



//=============================================================================
// Read the configuration file 
//
// Passed: name of the config file
//
// On exit: all the fields in the global "config" structure are filled in
//=============================================================================
void readConfigFile(string filename)
{
    CConfigFile c;
    CConfigScript script;

    // Read the config file and complain if we can't
    if (!c.read(filename, false))
    {
        cerr << "Not found: " + configFile + "\n";
        exit(1);
    }


    // Parse our settings out of our configuration file
    try
    {
        // Fetch our service name
        c.get("service_name", &config.serviceName);

        // Fetch the name of the conduit configuration file
        c.get("conduit_conf", &config.conduitConf);

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        exit(1);
    }

    // Read the conduit config file and complain if we can't
    if (!c.read(config.conduitConf, false))
    {
        cerr << "Not found: " + config.conduitConf + "\n";
        exit(1);
    }

    try
    {
        // Fetch the UDP port number that the conduit will listen on
        c.get("udp_port", &config.conduitUdpPort);

        // Fetch the map of service names to port numbers
        c.get("services", &script);

        // Just in case we don't find out port in the script
        config.localUdpPort = 0;

        // Read the map of service-names to port-numbers
        while (script.get_next_line())
        {
            string name = script.get_next_token();
            int    port = script.get_next_int();
            if (name == config.serviceName)
            {
                config.localUdpPort = port;
                break;                
            }
        }

        // If our service name isn't defined in the conduit configuration
        // file, this is a drop-dead error!
        if (config.localUdpPort == 0)
        {
            cerr << "Service \"" + config.serviceName + "\" has no port\n";
            exit(1);
        }

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        exit(1);
    }
}
//=============================================================================


