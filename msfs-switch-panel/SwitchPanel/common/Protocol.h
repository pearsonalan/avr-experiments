#ifndef _Protocol_h_
#define _Protocol_h_

#ifdef ARDUINO
#include <WString.h>
using string = String;
#else
#include <string>
using string = std::string;
#endif

enum class MessageType {
    Invalid = -1,
    Hello = 0,
    StatusBegin,
    StatusEnd,
    SwitchOn,
    SwitchOff,
    LedOn,
    LedOff,
    LogLevel,
    StatusReportEnable
};

enum class ProtocolResult {
    Ok = 0,
    BufferFull,
    NoMessageAvailable
};

class ProtocolMessage {
public:
    ProtocolMessage() {}
    ProtocolMessage(MessageType type) :
        message_type_(type) {}
    ProtocolMessage(MessageType type, int component) :
        message_type_(type), component_(component) {}
    ProtocolMessage(MessageType type, int component, int arg) :
        message_type_(type), component_(component), arg_(arg) {}

    MessageType message_type() const { return message_type_; }
    int component() const { return component_; }
    int arg() const { return arg_; }
    string toString() const;

private:
    MessageType message_type_ = MessageType::Invalid;
    int component_ = 0;
    int arg_ = 0;
};

constexpr int kProtocolInputBufferLen = 32;

class Protocol {
public:
    Protocol() {}

    // Add data received from the other side to the protocol input buffer
    ProtocolResult addBytes(const char* data, int len);

    // Returns true if the input buffer contains a full message
    bool hasMessage();

    ProtocolMessage getMessage();

    int inputBufferLength() const { return input_buffer_len_; }

private:
    char* findMessageEnd();
    ProtocolMessage parseMessage(int len);
    void discardMessage(int len);

    char input_buffer_[kProtocolInputBufferLen] = { 0 };
    int input_buffer_len_ = 0;
};

#endif
