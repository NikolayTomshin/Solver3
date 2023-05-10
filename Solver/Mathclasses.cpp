#include "Mathclasses.h"

void sPnl() {
  Serial.println();
}
int8_t Mod(int8_t period, int8_t argument) {  //i phase length, a value
  return ((argument - (argument / period - 1) * period) % period);
}
int8_t Mod3(int8_t argument) {
  return (Mod(3, argument));
}
int8_t Sign(int8_t i, int8_t argument) {  //even positive, odd negative
  return (((i % 2) == 0) ? argument : -argument);
}
int8_t BSign(bool b, int8_t a) {  //false positive, true negative
  return (b * 2 - 1) * a;
}
int8_t BMult(bool b, int8_t a) {
  return (b ? a : 0);
}
int8_t cycleDistanceVector(int8_t position, int8_t target, uint8_t cycleLength) {  //polar vector from position on to target cycle in length of cycle
  position = Mod(cycleLength, position);
  target = Mod(cycleLength, target);
  target = (target - position);
  if (abs(target) > (cycleLength / 2)) target -= BSign(target > 0, cycleLength);
  return (target);
}
