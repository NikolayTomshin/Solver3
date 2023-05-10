#pragma once
#include <stdint.h>
#include "SCs.h"
//file for dealing with pieces of virtual cube (8)


struct Piece {       //real cube piece
  Vec defPos;        //position in solved cube (cube Cs)
  uint8_t scsIndex;  //orientation
  Piece() {}
  Piece(Vec defpos, uint8_t scsindex) {
    defPos = defpos;
    scsIndex = scsindex;
  }
};

struct Operation {
  uint8_t ortoVector;  //vector pointing to side being rotated from the center
  int8_t ortoAngle;    //ortoangle
  Operation(uint8_t value) {
    ortoVector = value / 3;
    ortoAngle = value % 3 + 1;
  }
  set(uint8_t oV, int8_t oA) {
    ortoVector = oV % 6;
    ortoAngle = oA % 4;
  }
  Operation(uint8_t oV, int8_t oA) {
    set(oV, oA);
  }
  void reverse() {
    ortoAngle = -ortoAngle;
  }
  uint8_t getEncoded() {
    return (ortoVector * 3 + Mod(4, ortoAngle) - 1);
  }
  static char opLetter(uint8_t ov) {
    switch (ov) {
      case 0: return ('F');
      case 1: return ('R');
      case 2: return ('U');
      case 3: return ('B');
      case 4: return ('L');
      case 5: return ('D');
    }
  }
  void print() {
    Serial.print("[");
    uint8_t oaMod = Mod(4, ortoAngle);
    if (oaMod) {  //if non zero
      Serial.print(opLetter(ortoVector));
      switch (oaMod) {
        case 1:
          Serial.print("'");
          break;
        case 2:
          Serial.print(2);
          break;
        default: break;
      }
    }
    Serial.print("]");
  }
};

namespace SCS {

uint8_t getPostOpearationIndex(uint8_t originIndex, Operation op);

}  //ns SCS