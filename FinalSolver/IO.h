#pragma once
#include "Arduino.h"
//crossplatform io
template<class T> inline void pout(T data) {
  Serial.print(data);
}
void pnl();

template<class T> inline void poutN(T data) {
  pout(data);
  pnl();
}