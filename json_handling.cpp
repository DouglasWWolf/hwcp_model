#include <iostream>
#include "globals.h"
#include "msg_structs.h"


// Access to our global variables
extern globals_t g;


std::string MsgPing::dispatch(json& j)
{
    MsgPing    msg(j);
    MsgPingRsp rsp(j);

    msg.deserialize();
    msg.handle(rsp);
    return rsp.serialize();
}


void MsgPing::deserialize()
{
    req.value = jsonGetNumber("value");
}


std::string MsgPingRsp::serialize()
{
    json j;
    j["value"] = value;
    return encodeJson(j);
}




//=============================================================================
// This gets called whenever a message is received
//=============================================================================
std::string CJsonServer::onMessage(const char* msg)
{
    // In debug mode, display the message we just received
    if (g.debug) printf("%s\n-----------------\n", msg);

    // Parse the JSON text of the received message
    try
    {
        // Parse the JSON text into a json object
        json j = json::parse(msg);

        // Fetch the message type
        std::string messageType = j["MessageType"];

        if (messageType == "MsgPing")
            return MsgPing::dispatch(j);
    }

    catch(const JsonError& e)
    {
        std::cerr << e.what() << '\n';
        return "";            
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return "";
    }

    // If we get here, we don't recognize the message type
    return "";
}
//=============================================================================
