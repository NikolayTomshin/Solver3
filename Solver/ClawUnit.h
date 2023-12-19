#include <math.h>
#include <stdint.h>
#pragma once
#include "Cube.h"
#include <Servo.h>
#include "Animation.h"
#define colorLag 5

const bool discMap[2][4] = { { 0, 1, 1, 0 },
                             { 0, 0, 1, 1 } };  //map of claw disk sectors (half)
Timer grabDelay = Timer(60);

struct DiscoData {
  //     <-cw    |      .
  //    0 | 1    |      1
  //   ---O---   |   -0-O-2-
  //    3 | 2    |      3
  // time frames |  assignment for cw counting
  uint8_t startOA;
  uint8_t discoState = 0;  //0 wait for syncg; 1 setting up speed;
  uint8_t rotations = 0;
  uint8_t quarterIndex = 0;  //index of current quarter(0..3)
  uint16_t lastRound = 0;
  uint16_t quarters[4] = { 0 };  //times of quartes 0-2, 2-4, 4-6, 6-8(0)
  bool lowError = false;
  DiscoData(uint8_t _startOA) {
    startOA = _startOA;
  }
  ~DiscoData() {
    Serial.println("Disco deleted");
  }
  uint8_t getQIndex(int8_t delta = 0) {
    return (Mod(4, delta + quarterIndex));
  }
  void incQIndex(int8_t delta = 1) {
    quarterIndex = getQIndex(delta);
  }
  long setQTime(TimeManager* timer, int8_t delta = 0) {
    long time = timer->timePassed();
    quarters[quarterIndex + delta] = time;
    timer->resetTime();
    return (time);
  }
  void printQ() {
    for (uint8_t i = 0; i < 4; i++) {
      Serial.print(quarters[i]);
      Serial.write('\t');
    }
    Serial.write('\n');
  }
  bool lastLowError() {
    return abs(quarters[getQIndex(-1)] - lastRound / 4) == 1;
  }
};
DiscoData* disco;  //only for disco time

class ClawUnit : public IUpdatable {  //class for claw module control
  uint8_t mD, mP, eA, eB, sC;         //Pins
  bool reverseDirection = false;      //reverse direction
  MyServo servoGrab;                  //Servo object


  uint8_t grabPositions[3];  //Positions for claw states
  uint8_t targetAngle;
  uint8_t grabState = 0;

  //variables for stable control of rotation
  bool ab[2];
  uint8_t rotShift = 0;         //0..3
  uint8_t targetRotation = 0;   //0..7
  uint8_t currentRotation = 0;  //0..7

  bool enableChase = false;  //variables for simple rotation control
  bool shouldCheckIfActionIsNeeded = true;
  uint8_t jamStatus = 0;     //0 not interested, 1 catching, 2 doing unjamming manuwer
  uint8_t arrivalState = 0;  //no data 0; over target 1; arrived 2; disco mod 3;
  uint8_t oscilationNumber = 0;
  uint8_t chaseRoutine = 0;     //0 no; 1 counting arrival; 2 ramping up power.
  TimeManager arrivalTime;      //counting time during flying over
  Timer rampUpTime;             //timer for ramping up
  TimeManager oscilationStall;  //timer to count stall during oscilations
  TimeManager jamTimer;         //to get out of jam
  Lerp rampUpLerp;
  uint8_t chaseBasePower = 255;
  uint8_t chaseMinPower = 100;
  uint8_t chasePower = 0;
  float chaseDecrement;
  bool chaseDirection;

  // //variables for predicting claw rotation between ortogonal orientations | unused currently
  // float degreesPosition;  // degrees         RealWorld //can be measured with robot hardware  +-360 from 0
  // float degreesVelocity;  // degrees/second  RealWorld
  // Lerp2 dynamicDrag;      // d/s^2          {Relative  // currently can't be measured with robot hardware
  // Lerp2 staticDrag;       // d/s^2           Relative  // system has 1 degree of freedom
  // Lerp2 degreeInertia;    // j=kg*m*degree   Relative  // but values can be picked relatively to make good predictions
  //                         // Lerp2 is used to predict these parameters vs degree of grabing
  // float cubeInertia;      // j               Relative  // each modules parametres are measured relative

