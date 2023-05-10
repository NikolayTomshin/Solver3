#pragma once
#include "Arduino.h"

void sPnl();
int8_t Mod(int8_t period, int8_t argument);
int8_t Mod3(int8_t argument);
int8_t Sign(int8_t i, int8_t argument);
int8_t BSign(bool b, int8_t a);
int8_t BMult(bool b, int8_t a);
int8_t cycleDistanceVector(int8_t position, int8_t target, uint8_t cycleLength);
