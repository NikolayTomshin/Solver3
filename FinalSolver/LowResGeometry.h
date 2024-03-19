#pragma once
#include <stdint.h>
#include "BitCoding.h"
#include "Utilities.h"
#include "IO.h"
//small 3d vectors, cs and orientations

// #define RotDebug
// #define AxisDebug
// #define CsDebug
// #define OrientationDebug
// #define OrientationSearch
/*
Library for working with small 3d space vectors and ortogonal CS transformations.
*/
class Orientation;
class Cs;
class Rotation;
class OrtAng;
class Vec {
  int8_t component[3];
public:
  Vec() {}
  Vec(int8_t x, int8_t y, int8_t z);

  const int8_t& operator[](uint8_t other) const;
  int8_t& operator[](uint8_t other);  //access component

  int8_t operator*(const Vec& other) const;            //dot product
  static Vec xP(const Vec& first, const Vec& second);  //cross product


  Vec& operator+=(const Vec& other);      //add to this
  Vec operator+(const Vec& other) const;  //sum
  Vec operator-(const Vec& other) const;  //sum
  Vec operator-() const;
  Vec& operator-=(const Vec& other);  //add to this

  Vec& operator*=(int8_t other);      //multiply this
  Vec operator*(int8_t other) const;  //multiplication result

  Vec& operator*=(const Cs& other);      //transform this
  Vec& operator/=(const Cs& other);      //untransform this
  Vec operator*(const Cs& other) const;  //transform copy
  Vec operator/(const Cs& other) const;  //untransform copy

  Vec& operator*=(const Rotation& other);      //rotate this
  Vec& operator/=(const Rotation& other);      //reverse rotate this
  Vec operator*(const Rotation& other) const;  //rotation result
  Vec operator/(const Rotation& other) const;  //reverse rotation result

  Vec& operator*=(const Orientation& other);      //rotate this
  Vec& operator/=(const Orientation& other);      //reverse rotate this
  Vec operator*(const Orientation& other) const;  //rotation result
  Vec operator/(const Orientation& other) const;  //reverse rotation result

  bool operator==(const Vec& other) const;

  bool isOneDimensional() const;
  bool isTwoDimensional() const;
  void print();
private:
  static void ortoRotate(int8_t& x, int8_t& y, OrtAng angle);
};
Vec operator*(int8_t i, const Vec& other);  //multiply by constant on the left

class Axis {
private:
  uint8_t code;
public:
  enum Direction {
    X = 0,
    Y = 1,
    Z = 2,
    nX = 3,
    nY = 4,
    nZ = 5
  };
  Axis(Direction dir);
  Axis(uint8_t ax = 0);
  Axis(uint8_t dimensionIndex, bool positive);
  Axis(const Vec& oneDimensionalVector);

  static Axis throughZeroDimension(const Vec& flat);
  static Axis rightOrt(const Axis& i, const Axis& j);
  static Axis lefttOrt(const Axis& i, const Axis& j);

  Axis operator=(const Axis& other);       //for Cs
  Axis operator=(uint8_t other);           //for Cs
  Axis operator*=(const Rotation& other);  //rotate
  Axis operator/=(const Rotation& other);  //rotate reverse
  Axis operator*(const Rotation& other) const;
  Axis operator/(const Rotation& other) const;
  Axis operator*=(const Cs& other);  //transform
  Axis operator/=(const Cs& other);  //untransform
  Axis operator-();
  operator uint8_t() const;


  Vec getUnitVec() const;
  uint8_t getDIndex() const;
  bool isPositive() const;
  void printLetter() const;

  static Axis maxDimension(const Vec& vec);
};

class OrtAng {
  uint8_t code;
public:
  OrtAng(int8_t oa);
  void setAngle(int8_t oa);
  OrtAng& reverse();
  operator uint8_t() const;
  operator int8_t() const;
  OrtAng operator-() const;
};
class Rotation {
protected:
  uint8_t code;
public:
  Rotation() {
    code = 0;
  }
  Rotation(uint8_t code) {
    this->code = code & 0b00011111;
  }
  Rotation(const Axis& ax, const OrtAng& oa = 1);
  void setAxis(const Axis& ax);
  void setOrtAng(const OrtAng& oa);
  OrtAng getOA() const;
  Axis getAx() const;
  Rotation operator-() const;
  operator uint8_t() const;
};

class Cs {
  uint8_t code[2];
public:
  Cs();
  Cs(const Axis& i, const Axis& j, const Axis& k);
  Cs(uint8_t i, uint8_t j, uint8_t k);
  Cs(const Axis& i, const Axis& j, int8_t shift = 0);
  Cs(const Axis& k);
  void shiftComponents(int8_t shift);

  Axis operator[](uint8_t other) const;
  uint8_t getComponentCode(uint8_t componentIndex) const;  //axis code

  void setComponent(uint8_t index, const Axis& ax);
  void setComponents(const Axis& i, const Axis& j, const Axis& k);
  void setComponents(uint8_t i, uint8_t j, uint8_t k);

  Cs& operator*=(const Rotation& other);
  Cs& operator/=(const Rotation& other);
  Cs operator*(const Rotation& other) const;
  Cs operator/(const Rotation& other) const;

  Cs& operator*=(const Cs& other);      //transform
  Cs& operator/=(const Cs& other);      //untransform
  Cs operator*(const Cs& other) const;  //transform
  Cs operator/(const Cs& other) const;  //untransform

  bool operator==(const Cs& other) const;
  bool operator!=(const Cs& other) const;

  void print();
};

class Orientation {
protected:
  uint8_t index;
private:
  class OrientationNode : public Cs {
    uint8_t linksCode[4];
  public:
    OrientationNode(const Cs& cs, uint8_t x, uint8_t y, uint8_t z, uint8_t _x, uint8_t _y, uint8_t _z);
    uint8_t getLink(uint8_t rotIndex) const;
  };
  static const OrientationNode space[24];
protected:
  static int8_t indexOf(const Cs& cs);
  void rotate(uint8_t rotIndex);
public:
  Orientation(uint8_t _index);
  Orientation();
  Orientation(const Cs& cs);
  Orientation(const Axis& i, const Axis& j, int8_t shift = 0);
  void setOrientation(const Orientation& other);
  operator uint8_t() const;
  const Cs& getCs() const;

  Orientation& operator*=(const Rotation& other);
  Orientation& operator/=(const Rotation& other);
  Orientation operator*(const Rotation& other) const;
  Orientation operator/(const Rotation& other) const;

  Orientation& operator*=(const Orientation& other);
  Orientation& operator/=(const Orientation& other);
  Orientation operator*(const Orientation& other) const;
  Orientation operator/(const Orientation& other) const;

  Orientation operator-() const;

  bool isNeutral() const;
  Axis coincidentComponent(const Axis& inDirection) const;
  void print() const;
};
