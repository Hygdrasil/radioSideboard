#pragma once
#include <Arduino.h>
#include <map>

enum class ButtonPin {
   Speach = 12,
   On = 13,
   Stereo = 14,
   Phono = 27,
   Ukw = 5,
   Short = 18,
   Middle = 19,
   Tape = 23
};

const std::map<ButtonPin, const char*> allButtons = {
   {ButtonPin::Speach, "Sprache"},
   {ButtonPin::On, "An"},
   {ButtonPin::Stereo, "Stereo"},
   {ButtonPin::Phono, "Phone"},
   {ButtonPin::Ukw, "UKW"},
   {ButtonPin::Short, "Kurz"},
   {ButtonPin::Middle, "Mittel"},
   {ButtonPin::Tape, "Tonband"}
};

enum class PotiPin {
   Highs = 25,
   Bass = 26,
   Balance = 32,
   Volume = 33
};

const std::map<PotiPin, const char*> allPotis = {
   {PotiPin::Highs, "Höhen"},
   {PotiPin::Bass, "Bass"},
   {PotiPin::Balance, "Balance"},
   {PotiPin::Volume, "Lautstärke"}
};

bool readButton(ButtonPin button){
   const uint8_t pin = static_cast<std::underlying_type_t<ButtonPin>>(button);
   return digitalRead(pin);
}

void initButtons(){
   for(const auto& button: allButtons){
      const uint8_t pin = static_cast<std::underlying_type_t<ButtonPin>>(button.first);
      pinMode(pin, INPUT_PULLUP);
   }
}

uint16_t readPoti(PotiPin poti){
   const uint8_t pin = static_cast<std::underlying_type_t<PotiPin>>(poti);
   return analogRead(pin);
}

void testButtons(){
   for(const auto&  button: allButtons){
      Serial.printf("%s:\t%s", button.second, readButton(button.first)? "on": "off");
   }
}

void testPoti(){
   for(const auto& poti: allPotis){
      Serial.printf("%s:\t%s", poti.second, readPoti(poti.first)? "on": "off");
   }
}