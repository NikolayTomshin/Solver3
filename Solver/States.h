#pragma once
#include <stdint.h>
#include "Awareness.h"
#include <avr/pgmspace.h>


const char com_0[] PROGMEM = "pre";  //
const char com_1[] PROGMEM = "beg";  //begin
const char com_2[] PROGMEM = "s##";  //servoSet
const char com_3[] PROGMEM = "r##";  //rotSet
const char* const comSet[] PROGMEM = { com_0, com_1, com_2, com_3 };
const uint8_t comSize PROGMEM = 4;
char pars[2];
char buffer[3];


void readCom() {
  while (Serial1.available() > 3) {
    while (Serial1.available())
      if (Serial1.peek() != '@')  //clear until @
        Serial1.read();
    //first symbol is @
    if (Serial1.available() > 3) {  //if
      Serial1.read();               //remove @
      for (uint8_t i = 0; i < 3; i++) {
        buffer[i] = Serial1.read();  //fill buffer
      }
      uint8_t parsIndex = 0;
      bool acc;
      for (uint8_t c = 0; c < pgm_read_byte(&comSize); c++) {  //try every mask(commands)
        acc = true;                                            //accepted by default
        char* _comSet = (char*)pgm_read_word(&(comSet[c]));    //select command address

        for (uint8_t i = 0; i < 3; i++) {  //check every symbol
          char mask = (char)pgm_read_byte(_comSet[i]);
          char com = buffer[i];
          if (mask == '#') {
            pars[parsIndex] = com;
            parsIndex++;
          } else {
            acc = mask == com;
            if (!acc) break;
          }
        }
        if (acc) {
          robotState->exe(c);
          return;
        }
      }
    }
  }
}
class StandBy : public IRState {
  void update() {
    readCom();
  }
  void exe(uint8_t c) {
    switch (c) {
      case 0:

        break;
      case 1:

        break;
      case 2:

        break;
      case 3:

        break;
      default:;
    }
  }
};

