#pragma once
#include <stdint.h>

struct Cs {
  int8_t ON[3];
  Cs(int8_t i = 0, int8_t j = 1, int8_t k = 2) {
    Set(i, j, k);
  }
  void Set(int8_t i, int8_t j, int8_t k) {
    ON[0] = i;
    ON[1] = j;
    ON[2] = k;
  }
  void Print() {
    Serial.print("(");
    GetLetter(ON[0]);
    Serial.print(",");
    GetLetter(ON[1]);
    Serial.print(",");
    GetLetter(ON[2]);
    Serial.print(")");
  }
  void GetLetter(int8_t F) {
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
