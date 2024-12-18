#include <iostream>

#include "Protocol.h"

#define BEGIN_TEST() bool test_ok__ = true

#define EXPECT(x) if (!(x)) { \
    std::cerr << "Failed expectation !(" << #x << ") in " << __FUNCTION__ << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
    test_ok__ = false; \
}

#define ASSERT(x) if (!(x)) { \
    std::cerr << "Failed assertion !(" << #x << ") in " << __FUNCTION__ << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
    exit(-1); \
}

#define END_TEST() { \
   std::cerr << __FUNCTION__  << ": " << (test_ok__ ? "OK" : "FAILED") << std::endl; \
}

void TestMessageToString() {
    BEGIN_TEST();
    EXPECT(ProtocolMessage().toString() == "");
    EXPECT(ProtocolMessage(MessageType::Hello).toString() == "helo");
    EXPECT(ProtocolMessage(MessageType::SwitchOn, 1).toString() == "sw11");
    EXPECT(ProtocolMessage(MessageType::SwitchOff, 4).toString() == "sw40");
    EXPECT(ProtocolMessage(MessageType::LedOn, 0).toString() == "ld01");
    EXPECT(ProtocolMessage(MessageType::LedOff, 0).toString() == "ld00");
    EXPECT(ProtocolMessage(MessageType::LogLevel, 0, 1).toString() == "lvl1");
    EXPECT(ProtocolMessage(MessageType::StatusReportEnable, 0, 1).toString() == "sre1");
    END_TEST();
}

void TestNoMessage() {
    BEGIN_TEST();
    Protocol protocol;

    EXPECT(!protocol.hasMessage());
    ProtocolMessage message = protocol.getMessage();

    EXPECT(message.message_type() == MessageType::Invalid);
    END_TEST();
}

void TestHasMessage() {
    BEGIN_TEST();
    Protocol protocol;

    EXPECT(!protocol.hasMessage());
    protocol.addBytes("sw", 2);
    EXPECT(!protocol.hasMessage());
    protocol.addBytes("01", 2);
    EXPECT(!protocol.hasMessage());
    protocol.addBytes("\n", 1);
    EXPECT(protocol.hasMessage());

    END_TEST();
}

void TestParseMessage() {
    BEGIN_TEST();
    Protocol protocol;
    ProtocolResult result;
    ProtocolMessage message;


    result = protocol.addBytes("sw00\n", 5);
    EXPECT(result == ProtocolResult::Ok);
    EXPECT(protocol.hasMessage());
    message = protocol.getMessage();
    EXPECT(message.message_type() == MessageType::SwitchOff);
    EXPECT(message.component() == 0);
    EXPECT(!protocol.hasMessage());

    result = protocol.addBytes("lvl1\n", 5);
    EXPECT(result == ProtocolResult::Ok);
    EXPECT(protocol.hasMessage());
    message = protocol.getMessage();
    EXPECT(message.message_type() == MessageType::LogLevel);
    EXPECT(message.component() == 0);
    EXPECT(message.arg() == 1);
    EXPECT(!protocol.hasMessage());

    result = protocol.addBytes("sre0\n", 5);
    EXPECT(result == ProtocolResult::Ok);
    EXPECT(protocol.hasMessage());
    message = protocol.getMessage();
    EXPECT(message.message_type() == MessageType::StatusReportEnable);
    EXPECT(message.component() == 0);
    EXPECT(message.arg() == 0);
    EXPECT(!protocol.hasMessage());

    result = protocol.addBytes("sre1\n", 5);
    EXPECT(result == ProtocolResult::Ok);
    EXPECT(protocol.hasMessage());
    message = protocol.getMessage();
    EXPECT(message.message_type() == MessageType::StatusReportEnable);
    EXPECT(message.component() == 0);
    EXPECT(message.arg() == 1);
    EXPECT(!protocol.hasMessage());

    END_TEST();
}

// Same as above test but use \r to end message.
void TestParseMessageCREnd() {
    BEGIN_TEST();
    Protocol protocol;

    ProtocolResult result = protocol.addBytes("sw00\r", 5);
    EXPECT(result == ProtocolResult::Ok);
    EXPECT(protocol.hasMessage());
    ProtocolMessage message = protocol.getMessage();

    EXPECT(message.message_type() == MessageType::SwitchOff);
    EXPECT(message.component() == 0);

    EXPECT(!protocol.hasMessage());

    END_TEST();
}

