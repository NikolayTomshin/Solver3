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
  return ((i % 2) ? argument : -argument);
}
int16_t BSign(bool flipSign, int16_t a) {  //false positive, true negative
  if (flipSign) return -a;
  else return a;
}
int8_t cycleDistanceVector(int8_t position, int8_t target, uint8_t cycleLength) {  //polar vector from position on to target cycle in length of cycle
  position = Mod(cycleLength, position);                                           //position on cycle
  target = Mod(cycleLength, target);                                               //target on cycle
  target = (target - position);                                                    //target is difference
  if (abs(target) > (cycleLength / 2))                                             //if difference greater than half cycle
    target += BSign(target > 0, cycleLength);
  return (target);
}
float floatSigned(float value, bool positive) {
  if (positive)
    return value;
  return -value;
}
uint8_t arcQuarter(int8_t x, int8_t y) {
  if (y > 0)
    return (x <= 0);
  else if (y < 0)
    return 2 + (x >= 0);
  else return 2 * (x < 0);
}