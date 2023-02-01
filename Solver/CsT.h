#include "HardwareSerial.h"
#pragma once
#include "Vec.h"

struct CsT : Cs {
  CsT(int8_t i = 0, int8_t j = 1, int8_t k = 2)
    : Cs(i, j, k) {}
  void Rotate(int8_t a, int8_t oa) {
    Vec v;
    for (int8_t i = 0; i < 3; i++) {
      // Serial.print("Rotating ");
      // Serial.print("i=");
      // Serial.print(i);
      // Serial.print("; Cs vec [");
      v = Ovecs[ON[i]];
      // Serial.print(ON[i]);
      // Serial.print("];");
      v.Rotate(a, oa);
      // Serial.print("Rotated [");
      ON[i] = V::GetON(v);
      // Serial.print(ON[i]);
      // Serial.println("]");
    }
  }
  bool Compare(Cs* cs) {  //Равенство с другой
    // Serial.print("Comparing");
    // Print();
    // Serial.print("to");
    // cs->Print();
    return ((cs->ON[0] == ON[0]) && (cs->ON[1] == ON[1]) && (cs->ON[2] == ON[2]));
  }
};