#pragma once
#include "Cube.h"
#include <Servo.h>
#include "Animation.h"
const bool discMap[2][4] = { { 0, 1, 1, 0 },
                             { 0, 0, 1, 1 } };  //map of claw disk sectors (half)
class ClawUnit;

struct ClawSetting {  //copy buffer
  bool copiedAngles = false, copiedDynamics = false, copiedPins = false;
  uint8_t mD, mP, eA, eB, sC;     //Pins
  bool reverseDirection = false;  //reverse direction
  uint8_t rotShift = 0;

  uint8_t grabPositions[3];

  float decrement;
  uint8_t sustainable;
  uint8_t min;
  void pastAngles(ClawUnit* target);
  void pastDynamics(ClawUnit* target);
  void pastPins(ClawUnit* target);
  void pastAll(ClawUnit* target);
  void copyAngles(ClawUnit* target);
  void copyDynamics(ClawUnit* target);
  void copyPins(ClawUnit* target);
  void copyAll(ClawUnit* target);
  void readEP(bool right);
  void writeEP(bool right);
};

class ClawUnit : public IUpdatable {  //class for claw module control
  uint8_t mD, mP, eA, eB, sC;         //Pins
  bool reverseDirection = false;      //reverse direction
  Servo servoGrab;                    //Servo object

  uint8_t grabPositions[3];  //Positions for claw states
  uint8_t grabState = 0;

  //variables for stable control of rotation
  bool ab[2];
  uint8_t rotShift = 0;         //0..7
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



  //variables for predicting claw rotation between ortogonal orientations | unused currently
  float degreesPosition;  // degrees         RealWorld //can be measured with robot hardware  +-360 from 0
  float degreesVelocity;  // degrees/second  RealWorld
  Lerp2 dynamicDrag;      // d/s^2          {Relative  // currently can't be measured with robot hardware
  Lerp2 staticDrag;       // d/s^2           Relative  // system has 1 degree of freedom
  Lerp2 degreeInertia;    // j=kg*m*degree   Relative  // but values can be picked relatively to make good predictions
                          // Lerp2 is used to predict these parameters vs degree of grabing
  float cubeInertia;      // j               Relative  // each modules parametres are measured relative

  float motorForce;  // j*d/s^2         base}     // to their motor torque

public:
  // void setBA(uint8_t _ba){};
  ClawUnit() {}
  ClawUnit(uint8_t md, uint8_t mp, uint8_t sc, uint8_t a, uint8_t b) {
    mD = md;  //Rotary DC motor direction control pin;
    mP = mp;  //Rotary DC motor power control pin;
    eA = a;   //Pin for encoder A channel (lower track);
    eB = b;   //Pin for encoder B channel (upper track);
    reattach(sc);
    pinMode(a, INPUT);
    pinMode(b, INPUT);
    pinMode(mD, OUTPUT);  //Set dc control
    pinMode(mP, OUTPUT);  //pins to output;
    rampUpLerp = Lerp(chaseMinPower, chaseBasePower);
  }
  void reattach(uint8_t sc) {
    servoGrab.detach();
    sC = sc;
    servoGrab.attach(sc);  //Attach servo to sc pin, it grabs cube;
  }
  // set(uint8_t pin, uint8_t value) {
  //   switch (pin) {
  //     case 0:
  //       mP = value;
  //       break;
  //     case 1:
  //       cS = value;
  //       break;
  //     case 2:
  //       eA = value + 18;
  //       break;
  //     case 3:
  //       eB = value + 18;
  //       break;
  //     case 4:
  //       mD = value;
  //       break;
  //     default:
  //   }
  // A
  // 18
  // 19
  // 20
  // 21
  // 22
  // 23
  void SetAngles(uint8_t releas, uint8_t hold, uint8_t grab) {  //set angles in degrees for states of grabbing
    grabPositions[0] = releas;
    grabPositions[1] = hold;
    grabPositions[2] = grab;
  }
  void runRotationMotor(bool clockwise, uint8_t force) {
    analogWrite(mP, force);
    digitalWrite(mD, !clockwise != reverseDirection);
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
    if (grabState > 0)
      setGrab(1 + (grabState % 2));
  }
  void toggleGrab() {
    setGrab(2 * (!(grabState > 0)));
  }
  void orientationUpdate() {                                     //updates encoder
    bool _ab[2] = { digitalRead(eA), digitalRead(eB) };          //remember current values
    uint8_t difference = (_ab[0] != ab[0]) + (_ab[1] != ab[1]);  //count differenses
    if (difference) {
      // Serial.print(_ab[0]);  //log AB channels
      // Serial.print(_ab[1]);
      // Serial.print(" ");
      for (uint8_t i = 0; i < 2; i++) {                                      //step length
        uint8_t potential = Mod(8, currentRotation + BSign(i, difference));  //potential rotation index
        uint8_t searchIndex = Mod(4, potential + rotShift);                  //potential index on disc map
        if (_ab[0] == discMap[0][searchIndex])
          if (_ab[1] == discMap[1][searchIndex])  //if potential channels = current
          {
            currentRotation = potential;  //potential is current
            break;                        //stop search
          }
      }
      ab[0] = _ab[0];
      ab[1] = _ab[1];  //new channel values assigning to static
      // Serial.print(currentRotation);
      // Serial.println(targetRotation);
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
  void assumeRotation() {
    bool _ab[2] = { digitalRead(eA), digitalRead(eB) };
    for (currentRotation = 0; currentRotation < 3; currentRotation++) {
      if ((discMap[0][currentRotation] == _ab[0]) && (discMap[1][currentRotation] == _ab[1]))
        break;
    }
    currentRotation = Mod(4, currentRotation + rotShift);
  }
  void changeRotshift(bool positive) {
    int8_t delta = BSign(!positive, 1);
    rotShift = Mod(4, int8_t(rotShift) + delta);
    currentRotation = Mod(4, currentRotation - delta);
    shouldCheckIfActionIsNeeded = true;
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
  friend class StandBy;
  friend struct ClawSetting;
};

//settings copypast
void ClawSetting::pastAngles(ClawUnit* target) {
  for (uint8_t i = 0; i < 3; i++)
    target->grabPositions[i] = grabPositions[i];
}
void ClawSetting::pastDynamics(ClawUnit* target) {
  target->chaseDecrement = decrement;
  target->chaseMinPower = min;
}
void ClawSetting::pastPins(ClawUnit* target) {
  target->mD = mD;
  target->mP = mP;
  target->reattach(sC);
  target->eA = eA;
  target->eB = eB;
  target->reverseDirection = reverseDirection;
}
void ClawSetting::pastAll(ClawUnit* target) {
  target->rotShift = rotShift;
  pastAngles(target);
  pastDynamics(target);
  pastPins(target);
}
void ClawSetting::copyAngles(ClawUnit* target) {
  copiedAngles = true;
  for (uint8_t i = 0; i < 3; i++)
    grabPositions[i] = target->grabPositions[i];
}
void ClawSetting::copyDynamics(ClawUnit* target) {
  copiedDynamics = true;
  decrement = target->chaseDecrement;
  min = target->chaseMinPower;
}
void ClawSetting::copyPins(ClawUnit* target) {
  copiedPins = true;
  mD = target->mD;
  mP = target->mP;
  sC = target->sC;
  eA = target->eA;
  eB = target->eB;
  reverseDirection = target->reverseDirection;
}
void ClawSetting::copyAll(ClawUnit* target) {
  rotShift = target->rotShift;
  copyAngles(target);
  copyDynamics(target);
  copyPins(target);
}