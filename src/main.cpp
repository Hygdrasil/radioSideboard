/* Blink Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include "sdkconfig.h"
#include <Arduino.h>

#include <WiFi.h>
#include <huebridge.hpp>
#include "pin_config.hpp"
#include "wifi_settings.h"
/* Can run 'make menuconfig' to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/

void onWifiDisconnect(WiFiEvent_t event, WiFiEventInfo_t info){
    printf("disconnected wifi\n");
    digitalWrite(CONFIG_BLINK_GPIO, false);
    ESP.restart();
}

void setup() {
    WiFi.disconnect(true);
    digitalWrite(CONFIG_BLINK_GPIO, false);
    Serial.begin(115200);

    WiFi.onEvent(onWifiDisconnect, SYSTEM_EVENT_STA_DISCONNECTED);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Establishing connection to WiFi..");
    }
    pinMode(CONFIG_BLINK_GPIO, OUTPUT);
    initButtons();

}

void loop() {
    
    delay(2000);
    digitalWrite(CONFIG_BLINK_GPIO, !digitalRead(CONFIG_BLINK_GPIO));

    static HueBridge bridge{"192.168.188.27"};
    bridge.setToken(BRIDGE_TOKEN);
    bool succeeded;
    static bool initializedBrightness = false;
    static BulbState state = bridge.getState(1, &succeeded);
    if(!succeeded){
        printf("Failed to fetch data from the bridge are ip and token correct?,\n");
    }
    if(!readButton(ButtonPin::Speach)){
        return;
    }
    const uint32_t max_timeout = 4*60*1000;
    const uint32_t potiValue = readPotiAverage(PotiPin::Balance);
    printf("poty value: %d\n", potiValue);
    uint32_t timeout = (max_timeout / 4095) *potiValue;

    state = bridge.getState(TOGGLE_BUILB, &succeeded); 
    if(state.isReachable && state.isOn){
        digitalWrite(CONFIG_BLINK_GPIO, true);
        printf("in switch timout=%d\n", timeout);
        static int oldBrightness;
        if(!initializedBrightness){
            oldBrightness =  state.brightness;
            initializedBrightness = true;
        }
        printf("0. %s\n",bridge.stateToJson(state).c_str());
        if(!bridge.setBrightness(TOGGLE_BUILB, oldBrightness-1)){ //if the brightness is high again some switched it on of
            return;
        }
        delay(timeout/3*2);
        state = bridge.getState(TOGGLE_BUILB, &succeeded);
        printf("1. %s\n",bridge.stateToJson(state).c_str());
        if(state.brightness != oldBrightness-1){
            printf("1.1 failed %d,%d, %d, %d, %d\n",state.brightness != oldBrightness-1, !state.isOn, !state.isReachable, !succeeded, oldBrightness);
            initializedBrightness = false;
            return;
        }
        if( !state.isOn || !state.isReachable || !succeeded){
            printf("1.2 failed %d,%d, %d, %d, %d\n",state.brightness != oldBrightness-1, !state.isOn, !state.isReachable, !succeeded, oldBrightness);
            return;
        }
        if(!bridge.setBrightness(TOGGLE_BUILB, oldBrightness/2)){ //brightness change == new timeout
            printf("1.3 failed \n");
            return;
        }
        delay(timeout/3);
        state = bridge.getState(TOGGLE_BUILB, &succeeded);
        printf("2. %s\n",bridge.stateToJson(state).c_str());
        if(!state.isOn || !state.isReachable || !succeeded){
            printf("%d,%d,%d,%d\n",state.brightness != oldBrightness/2, !state.isOn, !state.isReachable, !succeeded);
            return;
        }
        if(state.brightness != oldBrightness/2 ){
            initializedBrightness = false;
            printf("2.1 brightness mismatch");
            return;
        }
        state = bridge.getState(TOGGLE_BUILB, &succeeded); 
        printf("%s\n",bridge.stateToJson(state).c_str());
        if(succeeded && state.isReachable && state.isOn && state.brightness == oldBrightness/2){
            
            state.brightness = oldBrightness;
            bridge.setState(TOGGLE_BUILB, state);//in two steps, because if the bulb is off the brighness cannot be set
            bridge.setState(TOGGLE_BUILB, false);
        }
    }

//    testButtons();
//    testPoti();
//    delay(200);
}