#pragma once

#include <EEPROM.h>
#include "USBAPI.h"
#include <stdint.h>
#include "Arduino.h"
#include "Updatable.h"
#include <Servo.h>
#include "Mathclasses.h"
#include "TimeManager.h"
#include "Animation.h"

// Lerp2 softStart(0, 1, 1);
const bool discMap[2][4] = { { 0, 1, 1, 0 },
                             { 0, 0, 1, 1 } };  //map of claw disk sectors (half)

class ClawUnit : IUpdatable {
  uint8_t mD, mP, eA, eB;    //Pins
  Servo servoGrab;           //Servo object
  uint8_t grabPositions[3];  //Positions for claw states
  uint8_t grabState = 0;

  //variables for stable control of rotation
  bool ab[2];
  uint8_t targetRotation = 0;   //0..7
  uint8_t currentRotation = 0;  //0..7

  bool enableChase = false;  //variables for simple rotation control
  bool shouldCheckIfActionIsNeeded = true;
  uint8_t arrivalState = 0;  //no data 0; over target 1; arrived 2.
  uint8_t oscilationNumber = 0;
  uint8_t chaseRoutine = 0;  //0 no; 1 counting arrival; 2 ramping up power.
  TimeManager arrivalTime = TimeManager();
  Timer rampUpTime;
  TimeManager oscilationStall = TimeManager();
  Lerp rampUpLerp;
  uint8_t chaseBasePower = 255;
  uint8_t chaseMinPower = 100;
  uint8_t chasePower = 0;
  float chaseDecrement;
  bool chaseDirection;



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
    pinMode(a, INPUT);
    pinMode(b, INPUT);
    pinMode(mD, OUTPUT);  //Set dc control
    pinMode(mP, OUTPUT);  //pins to output;

    rampUpLerp = Lerp(chaseMinPower, chaseBasePower);
  }
  void SetAngles(uint8_t releas, uint8_t hold, uint8_t grab) {  //set angles in degrees for states of grabbing
    grabPositions[0] = releas;
    grabPositions[1] = hold;
    grabPositions[2] = grab;
  }
  void runRotationMotor(bool clockwise, uint8_t force) {
    analogWrite(mP, force);
    digitalWrite(mD, !clockwise);
    motorForce = BSign(clockwise, force);
  }
  void setServo(uint8_t angle) {
    servoGrab.write(angle);
    Serial.println(angle);
  }
  void setGrab(uint8_t a) {
    setServo(grabPositions[a]);
    grabState = a;
  }
  void ease() {
    if (grabState >0) 
      setGrab(1+(grabState%2));
  }
  void toggleGrab() {
        setGrab(2*(!(grabState>0)));
  }
  void orientationUpdate() {                                     //updates encoder
    bool _ab[2] = { digitalRead(eA), digitalRead(eB) };          //remember current values
    uint8_t difference = (_ab[0] != ab[0]) + (_ab[1] != ab[1]);  //count differenses
    if (difference) {
      Serial.print(_ab[0]);  //log AB channels
      Serial.print(_ab[1]);
      Serial.print(" ");
      for (uint8_t i = 0; i < 2; i++) {                                      //step length
        uint8_t potential = Mod(8, currentRotation + BSign(i, difference));  //potential rotation index
        uint8_t searchIndex = Mod(4, potential);                             //potential index on disc map
        if (_ab[0] == discMap[0][searchIndex])
          if (_ab[1] == discMap[1][searchIndex])  //if potential channels = current
          {
            currentRotation = potential;  //potential is current
            break;                        //stop search
          }
      }
      ab[0] = _ab[0];
      ab[1] = _ab[1];  //new channel values assigning to static
      Serial.print(currentRotation);
      Serial.println(targetRotation);
      shouldCheckIfActionIsNeeded = true;
    }
  }
  void setChase(bool _chase) {
    enableChase = _chase;
  }
  void setTarget(int8_t _target) {
    targetRotation = Mod(8, _target);
    resetChase();
    shouldCheckIfActionIsNeeded = true;
  }
  void increaseTarget(int8_t delta) {
    setTarget(targetRotation + delta);
  }
  void resetChase() {
    oscilationNumber = 0;
    chaseRoutine = 0;
    arrivalState = 0;
  }
  uint8_t oscilationPower() {  // power to the motor vs oscilation number
    return (uint8_t(255 * pow(chaseDecrement, oscilationNumber)));
  }
  void chase() {                                //chases target
    if (shouldCheckIfActionIsNeeded) {          //if updated sector
      if (currentRotation == targetRotation) {  //if over target
        arrivalState = 1;                       //save it
        runRotationMotor(0, 0);                 //turn off motor
        chaseRoutine = 1;                       //turn on timer to arrival
        arrivalTime.resetTime();                //reset timer for proper work
      } else {
        chaseDirection = cycleDistanceVector(currentRotation, targetRotation, 8) > 0;  //if not over target define direction to target
        if (arrivalState) {                                                            //if was over target recently
          if (arrivalState == 2) {                                                     //if moved from outside forces
            chaseRoutine = 2;                                                          //start rampUpRoutin
            rampUpTime = Timer(150);
          } else {                                                //else if just arrived and oscilated before millis passed
            oscilationNumber++;                                   //count oscilation
            runRotationMotor(chaseDirection, oscilationPower());  //run motor with decreased power
            oscilationStall.resetTime();                          //run routine for stall prevention
            chaseRoutine = 3;
          }
          arrivalState = 0;  //update arrival state
        } else {             //if is not and wasn't over target
          runRotationMotor(chaseDirection, chaseBasePower);
        }
      }
      shouldCheckIfActionIsNeeded = false;
    }
    if (chaseRoutine)  //chase routines
      switch (chaseRoutine) {
        case 1:                                 //Arrival
          if (arrivalTime.isTimePassed(300)) {  //if 300 millis over target assume it's stopped and arrived
            chaseRoutine = 0;                   //stop routines
            arrivalState = 2;                   //affirm arrival
            oscilationNumber = 0;               //reset oscilations
            Serial.print("Arrived");
          }
          break;
        case 2:  //RampUp
          if (rampUpTime.isTime()) {
            chasePower = 255;
            chaseRoutine = 0;
            Serial.print("MAX POWA");
          } else {
            chasePower = rampUpLerp.whatHere(rampUpTime.spanPassed());
          }
          runRotationMotor(chaseDirection, chasePower);
          break;
        case 3:  //oscilation stall control
          if (oscilationStall.isTimePassed(200)) {
            resetChase();
            shouldCheckIfActionIsNeeded = true;
            Serial.print("Stall reset");
          }
          break;
        default:;
      }
  }
  void allignRotation() {
    runRotationMotor(true, chaseBasePower);
    do {
      orientationUpdate();
    } while (ab[0] != ab[1]);
    currentRotation = ab[0] * 2;
    runRotationMotor(0, 0);
  }
  void logEncoder() {
    Serial.print(digitalRead(eA));
    Serial.println(digitalRead(eB));
  }
  void update() {
    orientationUpdate();
    if (enableChase) chase();
  }
  void setChasePower(uint8_t base, uint8_t min, float dec) {
    chaseBasePower = base;
    chaseMinPower = min;
    chaseDecrement = dec;
  }
  void changeLimit(int8_t i, int8_t d) {
    grabPositions[i] += d;
  }
};