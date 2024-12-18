#include <string.h>

#ifndef ARDUINO
#include <memory.h>
#endif

#include "Protocol.h"

constexpr char kHelloMessage[] = "helo";
constexpr char kStatusBeginMessage[] = "stat";
constexpr char kStatusEndMessage[] = "send";
constexpr char kSwitchMessage[] = "sw";
constexpr char kLedMessage[] = "ld";
constexpr char kLogLevelMessage[] = "lvl";
constexpr char kStatusReportEnableMessage[] = "sre";

string ProtocolMessage::toString() const {
    char buf[4];
    switch (message_type_) {
    case MessageType::Hello:
        return kHelloMessage;    
    case MessageType::StatusBegin:
        return kStatusBeginMessage;
    case MessageType::StatusEnd:
        return kStatusEndMessage;
    case MessageType::SwitchOn:
        buf[0] = '0' + component_;
        buf[1] = 0;
        return string(kSwitchMessage) + string(buf) + "1";
    case MessageType::SwitchOff:
        buf[0] = '0' + component_;
        buf[1] = 0;
        return string(kSwitchMessage) + string(buf) + "0";
    case MessageType::LedOn:
        buf[0] = '0' + component_;
        buf[1] = 0;
        return string(kLedMessage) + string(buf) + "1";
    case MessageType::LedOff:
        buf[0] = '0' + component_;
        buf[1] = 0;
        return string(kLedMessage) + string(buf) + "0";
    case MessageType::LogLevel:
        buf[0] = '0' + arg_;
        buf[1] = 0;
        return string(kLogLevelMessage) + string(buf);
    case MessageType::StatusReportEnable:
        buf[0] = '0' + arg_;
        buf[1] = 0;
        return string(kStatusReportEnableMessage) + string(buf);
    default:
        return string();
    }
}

inline bool isEndChar(char c) {
    return c == '\n' || c == '\r';
}

ProtocolResult Protocol::addBytes(const char* data, int len) {
    bool discarded_input = false;

    for (int i = 0; i < len; i++) {
        if (input_buffer_len_ < kProtocolInputBufferLen - 1) {
            input_buffer_[input_buffer_len_++] = data[i];
        } else if (isEndChar(data[i]) && input_buffer_len_ == kProtocolInputBufferLen - 1) {
            input_buffer_[input_buffer_len_++] = data[i];
        } else {
            discarded_input = true;
        }
    }
    return discarded_input ? ProtocolResult::BufferFull : ProtocolResult::Ok;
}

char* Protocol::findMessageEnd() {
    // Look for a \n or \r character which marks the end of the message
    char* message_end = nullptr;
    for (int i = 0; i < input_buffer_len_; i++) {
        if (input_buffer_[i] == '\n' || input_buffer_[i] == '\r') {
            message_end = input_buffer_ + i;
            break;
        }
    }
    return message_end;
}

bool Protocol::hasMessage() {
    return findMessageEnd() != nullptr;
}

ProtocolMessage Protocol::getMessage() {
    ProtocolMessage message;

    char* message_end = findMessageEnd();
    if (message_end != nullptr) {
        // Calculate the length of the message. This will not include the END character.
        int message_len = message_end - input_buffer_;
        message = parseMessage(message_len);
        // Discard what was parsed out of the buffer (including the END character)
        discardMessage(message_len + 1);
    }

    return message;
}

ProtocolMessage Protocol::parseMessage(int len) {
    ProtocolMessage message;
    
    // std::cout << "Parsing message of length " << len << std::endl;
    if (len == 4 && memcmp(input_buffer_, kHelloMessage, 4) == 0) {
        message = ProtocolMessage(MessageType::Hello);
    }
    else if (len == 4 &&
        memcmp(input_buffer_, kSwitchMessage, 2) == 0 &&
        isdigit(input_buffer_[2]) &&
        (input_buffer_[3] == '0' || input_buffer_[3] == '1')) {
        message = ProtocolMessage(
                      (input_buffer_[3] == '0') ? MessageType::SwitchOff : MessageType::SwitchOn,
                      input_buffer_[2] - '0');
    }
    else if (len == 4 &&
        memcmp(input_buffer_, kLedMessage, 2) == 0 &&
        isdigit(input_buffer_[2]) &&
        (input_buffer_[3] == '0' || input_buffer_[3] == '1')) {
        message = ProtocolMessage(
                      (input_buffer_[3] == '0') ? MessageType::LedOff : MessageType::LedOn,
                      input_buffer_[2] - '0');
    }
    else if (len == 4 &&
        memcmp(input_buffer_, kLogLevelMessage, 3) == 0 &&
        isdigit(input_buffer_[3])) {
        message = ProtocolMessage(MessageType::LogLevel, 0, input_buffer_[3] - '0');
    }
    else if (len == 4 &&
        memcmp(input_buffer_, kStatusReportEnableMessage, 3) == 0 &&
        isdigit(input_buffer_[3])) {
        message = ProtocolMessage(MessageType::StatusReportEnable, 0, input_buffer_[3] - '0');
    }
    else if (len == 4 && memcmp(input_buffer_, kStatusBeginMessage, 4) == 0) {
        message = ProtocolMessage(MessageType::StatusBegin);
    }
    else if (len == 4 && memcmp(input_buffer_, kStatusEndMessage, 4) == 0) {
        message = ProtocolMessage(MessageType::StatusEnd);
    }

    return message;
}

void Protocol::discardMessage(int len) {
    if (input_buffer_len_ > len) {
        memmove(input_buffer_, input_buffer_ + len, input_buffer_len_ - len);
    }
    input_buffer_len_ -= len;
}