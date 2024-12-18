#ifndef _PanelClient_h_
#define _PanelClient_h_

#include "Protocol.h"

#define USE_FAKE_SWITCH     0

class PanelClient;

class Switch {
public:
    Switch(int id, int pin, PanelClient* panel_client);
    void poll();

    int id() const { return id_; }
    int state() const { return state_; }

protected:
    int id_;
    int pin_;
    int state_ = 0;
    PanelClient* panel_client_ = nullptr;
};

#if USE_FAKE_SWITCH
class FakeSwitch : public Switch {
public:
    FakeSwitch(int id, int pin, PanelClient* panel_client) :
        Switch(id, pin, panel_client) {}
    void poll();
};
#endif

class LED {
public:
    LED(int id, int pin, PanelClient* panel_client);
    void setState(int state);

    int id() const { return id_; }
    int state() const { return state_; }
    
protected:
    int id_;
    int pin_;
    int state_ = 0;
};

// Max LED and Switch count is 8
constexpr int kLEDCount = 6;
constexpr int kSwitchCount = 3;
constexpr unsigned char kAllSwitchFlags = 0b111;

class PanelClient {
public:
    void addSwitch(Switch* s);
    void addLED(LED* led);

    void pollSwitches();
    void switchUpdated(int id, int state);

    void processMessage(const ProtocolMessage& message);
    bool getMessage(ProtocolMessage* message);

    bool isConnected() const { return connected_; }

private:
    bool connected_ = false;
    long last_helo_timestamp_ = 0;

    // If true, send periodic status reports
    bool send_status_reports_ = true;

    // True when in the middle of a status report
    bool in_status_report_ = false;

    // Time in milliseconds of last status report
    long last_status_update_ = 0;

    Switch* switches_[kSwitchCount] = { nullptr };
    LED* leds_[kLEDCount] = { nullptr };

    // Bits 0..7 correspond to the on/off state of switches 0..7
    unsigned char switch_status_ = 0;

    // Bits 0..7 indicate if the given switch state has been changed since
    // the last report.  A 1 bit means that a swich on/off message needs to
    // be generated for the given switch.
    unsigned char switch_updated_ = 0;
};

#endif