#include <stdint.h>
#include "Piece.h"

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
  void print() {
    Serial.print(ortoVector);
    Serial.print(" ");
    Serial.print(ortoAngle);
  }
};
namespace path {


struct Branch {
  uint8_t rating[6];          //permutation of actions sorted by descending quality of result
  uint8_t prevAndPresent[2];  //5 bit index of previous action and 4 bit index of last attempted action from rating
  Branch() {}
  Branch(uint8_t previous) {
    setLastIndex(0);
    bitCoding::writeBits(0, 5, previous, prevAndPresent);
  }
  uint8_t getPreviousAction() {
    return (bitCoding::getBits(0, 5, prevAndPresent));
  }
  uint8_t getLastIndex() {
    return (bitCoding::getBits(5, 4, prevAndPresent));
  }
  void setLastIndex(uint8_t lI) {
    bitCoding::writeBits(5, 4, lI, prevAndPresent);
  }
  void increaseLastIndex() {
    setLastIndex(getLastIndex() + 1);
  }
  Operation getOperationFromRating(uint8_t index) {
    uint8_t ops[8];
    bitCoding::decodePermutation(15, rating, ops);
    uint8_t pOV = getPreviousAction() / 3;  //ортовектор предыдущего поворота.
    uint8_t operation = ops[index];
    if (pOV <= (operation / 3)) {  //если "запретный вектор" больше полученного, ничего, а если нет значит он пропускается и истинная операция на 3 единицы больше
      operation += 3;
    }
    return (Operation(uint8_t(operation)));
  }
};
}
namespace SCS {

uint8_t getPostOpearationIndex(uint8_t originIndex, Operation op) {
  switch (Mod(4, op.ortoAngle)) {
    default: return (originIndex);
    case 1:
      // Serial.println("CW 1oA");
      return (Space[originIndex].getLink(op.ortoVector));
    case 2:
      // Serial.println("2oA");
      return (Space[Space[originIndex].getLink(op.ortoVector)].getLink(op.ortoVector));
    case 3:
      // Serial.println("CCW 1oA");
      return (Space[originIndex].getLink((op.ortoVector + 3) % 6));
  }
}
}