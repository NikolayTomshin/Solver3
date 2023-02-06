#pragma once
#include <stdint.h>
#include "Arduino.h"
#include "Updatable.h"
#include <Servo.h>
#include "Mathclasses.h"
#include "TimeManager.h"
#include "Animation.h"
class ClawUnit : IUpdatable {
  uint8_t mD, mP, A, B;               //Pins
  Servo S;                            //Servo object
  uint8_t grabPositions[3];           //Positions for claw states
  uint8_t rotationData = 0b11111100;  //bits:[0-7 target,0-7 current,BA]=[3,3,2] bits
  uint8_t chaseData = 0;              //[5unused,clockwise,run,update]

  //variables for predicting claw rotation between ortogonal orientations
  float degreesPosition;  // degrees         RealWorld //can be measured with robot hardware  +-360 from 0
  float degreesVelocity;  // degrees/second  RealWorld
  Lerp2 dynamicDrag;      // d/s^2          {Relative  // currently can't be measured with robot hardware
  Lerp2 staticDrag;       // d/s^2           Relative  // system has 1 degree of freedom
  Lerp2 degreeInertia;    // j=kg*m*degree   Relative  // but values can be picked relatively to make good predictions
                          // Lerp2 is used to predict these parameters vs degree of grabing
  float cubeInertia;      // j               Relative  // each modules parametres are measured relative
  float motorForce;       // j*d/s^2         base}     // to their motor torque

public:
  // void setBA(uint8_t _ba){};
  ClawUnit() {}
  ClawUnit(uint8_t md, uint8_t mp, uint8_t sc, uint8_t a, uint8_t b) {
    mD = md;              //Rotary DC motor direction control pin;
    mP = mp;              //Rotary DC motor power control pin;
    A = a;                //Pin for encoder A channel (lower track);
    B = b;                //Pin for encoder B channel (upper track);
    S.attach(sc);         //Attach servo to sc pin, it grabs cube;
    pinMode(mD, OUTPUT);  //Set dc control
    pinMode(mP, OUTPUT);  //pins to output;
    setBA((digitalRead(B) << 1) | digitalRead(A));
  }
  void SetAngles(uint8_t releas, uint8_t hold, uint8_t grab) {  //set angles in degrees for states of grabbing
    grabPositions[0] = releas;
    grabPositions[1] = hold;
    grabPositions[2] = grab;
  }
  void physicsUpdate() {
  }
  uint8_t getBA() {  //get BA bits
    return (rotationData & 0b00000011);
  }
  void setBA(uint8_t _ba) {
    rotationData = (rotationData & (~3)) | _ba;
  }
  void setRotation(uint8_t value, bool target) {  //assign new value to target or current bits in rotary data
    rotationData = (rotationData & (~(0b00011100 << (target * 3)))) | (value << (2 + target * 3));
  }
  uint8_t getRotation(bool target) {  //get values of target ot current orientation inrotary data
    return ((rotationData >> (2 + 3 * target)) & 0b00000111);
  }

  void rotationPulse(bool clockwise) {            //register pulse from encoder, just changes position in variable cw or ccw
    int8_t position = getRotation(false);         //assign 0-7 rotation to int8_t
    position = position + BSign(clockwise, 1);    //modify position according to pulse direction
    setRotation(numberLoop(position, 8), false);  //assign new rotation value as uint8_t
  }
  void updateTarget() {
    if (getRotation(true) != getRotation(false)) {  //if target is not achieved
      chaseData = 0b00000011 | (4 * (cycleVector(getRotation(true), getRotation(false), 8) > 0));
    } else chaseData = 1;  //else target is achieved and chase can stop running
  }
  void updateBA() {                                        //Update BA values and register pulse if that's tha case
    uint8_t _ba = (digitalRead(B) << 1) | digitalRead(A);  //current reading stored in _ba
    uint8_t ba = getBA();                                  //remember previous BA

    if (_ba != ba) {  //if ba!=_ba then there is pulse
    Serial.print(getRotation(true));
      Serial.println(getRotation(false));
      bool _a = _ba & 1;
      rotationPulse(((_a != (ba & 1)) == (_a)) != (_ba >> 1));  //direction of pulse calculated and new rotation is written
      setBA(_ba);
      //rotary state update complete
      //initiating chase update
      updateTarget();
    }
  }
  void setTarget(uint8_t targetPos) {  //set target specifically
    setRotation(numberLoop(targetPos, 8), true);

    updateTarget();
  }
  void increasTarget(bool clockwise) {  //increase target by 2
    setTarget(numberLoop(getRotation(true) + BSign(clockwise, 2), 8));
  }
  void runRotation(bool direction, uint8_t power) {
    digitalWrite(mD, direction);
    analogWrite(mP, power);
    motorForce = float(BSign(direction, power));
  }
  void chase() {

    if (chaseData & 1) {  //if there is update for chase
      runRotation(chaseData & 4, 255 * (chaseData & 2));
      chaseData = chaseData & (~1);
    }
  }
  void allign() {
    runRotation(1, 255);
    while (getBA() != 0) {
      updateBA();
    }
    setRotation(0, false);
    setTarget(0);
  }
  void update() {}
};