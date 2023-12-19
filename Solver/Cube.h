#include <stdint.h>
#pragma once
#include "Path.h"
//file for dealing with virtual cube

namespace Cube {

uint8_t linearIndex(Vec v);

const uint8_t edgeLindex[12] = { 0, 1, 2, 3, 4, 5, 6, 7, 12, 13, 14, 15 };  //linear indexes of edge pieces

Vec unfoldLinIndex(uint8_t linindex);

extern Piece RealPieces[20];  //real cube pieces 20 подвижных деталек которые двигаются в системе куба

void resetRealPieces();  //each piece on it's default pos and def pos set as unfolded linear index

struct State {
  uint8_t scs[13];                        //compressed array of SCS indexes of pieces ordered by real array
  uint8_t coordinateArrangedIndexes[13];  //compressed array of real pieces indexes in order of 3d lineared coordinates
  inline static uint8_t cAIcopy[13];      //temp cAI copy for applying operations

  State();
  uint8_t getscs(uint8_t index);
  void setscs(uint8_t pieceIndex, uint8_t newSCsIndex);
  void showscs();                                        //print scs in string
  void setCAI(uint8_t pieceindex, uint8_t linearindex);  //задать ссылку индексом детали по линейным координатам
  void updateCAI();                                      //обновление массива индексов по координатам
  uint8_t getCAI(uint8_t index);                         //get CAI value(piece index) by linear index
  void showcai();                                        //print cai string
  uint8_t getscsByVec(Vec vec);                          //get scs index of piece by vector in this state
  static uint8_t getCAIcopy(uint8_t index);              //get CAI value(piece index) by linear index from copied array
  void applyOperation(Operation op);                     //modify state's scs and CAI according to operation(rotation of one side)
  void printSliced(bool piOrSc, CsT cubeCs = CsT());     //show scs kinda in 3d
  static uint8_t edgePenaltyByDefault(uint8_t pieceindex);
  uint8_t statePenalty();  //if 1; ep=0; (1+epbd)*lp[1..3]


};  //State
struct Color {
  uint8_t component[3];
  Color(uint8_t r, uint8_t g, uint8_t b) {
    component[0] = r;
    component[1] = g;
    component[2] = b;
  }
  Color(){};
  void print() {
    Serial.print("Col: \t");
    for (uint8_t i = 0; i < 3; i++) {
      Serial.print(component[i]);
      Serial.write('\t');
    }
    Serial.write('\n');
  }
  void printLetter() {
    float p = arSum<uint8_t>(component, 3) / 3.0;
    float props[3];
    for (uint8_t i = 0; i < 3; i++) {
      props[i] = (component[i] - 0.707106 * (component[Mod3(i + 1)] + component[Mod3(i + 2)])) / p;
    }
    for (uint8_t i = 0; i < 3; i++) {
      if (inMargin<float>(props[i], 0.75, 0.25))
        switch (i) {
          case 0:
            Serial.print("R");
            return;
          case 1:
            Serial.print("G");
            return;
          case 2:
            Serial.print("B");
            return;
        }
    }
    for (uint8_t i = 0; i < 3; i++) {
      if ((props[i] > 0) == (props[Mod3(i + 1)] > 0))
        switch (i) {
          case 0:
            Serial.print("Y");
            return;
          case 1:
            Serial.print("C");
            return;
          case 2:
            Serial.print("P");
            return;
        }
    }
    if (p > 40) {
      Serial.print("W");
      return;
    } else {
      Serial.print("D");
      return;
    }
  }
  uint16_t sum() {
    return uint16_t(component[0]) + uint16_t(component[1]) + uint16_t(component[2]);
  }
};

struct CubeArray {
  Color arr[54];
  CubeArray() {
    for (uint8_t i = 0; i < 54; i++) {
      arr[i] = Color(0, 0, 0);
    }
  }
  void copy(Color arrc[]) {
    for (uint8_t i = 0; i < 54; i++) {
      arr[i] = arrc[i];
    }
  }
  Color* getColor(Vec vector) {  //lindex for indexedTiles
                                 //vector around 3x3 (single component=2, other [-1;1] ) cube
    uint8_t axis;                //axis
    for (axis = 0; axis < 3; axis++) {
      if (abs(vector.c[axis]) == 2) {
        break;
      }
    }
    //6 sides each containing 9 tilesб 3 axis * 2 directions; 3 positions on second axis * 3 positions on third axis
    uint8_t index = 9 * (axis + 3 * (vector.c[axis] < 0)) + 3 * (vector.c[Mod3(axis + 1)] + 1) + (vector.c[Mod3(axis + 2)] + 1);
    Serial.print("Accessing");
    vector.Cords();
    Serial.print(" at\t");
    Serial.println(index);
    return &arr[index];
  }
  void print() {
    for (uint8_t ov = 0; ov < 6; ov++) {
      for (int8_t y = 1; y > -2; y--) {
        for (int8_t x = -1; x < 2; x++) {
          arr[9 * ov + 3 * (y + 1) + x + 1].printLetter();
        }
        Serial.print('\n');
      }
      Serial.print('\n');
    }
  }
  void printInitialization() {
    Serial.print("Color *testArr[]=new Color[54]{");
    for (uint8_t i = 0; i < 54; i++) {
      Serial.print("Color(");
      for (uint8_t j = 0; j < 3; j++) {
        Serial.print(arr[i].component[j]);
        if (j < 2)
          Serial.print(",");
      }
      Serial.print(")");
      if (i < 53)
        Serial.print(",\n");
    }
    Serial.print("};");
  }
};

}  //ns Cube
