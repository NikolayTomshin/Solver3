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
  SCs(CsT cs, uint8_t _link[6]) {
    basis = cs;
    for (uint8_t ov = 0; ov < 6; ov++) {
      setLink(ov, _link[ov]);
    }
  }
  getLink(uint8_t ov) {
    bitCoding::getBased(24, ov, link);
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
const SCs Space[24] = { SCs(0, 1, 2, 1, 2, 3, 4, 5, 6),  //map of all real ortogonal orientations
                        SCs(0, 5, 1, 7, 8, 9, 0, 10, 11),
                        SCs(2, 1, 3, 11, 12, 8, 13, 0, 14),
                        SCs(4, 0, 2, 8, 13, 15, 16, 9, 0),
                        SCs(0, 2, 4, 0, 14, 13, 7, 16, 17),
                        SCs(5, 1, 0, 9, 0, 16, 17, 12, 10),
                        SCs(1, 3, 2, 10, 11, 0, 14, 17, 15),
                        SCs(0, 4, 5, 4, 18, 19, 1, 20, 21),
                        SCs(2, 0, 1, 21, 22, 18, 3, 1, 2),
                        SCs(4, 5, 0, 18, 3, 23, 5, 19, 1),
                        SCs(5, 3, 1, 19, 1, 5, 6, 22, 20),
                        SCs(1, 5, 3, 20, 21, 1, 2, 6, 23),
                        SCs(3, 1, 5, 23, 5, 21, 22, 2, 19),
                        SCs(4, 2, 3, 2, 19, 22, 20, 3, 4),
                        SCs(2, 3, 4, 6, 23, 2, 19, 4, 18),
                        SCs(3, 4, 2, 22, 20, 6, 23, 18, 3),
                        SCs(5, 0, 4, 3, 4, 20, 21, 23, 5),
                        SCs(1, 2, 0, 5, 6, 4, 18, 21, 22),
                        SCs(2, 4, 0, 17, 15, 14, 9, 7, 8),
                        SCs(4, 3, 5, 14, 9, 12, 10, 13, 7),
                        SCs(5, 4, 3, 13, 7, 10, 11, 15, 16),
                        SCs(1, 0, 5, 16, 17, 7, 8, 11, 12),
                        SCs(3, 2, 1, 12, 10, 17, 15, 8, 13),
                        SCs(3, 5, 4, 15, 16, 11, 12, 14, 9) };
CsT getCsT(uint8_t index) {
  return (Space[index].basis);
}

CsT tempCsT;

void transform(Vec* target, uint8_t sCSIndex) {
  target->Transform(&Space[sCSIndex].basis);
}
void untransform(Vec* target, uint8_t sCSIndex) {
  tempCsT = Space[sCSIndex].CSdecode();
  target->Untransform(&Space[sCSIndex].basis);
}

}