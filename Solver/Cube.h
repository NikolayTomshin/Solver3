#include <stdint.h>
#include "Arduino.h"
#include "Piece.h"
#include <stdio.h>

namespace Cube {
struct state {
  uint64_t a;
  uint64_t b;
  uint8_t GetScsI(uint8_t pieceindex) {
    uint64_t temp;
    if (pieceindex < 10)
      return ((a >> (pieceindex * 5)) & (0b11111));
    return ((b >> (pieceindex * 5)) & (0b11111));
  }
  state(uint64_t A, uint64_t B)
  a=A;
  b=B
};
}
