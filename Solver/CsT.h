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
  CsT(uint8_t i, uint8_t j, uint8_t k, uint8_t missingComponent)
    : Cs(i, j, k) {  //create CsT by 2 defining orts and choosen missingComponent calculated
    Vec missingVec = Ovecs[getComponent((missingComponent + 1) % 3)];  //get 2nd ort before missing
    missingVec.rotate(getComponent((missingComponent + 2) % 3), -1);   //rotate it around next ort CCwise and get missing ort
    setComponent(missingComponent, V::GetON(missingVec));              //assign missingComponent as ortonumber of this vector
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
  void rotate(uint8_t ov) {
    rotate(ov % 3, 1 - 2 * (ov / 3));
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
  void transform(Cs* cs) {
    for (uint8_t i; i < 3; i++) {
      Vec component = Ovecs[getComponent(i)];  //get Vec ortovector
      component.Transform(cs);                 //transform vector
      setComponent(i, V::GetON(component));    //set new Ov index
    }
  }
  void untransform(Cs* cs) {
    for (uint8_t i; i < 3; i++) {
      Vec component = Ovecs[getComponent(i)];  //get Vec ortovector
      component.Untransform(cs);               //untransform vector
      setComponent(i, V::GetON(component));    //set new Ov index
    }
  }
};