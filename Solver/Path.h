#pragma once
#include <stdint.h>
#include "Nodes.h"

namespace path {

struct Branch {
  uint8_t rating[6];          //permutation(see bitcoding) of actions sorted by descending quality of result(first-best)
  uint8_t prevAndPresent[2];  //5 bit index of previous action and 4 bit index of last attempted action from rating
  Branch() {}
  Branch(uint8_t previous) {  //b
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
};  //branch

}  //ns path

