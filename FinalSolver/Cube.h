#pragma once
#include "LowResGeometry.h"
#include "BitCoding.h"
#include <stdint.h>
#include "IO.h"
//stroring cube

// #define IteratorDebug
// #define PrintDebug
// #define CubeStateOperationDebug
// #define PenaltyDebug
// #define ColorDebug
// #define PalletDebug
// #define PalletComparisonDebug
#define ScrambleLog
// #define ColorsWritingDebug

class CubeOperation : public Rotation {
public:
  virtual void print();  //print as formula
  operator Rotation() const;
  CubeOperation() {
    code = 0;
  }
  CubeOperation(uint8_t code)
    : Rotation(code) {}
  CubeOperation(const Axis& ax, const OrtAng& oa = 1);
  CubeOperation(const Rotation& other);
};
class Vec8Iterator : public FunctionIterator<Vec> {
private:
  static Vec helper(uint8_t index){
    return 
  }
protected:
  int8_t z  
public:
  Vec8Iterator(int8_t z_, int8_t shift_ = 0):FunctionIterator() {}
  Vec sticker8();
private:
  friend class CubeSidePieceIterator;
};

class CubeSidePieceIterator : public FunctionIterator<Vec> {
protected:
  Vec value;                  //prepared vec
  Vec8Iterator sideIterator;  //side iterator
  Cs sideCs;                  //cs to transform
public:
  CubeSidePieceIterator();
  CubeSidePieceIterator(Axis side);

  CubeSidePieceIterator& operator+=(int8_t other) override;
  CubeSidePieceIterator& operator-=(int8_t other) override;

  bool isEnd() const override;
  bool isLoop() override;
  uint8_t getIteration();
private:
  void updateVec();
};

class ICubeState {
public:
  virtual Orientation getOrientation(const Vec& position) const = 0;

  virtual void operator*=(const CubeOperation& other) = 0;
  virtual void operator/=(const CubeOperation& other) = 0;
  virtual bool isSolved() const = 0;

  void scramble(uint8_t moves);

  void printUnfolded(const Orientation& rotateAs);
  void printAxonometric(const Orientation& rotateAs);
  virtual void printList() = 0;

  uint16_t statePenalty() const;
  virtual void reset() = 0;
  static Vec vectorOf(uint8_t index);
  Axis sideOfSelection(const Vec& selection);
protected:
  static uint8_t indexOf(const Vec& position);
  static uint8_t cornerPenalty(const Orientation& i);
  static uint8_t edgePenalty(const Orientation& i);
  virtual char letterOfSide(const Axis& side) const;
  friend void setup();
};

class CubeState;

class Cube : public ICubeState {
public:
  class Piece : public Orientation {
    Vec origin;
  public:
    Piece() {}
    Piece(const Vec& origin_, const Orientation& orientation);
    const Vec& getOrigin() const;
    Vec getPosition() const;
  };
private:
  Piece cubelets[20];
public:

  Cube();
  Cube& operator=(const CubeState& other);

  Orientation getOrientation(const Vec& position) const;
  uint8_t operator[](const Vec& pos);            //long search
  const Piece& operator[](uint8_t index) const;  //access pieces
  Piece& operator[](uint8_t index);
  bool isSolved() const;

  void printList() override;

  void reset();

  void operator*=(const CubeOperation& other) override;  //apply operations
  void operator/=(const CubeOperation& other) override;  //apply operations

  friend class CubeState;
  friend class CubeColors;
};


class CubeState : public ICubeState {  //container for piece Orientations and Positions
  uint8_t orientationsByPosition[13];
  uint8_t pieceIndexesByPosition[13];
public:
  CubeState() {}
  CubeState(const Cube& model);
  CubeState operator=(const Cube& model);

  Orientation getOrientation(const Vec& position) const;
  uint8_t getPieceIndex(const Vec& position) const;

  void setPieceOrientation(const Vec& position, const Orientation& orientation);
  void setPieceIndex(const Vec& position, uint8_t pieceIndex);

  bool isSolved() const;

  void printList() override;
  void reset();

  void operator*=(const CubeOperation& other) override;  //apply operations
  void operator/=(const CubeOperation& other) override;  //apply operations

protected:
  void setValue(uint8_t* array, uint8_t value, uint8_t index);
  uint8_t getValue(uint8_t* array, uint8_t index);
  void setCube(Cube& other) const;
private:

