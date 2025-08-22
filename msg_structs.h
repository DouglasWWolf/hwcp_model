#pragma once
#include "msgbase.h"


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


