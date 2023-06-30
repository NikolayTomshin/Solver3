#pragma once
#include <stdint.h>
#include "Awareness.h"

ClawUnit* getBlock(bool right) {
  if (right) return &n2;
  else return &n1;
}

