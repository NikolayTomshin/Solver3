#pragma once
#include <stdint.h>
#include "Cs.h"
//second  file for math structures
//About vectors

struct Vec {
  int8_t c[3];
  void Set(int8_t *C[3]);
  Vec(int8_t x = 0, int8_t y = 0, int8_t z = 0);
  void SetC(int8_t x = 0, int8_t y = 0, int8_t z = 0);
  void CrosA(Vec *b);
  void CrosB(Vec *a);
  static int8_t Scal(Vec *a, Vec *b);
  void Transform(Cs *cs);
  void Untransform(Cs *cs);
  void rotate(int8_t axisOVIndex, int8_t rightAngles);
  void Cords();
  void Add(Vec *v, int8_t multiplier = 1);
  uint8_t norma();
};

const Vec Ovecs[6] = { Vec(1, 0, 0), Vec(0, 1, 0), Vec(0, 0, 1), Vec(-1, 0, 0), Vec(0, -1, 0), Vec(0, 0, -1) };

namespace V {
//Ovecs part next. Ovecs - orto-vectors - unit vectors collinear to axis of cartesian coordinates.
//numbers from 0 to 5 correspond to 6 ortovectors: i,j,k=0,1,2; -i,-j,-k=3,4,5
//Ovecs[6] contains prepared Vec objects with coordinates of every Ovec with corresponding indexes
int8_t GetON(Vec v);
int8_t Cord(int8_t dimensionIndex, int8_t ortovectorIndex);
void OVSetup();
int8_t GetNextOv(int8_t ortovectorIndex);
void OvecsCheck();
}  //ns V