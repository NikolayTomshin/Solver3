#pragma once
#include "Arduino.h"
#include <Servo.h>
#include "Utilities.h"
#include "Cube.h"
#include "CubeSolving.h"
#include "Settings.h"


void fullSystemUpdate();
void hardwareUpdate();
/*
class IUpdatable
class IReady
class TimeStamp
class Timer : public IReady
class StoppableTimer : public Timer
class LinChase : public IReady, public IUpdatable
class MyServo : public Servo, public LinChase
class MyServo : public Servo, public LinChase
class ClawUnit : public IUpdatable
class Scanner : public IReady, public IUpdatable
class RobotMotorics
class SubOperation
*/

class IUpdatable {
public:
  virtual void update() = 0;
};

class IReady {
public:
  virtual bool ready() const = 0;
  void waitReady();
  void wait(void (*updateFunc)());
};

class TimeStamp {
protected:
  unsigned long value = 0;
public:
  TimeStamp() {}
  TimeStamp(unsigned long value) {
    this->value = value;
  }
  static TimeStamp now() {
    return TimeStamp(millis());
  }
  void setNow();

  bool isTimePassed(unsigned long ms) const;
  static void waitFor(unsigned long time);
  static void waitUntil(TimeStamp time);

  unsigned long timeSince() const;
  operator unsigned long() const;

  bool operator>(const TimeStamp& other) const;
  bool operator<(const TimeStamp& other) const;
  bool operator>=(const TimeStamp& other) const;
  bool operator<=(const TimeStamp& other) const;
  bool operator==(const TimeStamp& other) const;

  unsigned long& operator*();
  const unsigned long& operator*() const;

  TimeStamp& operator+=(unsigned long time);
  TimeStamp& operator-=(unsigned long time);

  void printSince() const;
};

void waitTime(unsigned long time);
void waitUntil(TimeStamp time);

class Timer : public IReady {
protected:
  TimeStamp started;
  unsigned long period;
public:
  Timer() {}
  Timer(const TimeStamp& starting, unsigned long period);
  Timer(unsigned long period);
  Timer(const TimeStamp& ending);
  virtual bool ready() const override;
  void waitReset();

  long timeLeft() const;
  virtual float relativeTimeLeft() const;

  void reset();
};

class StoppableTimer : public Timer {
protected:
  unsigned long pausedTime;
  bool countingTime = true;
public:
  StoppableTimer(const TimeStamp& starting, unsigned long period)
    : Timer(starting, period) {}
  StoppableTimer(unsigned long period)
    : Timer(period) {}
  StoppableTimer(const TimeStamp& ending)
    : Timer(ending) {}
  void setTimer(bool countingTime);
  bool isCounting() const;
  bool ready() const override;
};

class LinChase : public IReady, public IUpdatable {  //Linear chase
  /*Model of point chasing target with const speed in real time.
    Usefull to predict when something real arrives when moving similarly*/
  bool underTarget = false;  //is position less than target
  bool going = false;
  float x = 0;
  uint16_t speed;
  int16_t target = 0;
  uint8_t upMod;
  uint8_t dMod;
  TimeStamp startedTime;
public:
  LinChase(){};
  LinChase(uint16_t unitsSec, uint8_t u = 100, uint8_t d = 100);
  void setSpeed(uint16_t unitsPerSec, uint8_t u = 100, uint8_t d = 100);
  void assumeDirection();
  void teleport(float place);
  void go(int16_t position);
  void update() override;
  bool ready() const override;
};

class MyServo : public Servo, public LinChase {
public:
  MyServo() {}
  MyServo(uint8_t unitsSec, int16_t startPosition, uint8_t u = 100, uint8_t d = 100);
  void write(int value);
};

const uint8_t colorLag = 5;

class RobotMotorics;
class ClawUnit : public IUpdatable, public IConfigurable {  //class for claw module control//ClawUnit//ClawUnit//ClawUnit//ClawUnit//ClawUnit//ClawUnit
protected:
  uint8_t mD, mP, eA, eB, sC;     //Pins
  bool reverseDirection = false;  //reverse direction
  MyServo servoGrab;              //Servo object

  uint8_t grabPositions[3];  //Positions for claw states
  uint8_t targetAngle;
  uint8_t grabState = 0;

  //variables for stable control of rotation
  static const bool discMap[2][4];

  bool ab[2];
  uint8_t rotShift = 0;         //0..3
  uint8_t targetRotation = 0;   //0..7
  uint8_t currentRotation = 0;  //0..7

  bool enableChase = false;  //variables for simple rotation control
  bool shouldCheckIfActionIsNeeded = true;

  uint8_t arrivalState = 0;  //no data 0; over target 1; arrived 2; disco mod 3;

  uint8_t chaseRoutine = 0;  //0 no; 1 counting arrival; 2 ramping up power.
  TimeStamp arrivalTime;     //counting time during flying over

