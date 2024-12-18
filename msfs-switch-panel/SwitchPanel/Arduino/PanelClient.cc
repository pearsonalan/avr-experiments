#include <Arduino.h>
#include "LogLevel.h"
#include "PanelClient.h"

Switch::Switch(int id, int pin, PanelClient* panel_client) :
    id_(id), pin_(pin), state_(0), panel_client_(panel_client) {
    panel_client_->addSwitch(this);
    pinMode(pin, INPUT);
}

void Switch::poll() {
    int state = digitalRead(pin_);
    if (state_ != state) {
        state_ = state;
        panel_client_->switchUpdated(id_, state_);
    }
}

#if USE_FAKE_SWITCH
void FakeSwitch::poll() {
    long now = millis();
    if (random(1000) < 3) {
        if (LOG_LEVEL(1)) {
            Serial.print("# Fake switch: triggering switch stage chage at ");
            Serial.println(now);
        }
        state_ = !state_;
        panel_client_->switchUpdated(id_, state_);
    }
}
#endif

LED::LED(int id, int pin, PanelClient* panel_client) :
    id_(id), pin_(pin), state_(0) {
    panel_client->addLED(this);
    pinMode(pin, OUTPUT);
}

void LED::setState(int state) {
    if (LOG_LEVEL(1)) {
        Serial.print("# Setting state on LED ");
        Serial.print(id_);
        Serial.print(" to ");
        Serial.println(state);
    }

    state_ = state;
    digitalWrite(pin_, state_ ? HIGH : LOW);
}

void PanelClient::addSwitch(Switch* s) {
    if (s->id() < kSwitchCount) {
        switches_[s->id()] = s;
    }
}

void PanelClient::addLED(LED* led) {
    if (led->id() < kLEDCount) {
        leds_[led->id()] = led;
    }
}

void PanelClient::pollSwitches() {
    for (int i = 0; i < kSwitchCount; i++) {
        if (switches_[i] != nullptr) {
            switches_[i]->poll();
        }
    }
}

void PanelClient::switchUpdated(int id, int state) {
    // turn the corresponding bit on or off 
    if (state == 0) {
        switch_status_ = switch_status_ & ~(1 << id);
    } else {
        switch_status_ = switch_status_ | (1 << id);
    }

    // Mark the switch updated
    switch_updated_ = switch_updated_ | (1 << id);

    if (LOG_LEVEL(1)) {
        Serial.print("# SWITCH UPDATE: sw=");
        Serial.print(id);
        Serial.print(", state=");
        Serial.print(state);
        Serial.print(": switch_status=");
        Serial.print(switch_status_, 2);
        Serial.print(", switch_updated=");
        Serial.println(switch_updated_, 2);
    }
}

void PanelClient::processMessage(const ProtocolMessage& message) {
    int i;

    if (LOG_LEVEL(1)) {
        Serial.print("# Got message: ");
        if (message.message_type() == MessageType::Invalid) {
            Serial.println("INVALID");
        } else {
            Serial.println(message.toString());
        }
    }
    
    switch (message.message_type()) {
    case MessageType::Hello: 
        // Helo serves as a bit of a reset.
        connected_ = true;
        send_status_reports_ = true;
        last_status_update_ = 0;
        in_status_report_ = false;
        for (i =0; i < kLEDCount; i++) {
            if (leds_[i] != nullptr) {
                leds_[i]->setState(0);
            }
        }
        break;
    case MessageType::LedOn:
        if (message.component() >= 0 && message.component() < kLEDCount) {
            leds_[message.component()]->setState(1);
        }
        break;
    case MessageType::LedOff:
        if (message.component() >= 0 && message.component() < kLEDCount) {
            leds_[message.component()]->setState(0);
        }
        break;
    case MessageType::LogLevel:
        verbose_ = message.arg();
        if (LOG_LEVEL(1)) {
            Serial.print("# Set verbose=");
            Serial.println(message.arg());
        }
        break;
    case MessageType::StatusReportEnable:
        send_status_reports_ = bool(message.arg());
        if (LOG_LEVEL(1)) {
            Serial.print("# Setting status_report_enable=");
            Serial.println(send_status_reports_);
        }
        break;
    }
}

bool PanelClient::getMessage(ProtocolMessage* message) {
    long now = millis();
    if (connected_) {
        if (send_status_reports_  && now - last_status_update_ > 5000) {
            *message = ProtocolMessage(MessageType::StatusBegin);
            // Marking all switches as updated will cause a message to be send for every switch.
            switch_updated_ = kAllSwitchFlags;
            in_status_report_ = true;
            last_status_update_ = now;
            return true;
        }

        if ((switch_updated_ & kAllSwitchFlags) != 0) {
            for (int i = 0; i < kSwitchCount; i++) {
                if (switch_updated_ & (1 << i)) {
                    int switch_status = (switch_status_ & (1 << i)) >> i;
                    if (switch_status == 0) {
                        *message = ProtocolMessage(MessageType::SwitchOff, i);
                    } else {
                        *message = ProtocolMessage(MessageType::SwitchOn, i);
                    }

                    switch_updated_ = switch_updated_ & ~(1 << i);
                    return true;
                }
            }
        }

        if (in_status_report_ && switch_updated_ == 0) {
            // Got to the end of the report.
            in_status_report_ = false;
            *message = ProtocolMessage(MessageType::StatusEnd);
            return true;
        }
    } else {
        if (now - last_helo_timestamp_ > 5000) {
            // Time to say hello...
            *message = ProtocolMessage(MessageType::Hello);
            last_helo_timestamp_ = now;
            return true;
        }
    }
    return false;
}