#pragma once
#include "Arduino.h"

void sPnl();
int8_t Mod(int8_t period, int8_t argument);
int8_t Mod3(int8_t argument);
int8_t Sign(int8_t i, int8_t argument);
int16_t BSign(bool negative, int16_t a);
int8_t cycleDistanceVector(int8_t position, int8_t target, uint8_t cycleLength);
template<class T> T limits(T value, T limit, bool upperLimit = true) {
  if ((value > limit) == upperLimit) {
    return limit;
  }
  return value;
}
template<class T> T doubleLimits(T value, T lLimit, T uLimit) {//double limit lower and upper
  return limits<T>(limits<T>(value, lLimit, false), uLimit, true);
}
template<class T> T myAbs(T value) {
  if (value < 0) {
    return -value;
  }
  return value;
}
template<class T> T TBSign(T value, bool stayPositive, bool assignSignNOTflip = false) {  //value, positive sign, sign assign mode
  if (assignSignNOTflip) value = myAbs<T>(value);
  if (stayPositive) {
    return value;
  }
  return value;
}
template<class T> T absLimits(T value, T limit, bool upperLimit = true) {
  bool negative = value < 0;
  if ((myAbs<T>(value) > limit) == upperLimit) {
    return TBSign(limit, !negative);
  }
  return value;
}
float floatSigned(float value, bool positive);

template<class T> T arSum(T arr[], uint8_t size) {
  T sum = arr[0];
  for (uint8_t i = 1; i < size; i++) {
    sum += arr[i];
  }
  return sum;
}

template<class T> bool inMargin(T value, T target, T epsilon) {
  T a = value - target;
  if (a > 0) {
    return a < epsilon;
  }
  return a > -epsilon;
}