  Lerp1<uint8_t> rampUpLerp;
  Timer rampUpTime;  //timer for ramping up

  uint8_t oscilationNumber = 0;
  TimeStamp oscilationStall;     //timer to count stall during oscilations
  uint8_t oscilationSmooth = 0;  //set true to decrease break of first oscilation

  uint8_t jamStatus = 0;  //0 not interested, 1 catching, 2 doing unjamming manuwer
  TimeStamp jamTimer;     //to get out of jam

  uint8_t chaseBasePower = 255;
  uint8_t chaseMinPower = 100;
  uint8_t chasePower = 0;
  float chaseDecrement = 0.9;
  bool chaseDirection;

  float currentMotorForce;

  static Timer grabDelay;


  virtual const uint8_t numberOfConfigs() const override;  //how many
  virtual Config getConfig(uint8_t index) const override;  //get by index
  struct DiscoData {                                       //DiscoData//DiscoData//DiscoData//DiscoData//DiscoData//DiscoData//DiscoData//DiscoData//DiscoData
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

    DiscoData(uint8_t _startOA);
    ~DiscoData();
    uint8_t getQIndex(int8_t delta = 0) const;
    void incQIndex(int8_t delta = 1);
    long setQTime(TimeStamp& timer, int8_t delta = 0);
    void printQ() const;
    bool lastLowError() const;
  };  //DiscoData//DiscoData//DiscoData//DiscoData//DiscoData//DiscoData//DiscoData//DiscoData//DiscoData//DiscoData//DiscoData

  static DiscoData* disco;  //only for disco time
public:
  ClawUnit() {}
  ClawUnit(uint8_t md, uint8_t mp, uint8_t sc, uint8_t a, uint8_t b);

  void reattach(uint8_t sc);
  void setServoSpeed(uint16_t unitsPerSec, uint8_t u = 100, uint8_t d = 100);
  void SetAngles(uint8_t releas, uint8_t hold, uint8_t grab);

  void runRotationMotor(bool clockwise, uint8_t force);
  void setServo(uint8_t angle);
  void setGrab(uint8_t positionIndex, uint8_t minusDelta = 0);
  void ease();
  void open();
  void retractPartially();  //retract enough to rotate around cube

  void setChase(bool _chase);
  void setChaseRampUp(uint8_t startPercent = 0);
  void setOscilationSmooth(uint8_t smooth = 0);
  void setTarget(int8_t _target, bool enableJam = true);
  void increaseTarget(int8_t delta);
  void changeRotshift(bool positive);

  void setReverse(bool _reverse);
  uint8_t getArrival() const;
  bool isArrived(uint8_t howMuch) const;

  void getTogether();

protected:
  uint8_t oscilationPower() const;
  void resetChase();

  void orientationUpdate();
  void chase();
  void update() override;

public:
  void assumeRotation();
  void logEncoder() const;
  void setChasePower(uint8_t base, uint8_t min, float dec);
  void changeLimit(int8_t i, int8_t d);
  bool servoReady() const;

protected:
  bool nextQ() const;
  void discUpdate();
public:
  bool isTimeToShoot(uint8_t targetOA, float phaseShiftRelativeToQuarter, float sizeRelativeToQuarter) const;
  bool isDisco() const;
  void discoMode(bool ON);

  friend class RobotMotorics;
  friend struct ClawSetting;
  friend void setup();
};

void setLed(bool on);

#include <Wire.h>
#include "Adafruit_TCS34725.h"
class Scanner : public IReady, public IUpdatable {
  Cs scannerCs = Cs(0, 4, 2);  //z-смотрит от куба, y от клешни сканера, x совпадает с x
public:
  Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_1X);
  MyServo servo;
  bool invertAngles = false;
  uint8_t position = 9;
  uint8_t servoAngles[4] = { 20, 42, 70, 80 };  //folded 0,1,2,3 centre
  // uint8_t positionCode;//unused
  // void setPosition(uint8_t posCode){//unused
  // }
  Scanner() {}
  Scanner(uint8_t sC, uint16_t degreesPerSec);
  void goAngle(uint8_t degrees);
  void goPosition(uint8_t _position);
  bool ready() const override;
  void update() override;
  void snap(Color& color);

  friend class RobotMotorics;
};

void waitIn();
class SubOperation {  //physical operation with cube
  uint8_t code = 0;   //{whole[1],axis[1],angle[2]/CodedExit[4]}
  //if exit only allows rotation/        /if0->no op/if>8 any exit
public:
  SubOperation() {}
protected:
  SubOperation(uint8_t _code);
public:
  SubOperation(bool whole, bool rightClaw, const OrtAng& angle);                          //minimal operation
  SubOperation(bool whole, const Axis& axis, const OrtAng& angle);                        //minimal operation
  SubOperation(int8_t tX, int8_t tY, bool letRotate = false);                             //no op only exit
  SubOperation(bool whole, const Axis& axis, const OrtAng& angle, int8_t tX, int8_t tY);  //full suboperation
  static SubOperation empty();
  CubeOperation forCube(const Orientation& cubeCs) const;

