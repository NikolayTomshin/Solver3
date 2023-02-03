#pragma once

class TimeManager {  //base class for measuring flow of time
protected:
  long startTime;  //starting point in time, relative zero on timeline etc.
public:
  TimeManager() {
    resetTime();
  }
  void resetTime() {  //make starting time the current time
    startTime = millis();
  }
  long timePassed() {  //Time passed from the startTime
    return millis() - startTime;
  }
  long loop() {                //time passed since creation or last call (auto reset)
    long thisTime = millis();  //keep time in variable for faster operation?
    long timePassed = thisTime - startTime;
    startTime = thisTime;
    return (timePassed);
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
public:
  long targetTime;
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
