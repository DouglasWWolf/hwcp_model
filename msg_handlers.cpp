#include "msg_structs.h"

//=============================================================================
// This just plugs the 32-bit value from the request into the response
//=============================================================================
void MsgPing::handle(MsgPingRsp& rsp)
{
    rsp.value = req.value;
}
//=============================================================================