  void setOperation(bool whole, const Axis& axis, const OrtAng& angle);  //set operation pars
  void setAnyExit();                                                     //stop exit not important

  void setExit(int8_t tX, int8_t tY);  //encode motorics coordinate
  void setWhole(bool whole);           //rotate whole or side
  void setAxis(const Axis& axisOV);    //absolute Cs axis ortoindex (pointing from claw) left-Z-2-false; right-~y-4-true
  void setAngle(const OrtAng& angle);

  bool getWhole() const;
  Axis getAxis() const;
  OrtAng getAngle() const;
  bool isRight() const;
  bool isOperation() const;
  bool exitRequired() const;
  int8_t getX() const;
  int8_t getY() const;
  bool isEmpty() const;

  void print() const;
};

// #define MotoricsDebug

#define DISCOCLAW false  //left
class RobotMotorics : public IUpdatable, public IConfigurable {
protected:
  int8_t cubeSizeDelta;
  int8_t x = 0, y = 0;  //current state
  uint8_t stabilityHealth = 20;
  int8_t stabilityPoints = 20;
  int8_t discoStatus = -1;

  bool holding = false;
  bool busyClaw = false;
  bool lastWhole = false;
  uint8_t rotationSmoothValue = 0;
  uint8_t maxSmooth;
  uint8_t autoSmooth;

  Orientation cubeCs;  //ориентация куба изначально не важна, закреплена за центральными клетками которые не считаются деталями
  OperationStack* savedOPs = NULL;
  void checkStability();
  void perfectGrab(bool rightMain, bool goingOutside);
public:
  ClawUnit left;
  ClawUnit right;
  Scanner scanner;

  virtual const uint8_t numberOfConfigs() const override;  //how many
  virtual Config getConfig(uint8_t index) const override;  //get by index

  RobotMotorics(int8_t sizeDelta) {
    cubeSizeDelta = sizeDelta;
    pinMode(13, OUTPUT);
  }
  void initialize();  //set values of all hardware
  void allignBoth();  //rotate claws to default state

  void update() override;  //update of hardware

  ClawUnit& getClaw(bool right);       //get claw reference
  Axis getClawAxis(bool right) const;  //get axis of claw

  bool clawsReady(uint8_t targetArrival, uint8_t clawsNumber) const;  //are 0left,1right,2both claws reached target arrival level
  bool servosReady(uint8_t claws3Scanner);                            //are 0left,1right,2both claws;3scanner servos ready

  void waitScanner();                                                             //do harware update until scanner ready
  void waitAnything(uint8_t arrival3Grab /*0,1,2|3*/, uint8_t one2Both3Scanner);  //wait arrival or servos of claws or scanner
  void waitAll();                                                                 //wait for everything stopping

  void grabAction(bool right, uint8_t grabState, uint8_t minusDelta = 0);  //set grab for one claw and wait completion
  void syncGrab(uint8_t position, uint8_t minusDelta = 0);                 //set grab for both claws(with optional negative delta) and wait
  void open();                                                             //sync grab for taking and putting cube in claws
  void setChase(bool chase);                                               //set chase for both claws

  void scannerAction(uint8_t position);  //set scanner to position and wait

  void setState(uint8_t _x = 0, uint8_t _y = 0);  //set motorics state if claws were desynchronized from state
  void reGrab();                                  //regrab cube to restore good grip
  void setSmooth(bool smooth);
  void go(const SubOperation& targetOperation);  //perform operation
  void go(const CubeOperation& cubeOperation);   //perform operation to make cube Operation

  //manual control funcs

  bool letGo();                                 //prepare cube for ejaction
  bool grab();                                  //take cube
  void RAction(bool whole, const OrtAng& ang);  //do something with right claw
  void LAction(bool whole, const OrtAng& ang);  //do something with left claw

  bool isHolding() const;              //is cube held in claws?
  const Orientation& getCube() const;  //orientation of cube
  bool isDisco() const;                //is currently disco happening *disco is special mode for scanning cube

  void printCords() const;  //print current motorics state

  void goDisco(bool right);
  void stopDisco();
  void snapColor(int8_t localX, int8_t localY, Color& targetColor);
  void scanCurrentSide(CubeColors& cubeColors);

  void setHistorySave(OperationStack* savedOPs = NULL);
  void scramble(uint8_t moves, long seed);
  void playBack();
private:
  static uint8_t zIndex(int8_t x, int8_t y);  //get z index of coordinates
  uint8_t currentZIndex() const;              //get z index of current state
  void zMove(int8_t tX, int8_t tY);           //perform actions to reach state by coords and not rotating cube
  void zAction(bool up);                      //perform single z step up or down the z(2)
};