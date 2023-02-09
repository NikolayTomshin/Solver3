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
int8_t cycleDistanceVector(int8_t position, int8_t target, uint8_t cycleLength) {  //polar vector from position to targer in length of cycle
  bool closerThanHalf = abs(target - position) < (cycleLength / 2);
  if (closerThanHalf) {          //if clothere than half the cyrcle than shortest path isn't going through polar 0 and calculates as difference
    return (target - position);  //
  }                              //else shortese path is going through polar 0 and we moving it
  return (target - position + BSign((target < position), cycleLength));
}
