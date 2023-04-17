#pragma once
#include "Cs.h"
//second  file for math structures
//About vectors

namespace V {
int8_t GetNextOv(int8_t v);  //
}

struct Vec {
  int8_t c[3];
  void Set(int8_t *C[3]) {  //set cords by pointer to array
    for (uint8_t i = 0; i < 3; i++) {
      c[i] = C[i];
    }
  }
  Vec(int8_t x = 0, int8_t y = 0, int8_t z = 0) {
    SetC(x, y, z);
  }
  void SetC(int8_t x = 0, int8_t y = 0, int8_t z = 0) {  //set cords by values
    c[0] = x;
    c[1] = y;
    c[2] = z;
  }
  void CrosA(Vec *b) {  //Result of this vector being A
    int8_t tempCoordinate[3] = { 0, 0, 0 };
    tempCoordinate[0] = c[1] * b->c[2] - c[2] * b->c[1];
    tempCoordinate[0] = c[2] * b->c[0] - c[0] * b->c[2];
    tempCoordinate[0] = c[0] * b->c[1] - c[1] * b->c[0];
    for (int8_t i = 0; i < 2; i++) {
      c[i] = tempCoordinate[i];
    }
  }
  void CrosB(Vec *a) {  //Result of this vector being B
    int8_t tempCoordinate[3] = { 0, 0, 0 };
    tempCoordinate[0] = c[2] * a->c[1] - c[1] * a->c[2];
    tempCoordinate[0] = c[0] * a->c[2] - c[2] * a->c[0];
    tempCoordinate[0] = c[1] * a->c[0] - c[0] * a->c[1];
    for (int8_t i = 0; i < 2; i++) {
      c[i] = tempCoordinate[i];
    }
  }
  static int8_t Scal(Vec *a, Vec *b) {  //dot product
    int8_t s = 0;
    for (int8_t i = 0; i < 3; i++)
      s += ((a->c[i]) * (b->c[i]));
    return (s);
  }
  void Transform(Cs *cs) {  //Transform this vector to this CS.  This vector viewed in ACS if constructed in CS.
    int8_t temp[3];
    for (int8_t i = 0; i < 3; i++) {  //Save cords
      temp[i] = c[i];
    }
    for (int8_t i = 0; i < 3; i++) {      //for each direction of cs basis assign coordinates
      int8_t ov = cs->getComponent(i);    //current ov of cs
      c[ov % 3] = Sign(ov / 3, temp[i]);  //ov assigns coresponding coordinate of vec to coresponding direction ov%3-coordinate Sign(ov/3)- *(-1) if negative
    }
  }
  void Untransform(Cs *cs) {  //Untransform this vector to CS. This vector viewed in CS if constructed in ACS.
    int8_t temp[3];
    for (int8_t i = 0; i < 3; i++) {  //Save cords
      temp[i] = c[i];
    }
    for (int8_t i = 0; i < 3; i++) {  //each coordinate assigns original coordinate coresponding to direction of coresponding ov of cs
      int8_t ov = cs->getComponent(i);
      c[i] = Sign(ov / 3, temp[ov % 3]);
    }
  }
  void rotate(int8_t axisOVIndex, int8_t rightAngles) {  //Rotates vector along ov axis for specified number of orto angles
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
    rightAngles = Mod(4, rightAngles);  //Cut oa
    x = V::GetNextOv(axisOVIndex);      //index of "x"
    y = V::GetNextOv(x);                //index of "y"
    x = x % 3;
    y = y % 3;
    if (rightAngles < 2) {
      if (rightAngles > 0) {
        c[y] = temp[x];
        c[x] = -temp[y];
      }
    } else {
      if (rightAngles < 3) {
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
  void Cords() {  //log cords of vector
    Serial.print("(");
    Serial.print(c[0]);
    Serial.print(";");
    Serial.print(c[1]);
    Serial.print(";");
    Serial.print(c[2]);
    Serial.println(")");
  }
  void Add(Vec *v, int8_t multiplier = 1) {  //Add vector*m to this vector
    for (int8_t i = 0; i < 3; i++)
      c[i] += v->c[i] * multiplier;
  }
  uint8_t norma() {
    return (abs(c[0]) + abs(c[1]) + abs(c[2]));
  }
};

const Vec Ovecs[6] = { Vec(1, 0, 0), Vec(0, 1, 0), Vec(0, 0, 1), Vec(-1, 0, 0), Vec(0, -1, 0), Vec(0, 0, -1) };

namespace V {
//Ovecs part next. Ovecs - orto-vectors - unit vectors collinear to axis of cartesian coordinates.
//numbers from 0 to 5 correspond to 6 ortovectors: i,j,k=0,1,2; -i,-j,-k=3,4,5
//Ovecs[6] contains prepared Vec objects with coordinates of every Ovec with corresponding indexes
int8_t GetON(Vec v) {  //returns number of supposed ortovector. -1 if error
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
int8_t Cord(int8_t dimensionIndex, int8_t ortovectorIndex) {  //returns value of dIndexed coordinate  of indexed ortovactor
  return (BMult(Mod3(ortovectorIndex) == dimensionIndex, Sign(ortovectorIndex > 2, 1)));
}
void OVSetup() {  //initialize ortovector prototypes again if you want
  for (int8_t i = 0; i < 6; i++)
    Ovecs[i].SetC(Cord(0, i), Cord(1, i), Cord(2, i));
}
int8_t GetNextOv(int8_t ortovectorIndex) {  //gets next ov index in cycles ( i -> j -> k ->) or ( -i -> -j -> -k ->)
  if (ortovectorIndex < 3) return (Mod3(ortovectorIndex + 1));
  else return (3 + Mod3(ortovectorIndex - 1));
}
void OvecsCheck() {  //debug function to check if initialized right ovecs: i,j,k, -(i,j,k)
  Serial.println("Ovecs:");
  for (uint8_t i = 0; i < 6; i++) {
    Serial.print(i);
    Ovecs[i].Cords();
  }
}
}

struct cubeVec {  //low memory storage of cube vector
  uint8_t encodedValue;
  cubeVec(Vec value) {  //creation
    for (uint8_t i = 0; i < 3; i++) {
      bitCoding::writeBased(3, value.c[i] + 1, i, encodedValue);
    }
  }
  Vec Def() {  //get vec
    Vec V;
    for (uint8_t i = 0; i < 3; i++) {
      V.c[i] = bitCoding::getBased(3, i, encodedValue) - 1;
    }
    return (V);
  }
};