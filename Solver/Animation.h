
#pragma once
#include "TimeManager.h"
#include "Updatable.h"

struct Function {
  whatHere(float t) {}
};

struct Lerp : Function {
public:
  float key[2];
public:
  Lerp() {}
  Lerp(float _a, float _b) {
    key[0] = _a;
    key[1] = _b;
  }
  virtual float whatHere(float t) {
    return (key[0] * (1 - t) + key[1] * t);
  }
};
struct Lerp2 : Function {  //Bilinear interpolation
public:
  float key[3];
public:
  Lerp2() {}
  Lerp2(float _a, float _b, float _c) {
    key[0] = _a;
    key[1] = _b;
    key[2] = _c;
  }
  virtual float whatHere(float t) {
    float _t = 1 - t;
    return (key[0] * _t * _t + 2 * key[1] * t * _t + key[2] * t * t);
  }
};
struct Animation : IUpdatable {
  Lerp *law;
  Timer *timing;
  float value;
  void update() {  //updates value according to law and timing
    value = law->whatHere(timing->spanPassed());
  }
};