  friend class Cube;
};

class Color {
  uint16_t component[3];
public:
  Color() {
#ifdef ColorDebug
    Serial.print(F("+Color!"));
#endif
  }
  Color(uint16_t r, uint16_t g, uint16_t b) {
    component[0] = r;
    component[1] = g;
    component[2] = b;
#ifdef ColorDebug
    Serial.print(F("+Color!"));
#endif
  }
  static Color Red() {
    return Color(255, 0, 0);
  }
  static Color Green() {
    return Color(0, 255, 0);
  }
  static Color Blue() {
    return Color(0, 0, 255);
  }
  static Color White() {
    return Color(255, 255, 255);
  }
  static Color Black() {
    return Color(25, 25, 25);
  }
  static Color Yellow() {
    return Color(255, 255, 0);
  }
  static Color Orange() {
    return Color(255, 140, 0);
  }
#ifdef ColorDebug
  ~Color() {
    Serial.print(F("~Color!"));
  }
#endif
  uint16_t& operator[](uint8_t indexRGB) {
    return component[indexRGB];
  }
  const uint16_t& operator[](uint8_t indexRGB) const {
    return component[indexRGB];
  }
  uint16_t sum() {
    return component[0] + component[1] + component[2];
  }
  Color& operator+=(const Color& other) {
    component[0] += other[0];
    component[1] += other[1];
    component[2] += other[2];
    return *this;
  }
  template<class T>
  Color& operator/=(const T& other) {
    component[0] /= other;
    component[1] /= other;
    component[2] /= other;
    return *this;
  }
  void print() {
    Serial.print("[");
    for (uint8_t i = 0; i < 3; i++) {
      Serial.print(component[i]);
      if (i < 2) Serial.print(",\t");
    }
    Serial.print(']');
  }
};
class CubeColors {
protected:
  class ColorCode {
  public:
    Color sample;
    float relRGB[3];
    ColorCode(const Color& ref) {
      sample = ref;
#ifdef PalletComparisonDebug
      Serial.print(F("From "));
      ref.print();
      Serial.print(F(" created sample "));
      sample.print();
      Serial.println();
#endif
      float sum = ref.sum();
      for (uint8_t i = 0; i < 3; i++)
        relRGB[i] = (3 * ref[i]) / sum;
    }
    isSimilar(const Color& ref, bool criteria, float margin) {
      ColorCode temp(ref);
#ifdef PalletComparisonDebug
      Serial.print(F("Comparing "));
      sample.print();
      Serial.print(F("\tto\t"));
      temp.sample.print();
      Serial.print("\t");
#endif
      for (uint8_t i = 0; i < 3; i++) {
        if (!criteria) {  //relative
          if (!inMargin(relRGB[i], temp.relRGB[i], margin)) {
#ifdef PalletComparisonDebug
            Serial.print(relRGB[i]);
            Serial.print(F(" more than "));
            Serial.print(margin);
            Serial.print(F(" away from "));
            Serial.println(temp.relRGB[i]);
#endif
            return false;
          }
        } else {  //abs
          if (!inMargin(sample[i], temp.sample[i], uint16_t(larger(sample[i], temp.sample[i]) * margin))) {
#ifdef PalletComparisonDebug
            Serial.print(sample[i]);
            Serial.print(F(" more than "));
            Serial.print(uint16_t(sample[i] * margin));
            Serial.print(F(" away from "));
            Serial.println(temp.sample[i]);
#endif
            return false;
          }
        }
      }
      return true;
    }
  };

  Color* array = NULL;        //raw Colors
  uint8_t* normalMap = NULL;  //color indexes(same as Axis)
  Color pallet[6];
  bool initialized = false;
  bool indexed = false;
public:
  CubeColors() {}

  void initialize();
  void deinitialize();

  Color& operator[](Vec selection);

  void fakeScan(ICubeState& target, float noize);
  void createPallet();
  void writeCube(Cube& cube) const;

  static uint8_t indexOf(const Vec& pos);
  static Vec vectorOf(uint8_t i);

  void print() const;
  void printPallet() const;

  void printNormalMap();
protected:
  void setNormal(uint8_t ax, uint8_t index);
  uint8_t getNormal(uint8_t index) const;
  friend void setup();
};