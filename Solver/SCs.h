#include <stdint.h>
#pragma once
#include "CsT.h"
//Super Coordinate System - way of fast cs rotation without actually doing it
//How it works: SCS.Space -array of all possible rotations of normal basis in space. All 24 Cs can be returned by value by .CSdecod from SCS
//function .GetLink(uint8_t v) returns index of SCs that is same as THIS Cs rotated clockwise by ortovector by index v
//counterclockwise rotation is same as rotation clockwise by negative vector, calculate it's index somewhere else
//so, instead of rotating every vector of cs, we store index of SCs and see which SCs it's pointing to


struct SCs {                //structure for orientation space storage
  uint8_t link[4] = { 0 };  //bitcoded links
  CsT basis;                //Straightforward CsT as value
  SCs() {}
  SCs(CsT cs) {
    basis = cs;
  }
  SCs(CsT cs, uint8_t i, uint8_t j, uint8_t k, uint8_t _i, uint8_t _j, uint8_t _k)
    : SCs(cs) {
    setLink(0, i);
    setLink(1, j);
    setLink(2, k);
    setLink(3, _i);
    setLink(4, _j);
    setLink(5, _k);
  }
  getLink(uint8_t ov) {
    return (bitCoding::getBased(24, ov, link));
  }
  setLink(uint8_t ov, uint8_t sindex) {
    bitCoding::writeBased(24, sindex, ov, link);
  }
};

namespace SCS {
uint8_t ET(uint8_t i) {  //returns number of orto rotations to origin orientations (i,j,k) by SCs index
  if ((i > 7) && (i < 15)) return 2;
  switch (i) {
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
SCs Space[24] = { SCs(CsT(0, 1, 2), 1, 2, 3, 4, 5, 6),
                  SCs(CsT(0, 2, 4), 7, 8, 9, 0, 10, 11),
                  SCs(CsT(5, 1, 0), 9, 12, 13, 14, 0, 8),
                  SCs(CsT(1, 3, 2), 10, 9, 15, 13, 16, 0),
                  SCs(CsT(0, 5, 1), 0, 13, 16, 7, 17, 14),
                  SCs(CsT(2, 1, 3), 11, 0, 10, 16, 12, 17),
                  SCs(CsT(4, 0, 2), 8, 14, 0, 17, 11, 15),
                  SCs(CsT(0, 4, 5), 4, 18, 19, 1, 20, 21),
                  SCs(CsT(5, 0, 4), 19, 22, 2, 6, 1, 18),
                  SCs(CsT(1, 2, 0), 20, 19, 23, 2, 3, 1),
                  SCs(CsT(2, 3, 4), 21, 1, 20, 3, 22, 5),
                  SCs(CsT(4, 2, 3), 18, 6, 1, 5, 21, 23),
                  SCs(CsT(3, 1, 5), 23, 5, 21, 22, 2, 19),
                  SCs(CsT(5, 3, 1), 3, 23, 18, 21, 4, 2),
                  SCs(CsT(4, 5, 0), 2, 21, 4, 20, 6, 22),
                  SCs(CsT(3, 4, 2), 22, 20, 6, 23, 18, 3),
                  SCs(CsT(1, 5, 3), 5, 3, 22, 18, 19, 4),
                  SCs(CsT(2, 0, 1), 6, 4, 5, 19, 23, 20),
                  SCs(CsT(5, 4, 3), 16, 15, 8, 11, 7, 13),
                  SCs(CsT(1, 0, 5), 17, 16, 12, 8, 9, 7),
                  SCs(CsT(2, 4, 0), 14, 7, 17, 9, 15, 10),
                  SCs(CsT(4, 3, 5), 13, 11, 7, 10, 14, 12),
                  SCs(CsT(3, 5, 4), 12, 10, 14, 15, 8, 16),
                  SCs(CsT(3, 2, 1), 15, 17, 11, 12, 13, 9) };  //map of all real ortogonal orientations
CsT getCsT(uint8_t index) {
  return (Space[index].basis);
}

CsT tempCsT;

void transform(Vec* target, uint8_t sCSIndex) {
  target->Transform(&Space[sCSIndex].basis);
}
void untransform(Vec* target, uint8_t sCSIndex) {
  target->Untransform(&Space[sCSIndex].basis);
}

void generateSCs() {                         //generate code fore Space Loading
  SCs arr[24];                               //empty space
  arr[0] = (CsT());                          //first element - default i,j,k basis
  uint8_t size = 1;                          //define size of space=1
  CsT tempCs;                                //declare temp cs
  for (uint8_t sci = 0; sci < 24; sci++) {   //foreach of 24 SCs
    for (uint8_t scl = 0; scl < 6; scl++) {  //foreach of 6 ov ops/links
      tempCs = arr[sci].basis;
      tempCs.rotate(scl);  //tempCs is rotated Cs
      bool found = false;
      for (uint8_t i = 0; i < size; i++) {    //search space for equal
        if (tempCs.Compare(&arr[i].basis)) {  //if found
          found = true;
          arr[sci].setLink(scl, i);  //write link
          break;
        }
      }
      if (!found) {               // if not found
        arr[size] = SCs(tempCs);  //create new element of this CS
        arr[sci].setLink(scl, size);
        size++;  //inc size
      }
    }  //scl loop
    SCs* p = &arr[sci];
    Serial.print("SCs(CsT(");
    for (uint8_t i = 0; i < 3; i++) {
      Serial.print(p->basis.getComponent(i));
      if (i < 2) {
        Serial.print(",");
      }
    }
    Serial.print("),");
    for (uint8_t i = 0; i < 6; i++) {
      Serial.print(p->getLink(i));
      if (i < 5) {
        Serial.print(",");
      }
    }
    Serial.print(")");
    if (sci < 23) {
      Serial.println(",");
    }
  }  //sci loop
}
void loadSCs() {  //load prepped SCs to Space
}
}