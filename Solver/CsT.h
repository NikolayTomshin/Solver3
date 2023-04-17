#pragma once
#include "Vec.h"
//third math file describing Cs Transformable which are being used

struct CsT : public Cs {
  CsT(uint8_t ortovector, uint8_t ijkIndex) {  //create CsT around 1 fixed vector
    setComponent(ijkIndex, ortovector);        //set choosen vector as choosen vector
    for (uint8_t i = 0; i < 2; i++) {
      uint8_t next = V::GetNextOv(ortovector);
      if (ortovector < 3)  //next ortovector returns right if positive direction and left when negative
      {
        ijkIndex++;
      } else {
        ijkIndex--;
      }
      setComponent(Mod3(ijkIndex), next);
      ortovector = next;
    }
  }
  CsT(int8_t i = 0, int8_t j = 1, int8_t k = 2)  //default right basis
    : Cs(i, j, k) {}
  void rotate(int8_t axisOVIndex, int8_t rightAngles) {
    Vec v;
    for (int8_t i = 0; i < 3; i++) {
      // Serial.print("Rotating ");
      // Serial.print("i=");
      // Serial.print(i);
      // Serial.print("; Cs vec [");
      v = Ovecs[getComponent(i)];
      // Serial.print(ON[i]);
      // Serial.print("];");
      v.rotate(axisOVIndex, rightAngles);
      // Serial.print("Rotated [");
      setComponent(i, V::GetON(v));
      // Serial.print(ON[i]);
      // Serial.println("]");
    }
  }
  bool Compare(Cs* cs) {  //Равенство с другой
    // Serial.print("Comparing");
    // Print();
    // Serial.print("to");
    // cs->Print();
    for (uint8_t i = 0; i < 3; i++) {
      if (getComponent(i) != cs->getComponent(i)) return (false);
    }
    return (true);
  }
};