  float motorForce;  // j*d/s^2         base}     // to their motor torque

public:
  void setReverse(bool _reverse) {
    reverseDirection = _reverse;
  }
  // void setBA(uint8_t _ba){};
  uint8_t getArrival() {
    return arrivalState;
  }
  bool isArrived(uint8_t howMuch) {
    return (arrivalState >= howMuch);
  }
  ClawUnit() {}
  ClawUnit(uint8_t md, uint8_t mp, uint8_t sc, uint8_t a, uint8_t b) {
    mD = md;  //Rotary DC motor direction control pin;
    mP = mp;  //Rotary DC motor power control pin;
    eA = a;   //Pin for encoder A channel (lower track);
    eB = b;   //Pin for encoder B channel (upper track);
    servoGrab = MyServo(360, 240, 65, 100);
    servoGrab.teleport(0);
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
  void setServoSpeed(uint16_t unitsPerSec, uint8_t u = 100, uint8_t d = 100) {
    servoGrab.setSpeed(unitsPerSec, u, d);
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
    // Serial.print("clockwise=");
    // Serial.println(clockwise);
    // Serial.print('\t');
    // Serial.println(force);
    
    analogWrite(mP, force);
    digitalWrite(mD, !clockwise != reverseDirection);
    motorForce = BSign(!clockwise, force);
  }
  void setServo(uint8_t angle) {
    jdun(&grabDelay, systemUpdate);
    grabDelay.resetTime();
    servoGrab.write(angle);
    // Serial.println(angle);
  }
  void setGrab(uint8_t positionIndex, uint8_t minusDelta = 0) {
    setServo(grabPositions[positionIndex] - minusDelta);
    grabState = positionIndex;
    // Serial.print("SG");
    // Serial.println(grabState);
  }
  void ease() {
    if (grabState > 0)
      setGrab(1 + (grabState % 2));
  }
  // void toggleGrab() {
  //   setGrab(!(grabState > 0));
  // }
  void orientationUpdate() {                                     //updates encoder
    bool _ab[2] = { digitalRead(eA), digitalRead(eB) };          //remember current values
    uint8_t difference = (_ab[0] != ab[0]) + (_ab[1] != ab[1]);  //count differenses
    if (difference) {
      // Serial.print(_ab[0]);  //log AB channels
      // Serial.print(_ab[1]);
      // Serial.print(" ");
      switch (difference) {
        case 1:
          for (uint8_t i = 0; i < 2; i++) {                             //step length
            uint8_t potential = Mod(8, currentRotation + BSign(i, 1));  //potential rotation index
            uint8_t searchIndex = Mod(4, potential - rotShift);         //potential index on disc map
            if (_ab[0] == discMap[0][searchIndex])
              if (_ab[1] == discMap[1][searchIndex])  //if potential channels = current
              {
                currentRotation = potential;  //potential is current
                break;                        //stop search
              }
          }
          break;
        default /*2*/: currentRotation = Mod(8, currentRotation + BSign(motorForce < 0, 2));  //assume rotated with direction of motor power
      }
      ab[0] = _ab[0];
      ab[1] = _ab[1];  //new channel values assigning to static
      // Serial.print(mD);
      Serial.print(currentRotation);
      Serial.println(targetRotation);
      shouldCheckIfActionIsNeeded = true;
    }
  }
  void setChase(bool _chase) {
    enableChase = _chase;
    if (_chase) {
      discoMode(false);
    }
  }
  void setTarget(int8_t _target, bool enableJam = true) {
    targetRotation = Mod(8, _target);
    resetChase();
    shouldCheckIfActionIsNeeded = true;
    jamStatus = enableJam;  //initiate jam catching
    jamTimer.resetTime();
  }
  void increaseTarget(int8_t delta) {
    setTarget(targetRotation + delta);
    update();
  }
  void resetChase() {
    oscilationNumber = 0;
    chaseRoutine = 0;
    arrivalState = 0;
  }
  uint8_t oscilationPower() {  // power to the motor vs oscilation number
    return (uint8_t(255 * pow(chaseDecrement, oscilationNumber - 1)));
  }
  void chase() {                        //chases target
    if (shouldCheckIfActionIsNeeded) {  //if updated sector
      if (jamStatus == 1) {             //reset jam timer if catching
        jamTimer.resetTime();
      }
      if (currentRotation == targetRotation) {  //if over target
        jamStatus = 0;
        arrivalState = 1;         //save it
        runRotationMotor(0, 0);   //turn off motor
        chaseRoutine = 1;         //turn on timer to arrival
        arrivalTime.resetTime();  //reset timer for proper work
      } else {
        chaseDirection = (cycleDistanceVector(currentRotation, targetRotation, 8) > 0);  //if not over target define direction to target
        // Serial.print("dir");
        // Serial.println(cycleDistanceVector(currentRotation, targetRotation, 8));
        if (arrivalState) {         //if was over target recently
          if (arrivalState == 2) {  //if moved from outside forces
            jamStatus = 0;
            chaseRoutine = 2;  //start rampUpRoutin
            rampUpTime = Timer(300);
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
          if (arrivalTime.isTimePassed(100)) {  //if 300 millis over target assume it's stopped and arrived
            chaseRoutine = 0;                   //stop routines
            arrivalState = 2;                   //affirm arrival
            oscilationNumber = 0;               //reset oscilations
            // Serial.print("Arrived");
          }
          break;
        case 2:  //RampUp
          jamTimer.resetTime();
          if (rampUpTime.isTime()) {
            chasePower = 255;
            chaseRoutine = 0;
            // Serial.print("MXP");
          } else {
            chasePower = rampUpLerp.whatHere(rampUpTime.spanPassed());
          }
          runRotationMotor(chaseDirection, chasePower);
          break;
        case 3:  //oscilation stall control
          jamTimer.resetTime();
          if (oscilationStall.isTimePassed(200)) {
            resetChase();
            shouldCheckIfActionIsNeeded = true;
            // Serial.print("Stall reset");
          }
          break;
        default:;
      }
  }
  void assumeRotation() {
    ab[0] = digitalRead(eA);
    ab[1] = digitalRead(eB);                                             //read encoder
    for (currentRotation = 0; currentRotation < 3; currentRotation++) {  //go through discmap 0..2
      uint8_t searchIndex = Mod(4, currentRotation - rotShift);
      if ((discMap[0][searchIndex] == ab[0]) && (discMap[1][searchIndex] == ab[1])) {
        // Serial.print("Assumed: ");
        // Serial.println(searchIndex);
        return;
      }
    }
    // Serial.print("Assumed: ");
    currentRotation = 7;  //if 3 call it 7
    // Serial.print(currentRotation);
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
    orientationUpdate();  //check orientation
    if (enableChase) {
      switch (jamStatus) {
        default:
          chase();  //chase in case
          break;
        case 1:
          chase();  //chase and try catch jam
          bool jamDetected;
          if (currentRotation % 2) {
            jamDetected = jamTimer.isTimePassed(300);  //1 diagonal
          } else {
            jamDetected = jamTimer.isTimePassed(100);  //0 straight
          }
          if (jamDetected) {
            // Serial.println("JAM");
            jamStatus = 2;
            jamTimer.resetTime();
            runRotationMotor(!chaseDirection, chaseBasePower);
          }
          break;
        case 2:                              //unjam maneuver
          if (jamTimer.isTimePassed(250)) {  //stop unjaming timer
            jamTimer.resetTime();
            jamStatus = 1;
            runRotationMotor(0, 0);
            shouldCheckIfActionIsNeeded = true;
          }
      }
    } else if (arrivalState == 3) {  //discoMod
      // Serial.print("d");
      discUpdate();
    }
    servoGrab.update();  //update servo prediction
  }
  void setChasePower(uint8_t base, uint8_t min, float dec) {
    chaseBasePower = base;
    chaseMinPower = min;
    chaseDecrement = dec;
  }
  void changeLimit(int8_t i, int8_t d) {
    grabPositions[i] += d;
  }
  void getTogether() {
    assumeRotation();
    setChase(true);
    setTarget(0, false);
  }
  bool servoReady() {
    return servoGrab.ready();
  }
  void open() {
    setGrab(1, 15);
  }
  bool nextQ() {
    return !(disco->getQIndex(1 - ((currentRotation / 2) - disco->startOA)));
  }
  void discUpdate() {                              //read timings and control speed
    if (shouldCheckIfActionIsNeeded && nextQ()) {  //after rotation update
      // Serial.println("NextQ");
      disco->setQTime(&arrivalTime);
      arrivalTime.resetTime();      //enter time for past Q
      disco->incQIndex();           //inc Q index
      if (disco->getQIndex() == 0)  //if full circle inc rotations
      {
        disco->rotations++;
      }
      uint16_t sum = arSum<uint16_t>(disco->quarters, 4);
      uint16_t delta = sum - disco->lastRound;
      disco->lowError = abs(delta) <= 1;
      disco->lastRound = sum;
    }
  }
  bool isTimeToShoot(uint8_t targetOA, float phaseShiftRelativeToQuarter, float sizeRelativeToQuarter) {
    //                                     Middle of target window
    //                                              |
    //                          window left border  |  window right border
    //                                           |<-+->|=sizeRelativeToQuarter
    //                                           |  T  |
    //         phaseShiftRelativeToQuarter=|<----|->|  |
    //           ------[-1]---------------[0]----[windw]----[1]--
    //           previous=current       targetOA            next
    //localeRelativeTime=|------->|        |
    //                            |<-------|=timePassed+distanceToCurrent(=-1)
    if (disco->lowError && disco->lastLowError())  //condition of low error at this moment
    {
      int8_t currIndex = disco->getQIndex();                                                                 //current index of disco
      float localeRelativeTime = (float(arrivalTime.timePassed()) + colorLag) / disco->quarters[currIndex];  //relative time in current Q
      sizeRelativeToQuarter /= 2;                                                                            //make half of width
      localeRelativeTime += cycleDistanceVector(targetOA, currIndex, 4);                                     //time in target Q axis
      //    0  1    T+1C
      //           -1 -2
      //    3  2    C+2
      //if locale is left of target it will decrease, else increase
      return ((phaseShiftRelativeToQuarter - sizeRelativeToQuarter) <= localeRelativeTime) && (localeRelativeTime <= (phaseShiftRelativeToQuarter + sizeRelativeToQuarter));
      //if [localeRelativeTime] between window borders that means time to shoot cause time window is NOW
    }
    return false;
  }
  bool isDisco() {
    return arrivalState == 3;
  }
  void discoMode(bool ON) {
    if (isDisco() != ON)
      if (ON) {                                      //turning ON must be arrived at even rotation
        disco = new DiscoData(currentRotation / 2);  //create DD
        Serial.println("DISCO MODE");
        enableChase = false;  //turn off chase
        arrivalState = 3;     //mark DM
        runRotationMotor(1, 220);
        waitTime(300);
        runRotationMotor(1, 255);
        arrivalTime.resetTime();
      } else {         //turniong OFF
        delete disco;  //delete DD
        resetChase();
        runRotationMotor(0, 0);  //turn of rotation
        waitTime(500);           //wait for claw to stop
      }
  }
  friend class StandBy;
  friend struct ClawSetting;
  friend void setup();
};