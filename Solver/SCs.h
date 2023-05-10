#pragma once
#include <stdint.h>
#include "CsT.h"
//Super Coordinate System - way of fast cs rotation without actually doing it
//How it works: SCS.Space -array of all possible rotations of normal basis in space. All 24 Cs can be returned by value by .CSdecod from SCS
//function .GetLink(uint8_t v) returns index of SCs that is same as THIS Cs rotated clockwise by ortovector by index v
//counterclockwise rotation is same as rotation clockwise by negative vector, calculate it's index somewhere else
//so, instead of rotating every vector of cs, we store index of SCs and see which SCs it's pointing to


struct SCs {                //structure for orientation space storage
  uint8_t link[4] = { 0 };  //bitcoded links
  CsT basis;                //Straightforward CsT as value
  SCs();
  SCs(CsT cs);
  SCs(CsT cs, uint8_t i, uint8_t j, uint8_t k, uint8_t _i, uint8_t _j, uint8_t _k);
  uint8_t getLink(uint8_t ov);
  void setLink(uint8_t ov, uint8_t sindex);
};

namespace SCS {
const extern SCs Space[24];
uint8_t getSCindex(CsT cs);
CsT getCsT(uint8_t index);
void transform(Vec* target, uint8_t sCSIndex);
void untransform(Vec* target, uint8_t sCSIndex);

void generateSCs();

}