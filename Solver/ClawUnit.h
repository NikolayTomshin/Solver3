#pragma once
#include "Arduino.h"
#include "Updatable.h"
#include <Servo.h>
#include "Mathclasses.h"
class ClawUnitBase : IUpdatable {
protected:
  uint8_t mD, mP, A, B;  //Pins
  Servo S;               //Servo object
  uint8_t grabPositions[3];
  uint8_t rstate;

public:
  ClawUnitBase() {}
  ClawUnitBase(uint8_t md, uint8_t mp, uint8_t sc, uint8_t a, uint8_t b) {
    mD = md;              //Rotary DC motor direction control pin;
    mP = mp;              //Rotary DC motor power control pin;
    A = a;                //Pin for encoder A channel (lower track);
    B = b;                //Pin for encoder B channel (upper track);
    S.attach(sc);         //Attach servo to sc pin, it grabs cube;
    pinMode(mD, OUTPUT);  //Set dc control
    pinMode(mP, OUTPUT);  //pins to output;
  }
  void SetAngles(uint8_t releas, uint8_t hold, uint8_t grab) {
    grabPositions[0] = releas;
    grabPositions[1] = hold;
    grabPositions[2] = grab;
  }

  void stop() {
    digitalWrite(mP, LOW);
  }
  void go(bool dir) {
    digitalWrite(mD, dir);
    analogWrite(mP, 170);
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
  uint8_t getState() {
    return (rstate);
  }
  void update() {}
  friend class ClawState;
};

class ClawState {
  ClawUnitBase *subject;
  virtual stateUpdate(){};
};

class ClawUnit : ClawUnitBase {
  ClawState state;
public:
  ClawUnit()
    : ClawUnitBase() {}
  ClawUnit(uint8_t md, uint8_t mp, uint8_t sc, uint8_t a, uint8_t b)
    : ClawUnitBase(md, mp, sc, a, b) {}
};