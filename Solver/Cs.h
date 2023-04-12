#pragma once
#include <stdint.h>
//first file of math structures
//Introduction of ortoCoordinate Systems that doubles as integer vector

struct Cs {//actually 3 int8 vector base class for coordinate systems and value vectors
  int8_t ON[3]; //components are coordinates of 3dim vector or indexes of ortovectors of cs
  Cs(int8_t i = 0, int8_t j = 1, int8_t k = 2) {
    Set(i, j, k);
  }
  void Set(int8_t i, int8_t j, int8_t k) {
    ON[0] = i;
    ON[1] = j;
    ON[2] = k;
  }
  void print() {
    Serial.print("(");
    GetLetter(ON[0]);
    Serial.print(",");
    GetLetter(ON[1]);
    Serial.print(",");
    GetLetter(ON[2]);
    Serial.print(")");
  }
  void GetLetter(int8_t F) { //function for getting string of ortovector by index
    switch (F) {
      case 0:
        Serial.print("i");
        break;
      case 1:
        Serial.print("j");
        break;
      case 2:
        Serial.print("k");
        break;
      case 3:
        Serial.print("-i");
        break;
      case 4:
        Serial.print("-j");
        break;
      case 5:
        Serial.print("-k");
        break;
      default: Serial.print("X");
    }
  }
};
