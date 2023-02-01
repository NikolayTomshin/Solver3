#pragma once

class TimeManager {
protected:
  long a = 0;
public:
  void Reset() {
    a = millis();
  }
  long Loop() {
    long b = millis();
    long r = b - a;
    a = b;
    return (r);
  }
  void Loog() {
    Serial.println(Loop());
  }
};

class Timer : TimeManager {
  long time;
  bool over() {
    return ((a >= time));
  }
  Timer(long _time) {

    time = _time;
  }
};