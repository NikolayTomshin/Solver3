#include "USBAPI.h"
#include "Arduino.h"
#pragma once
class ITimeManager {
public:
  ITimeManager() {}
  ITimeManager(long _startTime) {}
  void resetTime() {}
  long getStartTime() {}
  long timePassed() {}
  long loop() {}
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
  resetTime() {  //make starting time the current time
    startTime = millis();
  }
  long getStartTime() {
    return startTime;
  }
  long timePassed() {  //Time passed from the startTime
    return (millis() - startTime);
  }
  long loop() {                //time passed since creation or last call (auto reset)
    long thisTime = millis();  //keep time in variable for faster operation?
    long _timePassed = timePassed();
    startTime = thisTime;
    return (_timePassed);
  }
  void loog() {  //Log loop
    Serial.println(loop());
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

class Timer : TimeManager {  //TimeManager with dedicated target time variable to save time on loading target time every time
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