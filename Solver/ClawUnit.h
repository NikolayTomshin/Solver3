#include "USBAPI.h"
#pragma once
#include <stdint.h>
#include "Arduino.h"
#include "Updatable.h"
#include <Servo.h>
#include "Mathclasses.h"
#include "TimeManager.h"
#include "Animation.h"

// Lerp2 softStart(0, 1, 1);
const bool discMap[2][4] = { { 0, 0, 1, 1 },
                             { 0, 1, 1, 0 } };  //map of claw disk sectors (half)

class ClawUnit : IUpdatable {
  uint8_t mD, mP, eA, eB;    //Pins
  Servo servoGrab;           //Servo object
  uint8_t grabPositions[3];  //Positions for claw states
  uint8_t grabState = 0;

  //variables for stable control of rotation
  bool ab[2];
  uint8_t targetRotation = 0;   //0..7
  uint8_t currentRotation = 0;  //0..7
  bool chaseFunctionShouldAct;
  bool justArrived = false;
  uint8_t oscilationNumber = 0;
  TimeManager oscilationCD;

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
    mD = md;               //Rotary DC motor direction control pin;
    mP = mp;               //Rotary DC motor power control pin;
    eA = a;                //Pin for encoder A channel (lower track);
    eB = b;                //Pin for encoder B channel (upper track);
    servoGrab.attach(sc);  //Attach servo to sc pin, it grabs cube;
    pinMode(mD, OUTPUT);   //Set dc control
    pinMode(mP, OUTPUT);   //pins to output;
  }
  void SetAngles(uint8_t releas, uint8_t hold, uint8_t grab) {  //set angles in degrees for states of grabbing
    grabPositions[0] = releas;
    grabPositions[1] = hold;
    grabPositions[2] = grab;
  }
  void runRotationMotor(bool clockwise, uint8_t force) {
    analogWrite(mP, force);
    digitalWrite(mD, clockwise);
    motorForce = BSign(clockwise, force);
  }
  void toggleGrab() {
    grabState = (~grabState) & 2;
    servoGrab.write(grabPositions[grabState]);
  }
  void orientationUpdate() {
    bool _ab[2] = { digitalRead(eA), digitalRead(eB) };  //remember current values

    uint8_t difference = (_ab[0] != ab[0]) + (_ab[1] != ab[1]);  //count differenses
    if (difference) {                                            //if anything different
      for (uint8_t i = 0; i < 2; i++) {
        uint8_t potential = numberLoop(currentRotation + BSign(i, difference), 8);
        uint8_t searchIndex = numberLoop(potential, 4);  //to find where
        if (_ab[0] == discMap[0][searchIndex])
          if (_ab[1] == discMap[1][searchIndex])  //on the map of disc new place is
            currentRotation = potential;
      }
      ab[0] = _ab[0];
      ab[1] = _ab[1];
      chaseFunctionShouldAct = true;
    }
  }
  void setTarget(int8_t _target) {
    targetRotation = numberLoop(_target, 8);
    justArrived = false;
    chaseFunctionShouldAct=true;
  }
  void increaseTarget(int8_t delta) {
    setTarget(targetRotation + delta);
  }

  void chase() {  //chases target
    if (oscilationNumber > 0)
      if (oscilationCD.isTimePassed(150)) {
        oscilationNumber = 0;  //reset oscilations
        justArrived = false;
      }

    if (chaseFunctionShouldAct) {
      if (targetRotation == currentRotation)  //if arrived
      {
        justArrived = oscilationNumber == 0;  //if not oscilating than just arrived
        runRotationMotor(0, 0);
      } else {  //if not arrived
        Serial.print(cycleVector(currentRotation, targetRotation, 8));
        bool clockwise = (cycleVector(currentRotation, targetRotation, 8) > 0);
        uint8_t force = 100;
        if (justArrived) {  //oscilation detected
          oscilationCD.resetTime();
          force = uint8_t(float(force) * pow(0.7, oscilationNumber));
        }
        runRotationMotor(clockwise, force);
      }
      chaseFunctionShouldAct = false;
    }
  }
  void allignRotation() {
    runRotationMotor(true, 150);
    do {
      orientationUpdate();
    } while (ab[0] != ab[1]);
    currentRotation = ab[0] * 2;
    runRotationMotor(true, 0);
  }
  void update() {
    orientationUpdate();
    chase();
  }
};