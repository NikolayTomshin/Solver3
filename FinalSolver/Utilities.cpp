#include "Utilities.h"


void spn() {
  Serial.println();
}
bool& f(bool& other) {
  other = !other;
  return other;
}
int8_t Mod(int8_t period, int8_t argument) {  //it works
  if (!period) return 0;
  int8_t mod = argument % period;
  return mod < 0 ? period + mod : mod;
}
int8_t Mod3(int8_t argument) {
  return (Mod(3, argument));
}
int8_t Mod4(int8_t argument) {
  return (Mod(4, argument));
}
int8_t Mod8(int8_t argument) {
  return (Mod(8, argument));
}
char digitOf(uint8_t i){
  return '0'+i;
}

int8_t cycleDistanceVector(int8_t position, int8_t target, uint8_t cycleLength) {  //polar vector from position on to target cycle in length of cycle
  position = Mod(cycleLength, position);                                           //position on cycle
  target = Mod(cycleLength, target);                                               //target on cycle
  target = (target - position);                                                    //target is difference
  if (abs(target) > (cycleLength / 2))                                             //if difference greater than half cycle
    target += flipSign(target > 0, cycleLength);
  return (target);
}
float floatSigned(float value, bool positive) {
  if (positive)
    return value;
  return -value;
}
uint8_t arcQuarter(int8_t x, int8_t y) {  //windmill
  /* 
   y
 1 1 0 
 2 - 0>x
 2 3 3
*/
  if (y > 0)
    return (x <= 0);
  else if (y < 0)
    return 2 + (x >= 0);
  else return 2 * (x < 0);
}