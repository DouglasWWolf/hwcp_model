#include <iostream>
#include "globals.h"
#include "msg_structs.h"


// Access to our global variables
extern globals_t g;

// The names of required fields
const char* MESSAGE_TYPE    = "MessageType";
const char* IMM_REPLY_TO    = "ImmediateReplyTo";
const char* FUTURE_REPLY_TO = "FutureReplyTo";
const char* CONTEXT_ID      = "ContextId";
const char* RESPONSE_CODE   = "ResponseCode";
const char* RESPONSE_TEXT   = "ResponseText";


#define STD_RSP()     j[MESSAGE_TYPE ] = mand.MessageType;     \
                      j[CONTEXT_ID   ] = mand.ContextId;       \
                      j[RESPONSE_CODE] = mand.ResponseCode;    \
                      j[RESPONSE_TEXT] = mand.ResponseText;    \
                      return mand.replyTo + " " + to_string(j) +"\n";



// Define a custom exception class inheriting from std::runtime_error
class JsonError : public std::runtime_error
{
public:

    explicit JsonError(const char* name, const char* message)
        : std::runtime_error(std::string(name) + ": " + std::string(message)) {}

    // Constructor that takes a std::string message
    explicit JsonError(const char* name, const std::string& message)
        : std::runtime_error(std::string(name) + ": " + message) {}
};




json getNumber(json& j, const char* name)
{
    json value = j[name];
    if (value.is_null())
        throw JsonError(name, "field not found");
    if (!value.is_number())
        throw JsonError(name, "not number");
    return value;
}


std::string getString(json& j, const char* name)
{
    json value = j[name];
    if (value.is_null())
        throw JsonError(name, "field not found");
    if (!value.is_string())
        throw JsonError(name, "not string");
    return value;
}


json getField(json& j, const char* name)
{
    json value = j[name];
    if (value.is_null())
        throw JsonError(name, "field not found");
    return value;
}


//=============================================================
// MsgBase routines
//==========================================================

//==========================================================
// Fetches a string from the local json object
//==========================================================
std::string ReqMsgBase::jsonGetString(const char* name)
{
    json value = j_[name];
    if (value.is_null())
        throw JsonError(name, "field not found");
    if (!value.is_string())
        throw JsonError(name, "not string");
    return value;
}
//==========================================================




//==========================================================
// Fetches a number from the local json object
//==========================================================
json ReqMsgBase::jsonGetNumber(const char* name)
{
    json value = j_[name];
    if (value.is_null())
        throw JsonError(name, "field not found");
    if (!value.is_number())
        throw JsonError(name, "not number");
    return value;
}
//==========================================================





//=============================================================
// Constructor - Fills in mandatory fields from the json
//=============================================================
ReqMsgBase::ReqMsgBase(json& j) : j_(j)
{
    mand_.ContextId     = getString(j_, "ContextId"    );
    mand_.FutureReplyTo = getString(j_, "FutureReplyTo");
}
//=============================================================


//=============================================================
// Constructor - Fills in mandatory fields from the json
//=============================================================
RspMsgBase::RspMsgBase(json& j) : j_(j)
{
    mand_.replyTo       = jsonGetString("ImmediateReplyTo");
    mand_.ContextId     = jsonGetString("ContextId"       );
    mand_.ResponseText  = "";
    mand_.ResponseCode  = 0;
    mand_.MessageType   = jsonGetString("MessageType") + "Rsp";
}
//=============================================================


//=============================================================
// This adds our mandatory response fields and returns
// the "ImmediateReplyTo" followed by a space followed by the
// serialized JSON text.
//=============================================================
std::string RspMsgBase::encodeJson(json& j)
{
    j["MessageType" ] = mand_.MessageType;
    j["ContextId"   ] = mand_.ContextId;
    j["ResponseCode"] = mand_.ResponseCode;
    j["ResponseText"] = mand_.ResponseText;
    return mand_.replyTo + " " + to_string(j) +"\n";
}
//=============================================================

//==========================================================
// Fetches a string from the local json object
//==========================================================
std::string RspMsgBase::jsonGetString(const char* name)
{
    json value = j_[name];
    if (value.is_null())
        throw JsonError(name, "field not found");
    if (!value.is_string())
        throw JsonError(name, "not string");
    return value;
}
//==========================================================




//=============================================================
// End MsgBase routines
//==========================================================


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
    printf("%s\n-----------------\n", msg);

    // Parse the JSON text of the received message
    try
    {
        // Parse the JSON text into a json object
        json j = json::parse(msg);

        // Fetch the message type
        std::string messageType = getField(j, "MessageType");

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
