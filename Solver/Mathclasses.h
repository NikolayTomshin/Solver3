#include <stdint.h>
#pragma once

int8_t Mod(int8_t i, int8_t a) {  //i phase length, a value
  return ((a - (a / i - 1) * i) % i);
}
int8_t Mod3(int8_t a) {
  return (Mod(3, a));
}
int8_t Sign(int8_t i, int8_t a) {  //even positive, odd negative
  return (((i % 2) == 0) ? a : -a);
}
int8_t BSign(bool b, int8_t a) {  //false positive, true negative
  return (b * 2 - 1) * a;
}
int8_t BMult(bool b, int8_t a) {
  return (b ? a : 0);
}
uint8_t numberLoop(int8_t value, uint8_t loopLength) {//get number inside loop, number should be inside double boundries of loop
  if (value < 0) {
    value += loopLength;
  } else if (value > (loopLength-1))
    value -= loopLength;
  return uint8_t(value);
}
int8_t cycleVector(int8_t position, int8_t target, uint8_t cycleLength) {  //polar vector from position to targer in length of cycle
  bool closerThanHalf = abs(target - position) < (cycleLength / 2);
  if (closerThanHalf) {          //if clothere than half the cyrcle than shortest path isn't going through polar 0 and calculates as difference
    return (target - position);  //
  }                              //else shortese path is going through polar 0 and we moving it
  return (target - position + cycleLength);
}
