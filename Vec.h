#pragma once
#include "HardwareSerial.h"
#include <stdint.h>
#include "Mathclasses.h"
#include "Cs.h"

namespace V
{
int8_t GetNextOv(int8_t v);
}

struct Vec {
  int8_t c[3];
  void Set(int8_t C[3]) {
    for (uint8_t i = 0; i < 3; i++)
      c[i] = C[i];
  }
  Vec(int8_t x = 0, int8_t y = 0, int8_t z = 0) {
    SetC(x, y, z);
  }
  void SetC(int8_t x = 0, int8_t y = 0, int8_t z = 0) {
    c[0] = x;
    c[1] = y;
    c[2] = z;
  }
  void CrosA(Vec b) {  //Result of this vector being A
    int8_t tc[3] = { 0, 0, 0 };
    for (int8_t i = 0; i < 2; i++) {
      for (int8_t j = 0; j < 3; j++) {
        tc[j] += Sign(i, (c[Mod3(j + 1 + i)] * b.c[Mod3(j - 1 - i)]));
      }
    }
    Set(tc);
  }
  void CrosB(Vec a) {  //Result of this vector being B
    int8_t tc[3] = { 0, 0, 0 };
    for (int8_t i = 0; i < 2; i++) {
      for (int8_t j = 0; j < 3; j++) {
        tc[j] += Sign(i, (a.c[Mod3(j + 1 + i)] * c[Mod3(j - 1 - i)]));
      }
    }
    Set(tc);
  }
  static int8_t Scal(Vec* a, Vec* b) {
    int8_t s = 0;
    for (int8_t i = 0; i < 3; i++)
      s += ((a->c[i]) * (b->c[i]));
    return (s);
  }
  void Transform(Cs* cs) {  //Transform this vector to this CS.  This vector viewed in ACS if constructed in CS.
    int8_t temp[3];
    for (int8_t i = 0; i < 3; i++) {  //Save cords
      temp[i] = c[i];
    }
    for (int8_t i = 0; i < 3; i++) {      //for each direction of cs basis assign coordinates
      int8_t ov = cs->ON[i];              //current ov of cs
      c[ov % 3] = Sign(ov / 3, temp[i]);  //ov assigns coresponding coordinate of vec to coresponding direction ov%3-coordinate Sign(ov/3)- *(-1) if negative
    }
  }
  void Untransform(Cs* cs) {  //Untransform this vector to CS. This vector viewed in CS if constructed in ACS.
    int8_t temp[3];
    for (int8_t i = 0; i < 3; i++) {  //Save cords
      temp[i] = c[i];
    }
    for (int8_t i = 0; i < 3; i++) {  //each coordinate assigns original coordinate coresponding to direction of coresponding ov of cs
      int8_t ov = cs->ON[i];
      c[i] = Sign(ov / 3, temp[ov % 3]);
    }
  }
  void Rotate(int8_t a, int8_t oa) {  //Rotates vector along ov axis for specified number of orto angles
    int8_t temp[3];
    // Serial.print("Rotating along:");
    // Serial.print(a);
    // Serial.print(" for ");
    // Serial.println(oa);
    // Serial.print("Pre rotation:");
    // Cords();
    for (int8_t i = 0; i < 3; i++) {  //Save cords
      temp[i] = c[i];
    }    
    
    int8_t x;
    int8_t y;
    oa = Mod(4, -oa); //Cut oa
    x =V::GetNextOv(a); //index of "x"
    y = V::GetNextOv(x); //index of "y"
    x=x%3;
    y=y%3;
    if (oa < 2) {
      if (oa > 0) {
        c[y] = temp[x];
        c[x] = -temp[y];
      }
    } else {
      if (oa < 3) {
        c[x] = -c[x];
        c[y] = -c[y];
      } else {
        c[y] = -temp[x];
        c[x] = temp[y];
      }
    }
    // Serial.print("Post rotation:");
    // Cords();
  }
  void Cords() {
    Serial.print("(");
    Serial.print(c[0]);
    Serial.print(";");
    Serial.print(c[1]);
    Serial.print(";");
    Serial.print(c[2]);
    Serial.println(")");
  }
  void Add(Vec* v, int8_t multiplier = 1) {  //Add vector*m to this vector
    for (int8_t i = 0; i < 3; i++)
      c[i] += v->c[i] * multiplier;
  }
};

const Vec Ovecs[6] = { Vec(1, 0, 0), Vec(0, 1, 0), Vec(0, 0, 1), Vec(-1, 0, 0), Vec(0, -1, 0), Vec(0, 0, -1) };

namespace V {
//Ovecs part next. Ovecs - orto-vectors - unit vectors collinear to axis of cartesian coordinates.
//numbers from 0 to 5 correspond to 6 ortovectors: i,j,k=0,1,2; -i,-j,-k=3,4,5
//Ovecs[6] contains prepared Vec objects with coordinates of every Ovec with corresponding indexes
//GetON(Vec* v) returns number of supposed ortovector.
int8_t GetON(Vec v) {
  // Serial.print("Getting Ovec index ");
  for (uint8_t i = 0; i < 3; i++) {
    int8_t a = v.c[i];
    if (a != 0) {
      // Serial.print("cord index=");
      // Serial.print(i);
      // Serial.print("; value=");
      // Serial.print(a);
      // Serial.print("; Assuming Ov index=");
      // Serial.println(i + ((a < 0) ? 3 : 0));
      return (i + ((a < 0) ? 3 : 0));
    }
  }
  return (-1);
}
int8_t Cord(int8_t c, int8_t i) {
  return (BMult(Mod3(i) == c, Sign(i > 2, 1)));
}
void OVSetup() {
  for (int8_t i = 0; i < 6; i++)
    Ovecs[i].SetC(Cord(0, i), Cord(1, i), Cord(2, i));
}
int8_t GetNextOv(int8_t v) {
  if (v < 3) return (Mod3(v + 1));
  else return (3 + Mod3(v - 1));
}
void OvecsCheck() {
  Serial.println("Ovecs:");
  for (uint8_t i = 0; i < 6; i++) {
    Serial.print(i);
    Ovecs[i].Cords();
  }
}
}