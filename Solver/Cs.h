#pragma once
#include "Mathclasses.h"
#include "bitCoding.h"
//first file of math structures
//Introduction of Cs -structs that store 3 encoded heximal numbers representing ortovectors

struct Cs {  //actually 3 int8 vector base class for coordinate systems and value vectors
  uint8_t ortoIndexes[2];
  Cs(uint8_t i = 0, uint8_t j = 1, uint8_t k = 2) {
    Set(i, j, k);
  }
  void Set(uint8_t i, uint8_t j, uint8_t k) {
    bitCoding::writeBased(6, i, 0, ortoIndexes);
    bitCoding::writeBased(6, j, 1, ortoIndexes);
    bitCoding::writeBased(6, k, 2, ortoIndexes);
  }
  uint8_t getComponent(uint8_t index) {
    return (bitCoding::getBased(6, index, ortoIndexes));
  }
  void setComponent(uint8_t index, uint8_t value) {
    bitCoding::writeBased(6, value, index, ortoIndexes);
  }
  void printLetters() {
    Serial.print("(");
    printLetter(getComponent(0));
    Serial.print(",");
    printLetter(getComponent(1));
    Serial.print(",");
    printLetter(getComponent(2));
    Serial.print(")");
  }
  void print() {
    Serial.print("(");
    printLetter(getComponent(0));
    Serial.print(",");
    printLetter(getComponent(1));
    Serial.print(",");
    printLetter(getComponent(2));
    Serial.print(")");
  }
  static void printLetter(int8_t F) {  //function for getting string of ortovector by index
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
