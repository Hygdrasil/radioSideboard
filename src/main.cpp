#include "Arduino.h"
//#include <WiFi.h>

//#include <huebridge.hpp>

#include "pin_config.hpp"
#include "private_settings.hpp" //BRIDGE_IP, SSID, PASSWORD



//HueBridge bridge{BRIDGE_IP};
void setup() {
    Serial.printf("starting\n");
    initButtons();
    Serial.printf("staretd\n");
}

void loop() {
    delay(1);
    testButtons();
    testPoti();
}