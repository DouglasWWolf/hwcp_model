#pragma once
#include <stdint.h>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Mandatory fields for a request message
struct mandReq
{
    std::string ContextId;
    std::string ImmediateReplyTo;
    std::string FutureReplyTo;
};

// Mandatory fields for a response message
struct mandRsp
{
    std::string replyTo;
    std::string MessageType;
    std::string ContextId;
    uint32_t    ResponseCode;
    std::string ResponseText;
};

class ReqMsgBase
{
public:
    ReqMsgBase(json& j);
protected:
    std::string jsonGetString(const char* name); 
    json        jsonGetNumber(const char* name);       
    json&       j_;
    mandReq     mand_;
};


class RspMsgBase
{
public:
    RspMsgBase(json& j);
protected:
    std::string jsonGetString(const char* name); 
    std::string encodeJson(json& j);
    json&       j_;
    mandRsp     mand_;
};



struct MsgPingRsp  : public RspMsgBase
{
    MsgPingRsp(json& j) :  RspMsgBase(j) {};
    uint32_t     value;
    std::string  serialize();
};



struct MsgPing : public ReqMsgBase
{
    MsgPing(json& j) : ReqMsgBase(j) {}
    void   deserialize();
    void   handle(MsgPingRsp&);
    static std::string dispatch(json& j);    

    struct 
    {
        uint32_t    value;
    } req;
};


