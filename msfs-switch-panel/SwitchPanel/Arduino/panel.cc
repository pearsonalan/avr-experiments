#include <Arduino.h>
#include <WString.h>

#include "LogLevel.h"
#include "Protocol.h"
#include "PanelClient.h"

using string = String;

Protocol protocol;
PanelClient panel_client;

Switch switch0(0, 5, &panel_client);
Switch switch1(1, 6, &panel_client);
Switch switch2(2, 7, &panel_client);

LED led0(0, 8, &panel_client);
LED led1(1, 9, &panel_client);
LED led2(2, 10, &panel_client);
LED led3(3, 11, &panel_client);
LED led4(4, 12, &panel_client);

int verbose_ = 0;

// Milliseconds between polling the switches
constexpr long kSwitchPollInterval = 50;
long last_switch_poll_time = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) {}
}

void loop() {
  long now = millis();

  // Process any incoming bytes
  while (Serial.available() > 0) {
    // read the incoming byte:
    char incomingByte = Serial.read();

    // Output what was read
    if (LOG_LEVEL(2)) {
      Serial.print("# I received: ");
      Serial.println(incomingByte, DEC);
    }
    protocol.addBytes(&incomingByte, 1);
  }

  // process any messages
  while (protocol.hasMessage()) {
    ProtocolMessage message = protocol.getMessage();
    panel_client.processMessage(message);
  }

  // Poll switches if it is time to...
  if (now - last_switch_poll_time >= kSwitchPollInterval) {
    if (LOG_LEVEL(2)) {
      Serial.print("# Polling switches at ");
      Serial.println(now);
    }
    panel_client.pollSwitches();
    last_switch_poll_time = now;
  }

  ProtocolMessage message;
  while (panel_client.getMessage(&message)) {
      string s = message.toString();
      Serial.println(s);
  } 
}
