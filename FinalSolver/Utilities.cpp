#include "Utilities.h"


void spn() {
  Serial.println();
}
bool& f(bool& other) {
  other = !other;
  return other;
}
String boolStr(bool value) {
  return value ? F("true") : F("false");
}
bool fromString(const String& boolString) {
  return !boolStr(true).compareTo(boolString);
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
char digitOf(uint8_t i) {
  return '0' + i;
}
uint8_t valueOf(char c) {
  return c - '0';
}
int8_t cycleDistanceVector(int8_t position, int8_t target, uint8_t cycleLength) {  //polar vector from position on to target cycle in length of cycle
  position = Mod(cycleLength, position);                                           //position on cycle
  target = Mod(cycleLength, target);                                               //target on cycle
  target = (target - position);                                                    //target is difference
  if (abs(target) > (cycleLength / 2))                                             //if difference greater than half cycle
    target += flipSign(target > 0, cycleLength);
  return (target);
}

uint16_t endOfTheLine(const String& string, const uint8_t& lineSize, const uint16_t& lineStart, const String& separator) {
  uint16_t lineLimit = lineStart + lineSize;                      //limit is first char of next line
  int16_t spaceIndex = string.lastIndexOf(separator, lineLimit);  //find first space from limit backwards
  if (spaceIndex < lineStart)                                     //if space is from previous line
    return limits(lineLimit, string.length());                    //keep limit limited to string size
  else
    return spaceIndex;  //else set to space index
}


uint16_t indexOfSkipping(const String& string, uint16_t startingIndex, const String& separator, bool directionForward) {
  struct FData {
    const String& string;
    const String& separator;
    uint16_t& i;
    uint8_t sepLen;
    uint8_t between;
    uint16_t jumpLimit;
    FData() {}
    uint16_t fForward() {                                              //forward
      jumpLimit = string.length() - ((string.length() - i) % sepLen);  //limit index
      while (i < jumpLimit) {
        if ((string.substring(i, i + sepLen).compareTo(separator)))  //if substring not equal to separator
          break;                                                     //stop
        i += sepLen;
      }
      return i;
    }
    uint16_t fBackward() {  //forward
      jumpLimit = i % sepLen;
      while (i != jumpLimit) {
        if ((string.substring(i - between, i + 1).compareTo(separator)))  //if substring not equal to separator
          break;                                                          //stop
        i -= sepLen;
      }
      return i;
    }
  } d;
  if (((d.sepLen = separator.length()) == 0) || (string.length() <= startingIndex)) return startingIndex;
  d.string = string;
  d.separator = separator;
  d.i = startingIndex;
  d.between = d.sepLen - 1;  //legth between first and last chars of separator
  if (directionForward) return d.fForward();
  else return d.fBackward();
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

//Non templates
//Array

//ArrayIterator

///ArrayIterator
///Array