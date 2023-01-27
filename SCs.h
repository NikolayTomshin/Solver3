#include "HardwareSerial.h"
#pragma once
#include <stdint.h>
#include "Vec.h"
#include "CsT.h"

struct SCs {
  uint32_t Link;
  uint8_t Basis;
  SCs(uint8_t i, uint8_t j, uint8_t k, uint8_t ii=0, uint8_t ij=0, uint8_t ik=0, uint8_t i_i=0, uint8_t i_j=0, uint8_t i_k=0) {
    Link = ii + (uint32_t(ij) << 5) + (uint32_t(ik) << 10) + (uint32_t(i_i) << 15) + (uint32_t(i_j) << 20) + (uint32_t(i_k) << 25);
    Basis = i + 6 * j + 36 * k;
  }
  CsT CSdecode() {  //returns CsT with ovec indexes of this orientation
    CsT cs;
    uint8_t t = Basis;
    cs.ON[2] = t / 36;
    t = t % 36;
    cs.ON[1] = t / 6;
    cs.ON[0] = t % 6;
    return (cs);
  }
  uint8_t GetLink(uint8_t v) {  //Returns Index of rotated SCs
        return ((Link>>(v*5)) & (0b11111));
  }
  // void PrintPars() {
  //   Serial.print("(");  //(Cs(i,j,k),)
  //   Serial.print(Basis%6);
  //   Serial.print(",");
  //   Serial.print(Basis%36/6);
  //   Serial.print(",");
  //   Serial.print(Basis/36);
  //   Serial.print(",");
  //   Serial.print(GetLink(0));
  //   Serial.print(",");
  //   Serial.print(GetLink(1));
  //   Serial.print(",");
  //   Serial.print(GetLink(2));
  //   Serial.print(",");
  //   Serial.print(GetLink(3));
  //   Serial.print(",");
  //   Serial.print(GetLink(4));
  //   Serial.print(",");
  //   Serial.print(GetLink(5));
  //   Serial.print(")");
  // }
};

namespace SCS {
  uint8_t ET(uint8_t i){//moves to good
if ((i>7)&&(i<15)) return 2;
switch(i){
case 0: return 0;
default: return 1;
case 2: return 3;
case 5: return 3;
case 16: return 2;
case 17: return 2;
case 18: return 3;
case 19: return 2;
case 20: return 3;
case 21: return 3;
case 22: return 2;
case 23: return 2;
}
}
  const SCs Space[24]={SCs(0,1,2,1,2,3,4,5,6),
SCs(0,5,1,7,8,9,0,10,11),
SCs(2,1,3,11,12,8,13,0,14),
SCs(4,0,2,8,13,15,16,9,0),
SCs(0,2,4,0,14,13,7,16,17),
SCs(5,1,0,9,0,16,17,12,10),
SCs(1,3,2,10,11,0,14,17,15),
SCs(0,4,5,4,18,19,1,20,21),
SCs(2,0,1,21,22,18,3,1,2),
SCs(4,5,0,18,3,23,5,19,1),
SCs(5,3,1,19,1,5,6,22,20),
SCs(1,5,3,20,21,1,2,6,23),
SCs(3,1,5,23,5,21,22,2,19),
SCs(4,2,3,2,19,22,20,3,4),
SCs(2,3,4,6,23,2,19,4,18),
SCs(3,4,2,22,20,6,23,18,3),
SCs(5,0,4,3,4,20,21,23,5),
SCs(1,2,0,5,6,4,18,21,22),
SCs(2,4,0,17,15,14,9,7,8),
SCs(4,3,5,14,9,12,10,13,7),
SCs(5,4,3,13,7,10,11,15,16),
SCs(1,0,5,16,17,7,8,11,12),
SCs(3,2,1,12,10,17,15,8,13),
SCs(3,5,4,15,16,11,12,14,9)};
}