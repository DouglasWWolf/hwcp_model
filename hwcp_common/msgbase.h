#pragma once
#include <stdint.h>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;


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

