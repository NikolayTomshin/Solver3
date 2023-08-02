#include <stdint.h>
#pragma once
#include "Updatable.h"
#include <Arduino.h>
class ITimeManager {
public:
  ITimeManager() {}
  ITimeManager(long _startTime) {}
  void resetTime() {}
  long getStartTime() {}
  long timePassed() {}
  long loopMeasure() {}
  void loog() {}
  bool isTimePassed(long targetTime) {}
  bool isLoopPassed(long loopTime) {}
};
class TimeManager : ITimeManager {  //base class for measuring flow of time
protected:
  long startTime = 0;  //starting point in time, relative zero on timeline etc.
public:
  TimeManager() {
    resetTime();
  }
  TimeManager(long _startTime) {
    startTime = _startTime;
  }
  void resetTime() {  //make starting time the current time
    startTime = millis();
  }
  long getStartTime() {
    return startTime;
  }
  long timePassed() {  //Time passed from the startTime
    return (millis() - startTime);
  }
  long loopMeasure() {         //time passed since creation or last call (auto reset)
    long thisTime = millis();  //keep time in variable for faster operation?
    long _timePassed = timePassed();
    startTime = thisTime;
    return (_timePassed);
  }
  void loog() {  //Log loop
    Serial.println(loopMeasure());
  }
  bool isTimePassed(long targetTime) {  //Returns true if enough time passed since the start
    return (timePassed() >= targetTime);
  }
  bool isLoopPassed(long loopTime) {  //Same as isTimePassed but immediatly resets start time
    bool passed = isTimePassed(loopTime);
    if (passed)
      resetTime();
    return (passed);
  }
};

class Timer : public TimeManager, public IReady {  //TimeManager with dedicated target time variable to save time on loading target time every time
protected:
  long targetTime;
public:
  Timer() {}
  Timer(long _targetTime) {
    targetTime = _targetTime;
  }
  long timeRemaining() {
    return targetTime - startTime;
  }
  bool isTime() {  //True if target Time passed
    return (timePassed() >= targetTime);
  }
  bool isLoop() {  //If target Time passed return true and resets time
    bool passed = isTime();
    if (passed)
      resetTime();
    return passed;
  }
  float spanPassed() {  //float value from 0 to infinity of how much time from start to target have passed, usefull for animating things
    return (float(timePassed()) / float((targetTime - startTime)));
  }
  bool ready() {
    return isTime();
  }
};

class StoppableTimer : ITimeManager {
  TimeManager stopped;
  TimeManager counting;
  bool isStopped = false;
public:
  StoppableTimer() {
  }
  void pause() {
    isStopped = true;
    stopped.resetTime();
  }
  void resume() {
    isStopped = false;
    counting = TimeManager(stopped.timePassed() + counting.getStartTime());
  }
  void resetTime() {
    stopped.resetTime();
    counting.resetTime();
  }
  long getStartTime() {
    return counting.getStartTime();
  }
  long timePassed() {
    if (isStopped) {
      return (counting.timePassed() - stopped.timePassed());
    }
    return counting.timePassed();
  }
  long loop() {
    long _timePassed = timePassed();
    resetTime();
    return _timePassed;
  }
  void loog() {
    Serial.print(loop());
  }
  bool isTimePassed(long targetTime) {
    return targetTime <= timePassed();
  }
  bool isLoopPassed(long loopTime) {
    bool passed = isTimePassed(loopTime);
    if (passed) resetTime();
    return passed;
  }
};
#include "Mathclasses.h"
void setLed(bool high) {
  digitalWrite(13, high);
}
class LinChase : public IReady, public IUpdatable {  //Linear chase
                                                     /*Model of point chasing target with const speed in real time. Usefull to predict when something real arrives when moving similarly*/
  bool underTarget = false;                          //is position less than target
  bool going = false;
  float x = 0;
  uint16_t speed;
  int16_t target = 0;
  uint8_t upMod;
  uint8_t dMod;
  TimeManager timer;
public:
  LinChase(){};
  LinChase(uint16_t unitsSec, uint8_t u = 100, uint8_t d = 100) {  //create with defined move speed
    setSpeed(unitsSec, u, d);
  }
  void setSpeed(uint16_t unitsPerSec, uint8_t u = 100, uint8_t d = 100) {  //set speed
    speed = unitsPerSec;
    upMod = u;
    dMod = d;
  }
  void assumeDirection() {  //establish position relative to target
    underTarget = (x < target);
  }
  void teleport(float place) {  //instantly change position
    x = place;
  }
  void go(int16_t position) {  //tell point to move to position
    target = position;         //set target
    assumeDirection();         //know where to move
    going = true;              //decide to go
    timer.resetTime();         //reset timer
  }
  void update() {                        //put in loop to precisely catch moment of arrival
    if (going) {                         //if going
      long deltaT = timer.timePassed();  //count delta time
      // Serial.print("dT=");
      // Serial.println(deltaT);
      if (deltaT) {  //if not 0
        float step = floatSigned(float(speed)
                                   * (float(upMod) / 100.0 * underTarget + float(dMod) / 100.0 * !underTarget)
                                   * deltaT / 1000.0,
                                 underTarget);
        // Serial.print(deltaT);
        // Serial.write('\t');
        // Serial.print(step*1000);
        // Serial.write('\t');
        // Serial.print(step);
        // Serial.write('\t');
        // Serial.println(x);
        x += step;          //add delta to position
        timer.resetTime();  //reset timer
        bool was = underTarget;
        assumeDirection();
        going = was == underTarget;  //if position relative to target didn't change than keep going
        if (going) return;
        x = float(target);  //else stop and fix position
      }
    }
  }
  bool ready() {  //ready when not going
    return (!going);
  }
};
class MyServo : public Servo,
                public LinChase {
public:
  MyServo() {}
  MyServo(uint8_t unitsSec, int16_t startPosition, uint8_t u = 100, uint8_t d = 100)  //Construct as Servo and LinChase
    : Servo::Servo(), LinChase::LinChase(unitsSec, u, d) {
    teleport(startPosition);
  }
  void write(int value) {
    Servo::write(value);
    go(value);
  }
};
void waitTime(long millis) {
  Timer waitFor(millis);
  jdun(&waitFor, systemUpdate);
}
#include "Mathclasses.h"
class PID {
  float p = 1, i = 1, d = 1;
  float error = 0;
  float integral = 0;
  float storedSignal = 0;
  TimeManager timer;
public:
  PID() {}
  PID(float _p, float _i, float _d) {
    p = _p;
    i = _i;
    d = _d;
    timer.resetTime();
  }
  float deltaSignal(float _error) {
    float dTime = float(timer.timePassed());  //delta time
    if (dTime) {                              //if time passed
      integral += _error * dTime;
      storedSignal = p * _error + i * integral + d * (_error - error) / dTime;  //calculate signal
      error = _error;
      timer.resetTime();
    }  //else return old signal
    return (storedSignal);
  }
  void print() {
    Serial.print("Er=");
    Serial.print(error);
    Serial.write('\t');
    Serial.print("In=");
    Serial.print(integral);
    Serial.write('\t');
    Serial.print("Sig=");
    Serial.print(storedSignal);
    Serial.write('\t');
  }
};