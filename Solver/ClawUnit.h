#pragma once
#include "Arduino.h"
#include "Updatable.h"
#include <Servo.h>
#include "Mathclasses.h"
class ClawUnit : IUpdatable {
  uint8_t mD, mP, minP, A, B, rstate, gstate, adP;
  Servo S;
public:
  ClawUnit() {}
  ClawUnit(uint8_t md, uint8_t mp, uint8_t minp, uint8_t adp, uint8_t sc, uint8_t a, uint8_t b) {
    mD = md;
    mP = mp;
    A = a;
    B = b;
    minP = minp;
    adP = adp;
    S.attach(sc);
    pinMode(mD, OUTPUT);
    pinMode(mP, OUTPUT);
  }
  void grab() {
    S.write(180);
    gstate = 2;
  }
  void release() {
    S.write(0);
    gstate = 0;
  }
  void loosen() {
    S.write(175);
    gstate = 1;
  }
  void toggle() {
    if (gstate > 0) release();
    else loosen();
  }
  void stop() {
    digitalWrite(mP, LOW);
  }
  void go(bool dir) {
    digitalWrite(mD, dir);
    analogWrite(mP, minP + adP * gstate);
  }
  void run(bool dir) {
    digitalWrite(mD, dir);
    analogWrite(mP, 255);
  }
  bool a() {
    return (digitalRead(A));  //нижний канал белый 1, чёрный 0
  }
  bool b() {
    return (digitalRead(B));  //верхний канал   '_'.
  }
  void Update() {
    rstate = a() + 2 * b();
  }
  uint8_t getState() {
    return (rstate);
  }
};