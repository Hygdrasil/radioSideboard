#pragma once
#include <Arduino.h>
#include <map>

enum class ButtonPin {
   Speach = 12,
   On = 13,
   Stereo = 14, //inverted
   Phono = 27,
   Ukw = 5, //inverted
   Short = 18, //inverted
   Middle = 19, //inverted
   Tape = 23  //inverted
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

const std::map<ButtonPin, bool> buttonInverted = {
   {ButtonPin::Speach, false},
   {ButtonPin::On, false},
   {ButtonPin::Stereo, true},
   {ButtonPin::Phono, false},
   {ButtonPin::Ukw, true},
   {ButtonPin::Short, true},
   {ButtonPin::Middle, true},
   {ButtonPin::Tape, true}
};
//all min at 4095 and max 0
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
   return digitalRead(pin) ^ buttonInverted.at(button);
}

void initButtons(){
   for(const auto& button: allButtons){
      const uint8_t pin = static_cast<std::underlying_type_t<ButtonPin>>(button.first);
      pinMode(pin, INPUT_PULLUP);
   }
}

uint16_t readPoti(PotiPin poti){
   const uint8_t pin = static_cast<std::underlying_type_t<PotiPin>>(poti);
   const uint16_t value = analogRead(pin);
   return 4095- value;
}

uint32_t readPotiAverage(PotiPin poti, uint8_t cycles=10){
   uint32_t average =0;
   if(cycles==0){
      return 0;
   }
   for(int i=0; i< cycles; i++){
      average+=readPoti(poti);
   }
   return average/cycles;
}

void testButtons(){
   for(const auto&  button: allButtons){
      Serial.printf("%s:\t%s\t", button.second, readButton(button.first)? "on": "off");
   }
   Serial.print('\n');
}

void testPoti(){
   for(const auto& poti: allPotis){
      Serial.printf("%s:\t%d\t", poti.second, readPoti(poti.first));
   }
   Serial.print('\n');
}