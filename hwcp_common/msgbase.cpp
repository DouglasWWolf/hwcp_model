#include "msgbase.h"

//=============================================================================
// Fetches a string from the local json object
//=============================================================================
std::string ReqMsgBase::jsonGetString(const char* name)
{
    json value = j_[name];
    if (value.is_null())
        throw JsonError(name, "field not found");
    if (!value.is_string())
        throw JsonError(name, "not string");
    return value;
}
//=============================================================================




//=============================================================================
// Fetches a number from the local json object
//=============================================================================
json ReqMsgBase::jsonGetNumber(const char* name)
{
    json value = j_[name];
    if (value.is_null())
        throw JsonError(name, "field not found");
    if (!value.is_number())
        throw JsonError(name, "not number");
    return value;
}
//=============================================================================





//================================================================================
// Constructor - Fills in mandatory fields from the json
//================================================================================
ReqMsgBase::ReqMsgBase(json& j) : j_(j)
{
    mand_.ContextId     = jsonGetString("ContextId"    );
    mand_.FutureReplyTo = jsonGetString("FutureReplyTo");
}
//================================================================================


//================================================================================
// Constructor - Fills in mandatory fields from the json
//================================================================================
RspMsgBase::RspMsgBase(json& j) : j_(j)
{
    mand_.replyTo       = jsonGetString("ImmediateReplyTo");
    mand_.ContextId     = jsonGetString("ContextId"       );
    mand_.ResponseText  = "";
    mand_.ResponseCode  = 0;
    mand_.MessageType   = jsonGetString("MessageType") + "Rsp";
}
//================================================================================


//================================================================================
// This adds our mandatory response fields and returns
// the "ImmediateReplyTo" followed by a space followed by the
// serialized JSON text.
//================================================================================
std::string RspMsgBase::encodeJson(json& j)
{
    j["MessageType" ] = mand_.MessageType;
    j["ContextId"   ] = mand_.ContextId;
    j["ResponseCode"] = mand_.ResponseCode;
    j["ResponseText"] = mand_.ResponseText;
    return mand_.replyTo + " " + to_string(j) +"\n";
}
//================================================================================

//=============================================================================
// Fetches a string from the local json object
//=============================================================================
std::string RspMsgBase::jsonGetString(const char* name)
{
    json value = j_[name];
    if (value.is_null())
        throw JsonError(name, "field not found");
    if (!value.is_string())
        throw JsonError(name, "not string");
    return value;
}
//=============================================================================