void TestPartialMessages() {
    BEGIN_TEST();
    Protocol protocol;
    ProtocolResult result;
    ProtocolMessage message;

    result = protocol.addBytes("hel", 3);
    EXPECT(result == ProtocolResult::Ok);
    EXPECT(!protocol.hasMessage());

    result = protocol.addBytes("o\n", 2);
    EXPECT(result == ProtocolResult::Ok);
    EXPECT(protocol.hasMessage());

    message = protocol.getMessage();
    EXPECT(message.message_type() == MessageType::Hello);
    EXPECT(!protocol.hasMessage());

    result = protocol.addBytes("sw2", 3);
    EXPECT(result == ProtocolResult::Ok);
    EXPECT(!protocol.hasMessage());

    result = protocol.addBytes("1\nsw20\n", 7);
    EXPECT(result == ProtocolResult::Ok);
    EXPECT(protocol.hasMessage());

    message = protocol.getMessage();
    EXPECT(message.message_type() == MessageType::SwitchOn);
    EXPECT(message.component() == 2);
    EXPECT(protocol.hasMessage());

    message = protocol.getMessage();
    EXPECT(message.message_type() == MessageType::SwitchOff);
    EXPECT(message.component() == 2);
    EXPECT(!protocol.hasMessage());

    END_TEST();
}

void TestMultipleMessages() {
    BEGIN_TEST();
    Protocol protocol;
    ProtocolResult result;
    ProtocolMessage message;

    result = protocol.addBytes("helo\nsw11\rld91\rlvl2\n", 20);
    EXPECT(result == ProtocolResult::Ok);
    EXPECT(protocol.hasMessage());

    message = protocol.getMessage();
    EXPECT(message.message_type() == MessageType::Hello);
    EXPECT(protocol.hasMessage());

    message = protocol.getMessage();
    EXPECT(message.message_type() == MessageType::SwitchOn);
    EXPECT(message.component() == 1);
    EXPECT(protocol.hasMessage());

    message = protocol.getMessage();
    EXPECT(message.message_type() == MessageType::LedOn);
    EXPECT(message.component() == 9);

    message = protocol.getMessage();
    EXPECT(message.message_type() == MessageType::LogLevel);
    EXPECT(message.arg() == 2);

    EXPECT(!protocol.hasMessage());

    END_TEST();
}

void TestBufferOverflow() {
    BEGIN_TEST();
    Protocol protocol;
    ProtocolResult result;
    ProtocolMessage message;

    result = protocol.addBytes("abcdefghijklmnopqrstuvwxyzabcd", 30);
    EXPECT(result == ProtocolResult::Ok);
    EXPECT(protocol.inputBufferLength() == 30);
    EXPECT(!protocol.hasMessage());

    result = protocol.addBytes("abcdefghijklmnopqrstuvwxyz\n", 27);
    EXPECT(result == ProtocolResult::BufferFull);
    EXPECT(protocol.inputBufferLength() == 32);
    EXPECT(protocol.hasMessage());
    message = protocol.getMessage();
    EXPECT(message.message_type() == MessageType::Invalid);
    EXPECT(protocol.inputBufferLength() == 0);

    result = protocol.addBytes("abcdefghijklmnopqrstuvwxyzabcdefghij\n", 37);
    EXPECT(result == ProtocolResult::BufferFull);
    EXPECT(protocol.inputBufferLength() == 32);
    EXPECT(protocol.hasMessage());
    message = protocol.getMessage();
    EXPECT(message.message_type() == MessageType::Invalid);
    EXPECT(protocol.inputBufferLength() == 0);

    protocol.addBytes("lvl0\n", 5);
    EXPECT(protocol.hasMessage());
    message = protocol.getMessage();
    EXPECT(message.message_type() == MessageType::LogLevel);
    EXPECT(message.arg() == 0);

    EXPECT(!protocol.hasMessage());
    EXPECT(protocol.inputBufferLength() == 0);

    END_TEST();
}

int main() {
    TestMessageToString();
    TestNoMessage();
    TestHasMessage(); 
    TestParseMessage();
    TestParseMessageCREnd();
    TestPartialMessages();
    TestMultipleMessages();
    TestBufferOverflow();
}