#include "Vec.h"
#include <stdint.h>
#include "SCs.h"
//file for dealing with pieces of virtual cube (8)

namespace Pieces {


struct Piece {//real cube piece
  cubeVec defPos;//position in solved cube (cube Cs)
  cubeVec curPos;//position in present time to track during cube
  Piece(Vec pos){
    curPos=cubeVec(pos);
  }
}

uint8_t indexByDefPos(Vec pos){         //corners 1234 5678; edges 1234 5678; vertical edges 1234
  uint8_t temp;
  temp=pos.c[0]+1+3*(pos.c[1]+1);
  for(uint8_t i=0; i<4;i++){
   
  }
}

Piece Sorted[20];
}