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
