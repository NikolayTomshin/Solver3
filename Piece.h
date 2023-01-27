#include "Vec.h"
#include <stdint.h>
#include "SCs.h"

struct Piece {
  uint8_t DefCords;
  Piece(uint8_t scs, Vec defvec) {    
    SCs=scs;
    DefCords=0;
     for(uint8_t i=0; i<3;i++){
     DefCords|(uint8_t(1+defvec.c[i])<<(2*i));
     }
  }
  Vec Def() {//вектор
    Vec V;
    V.c[0] = (DefCords & 0b11)-1;//первые 2 бита 
    V.c[1] = ((DefCords >> 2) & 0b11)-1;//вторые 2 бита 
    V.c[2] = ((DefCords >> 4) & 0b11)-1;//третьи 2 бита 
    return (V);
  }
}