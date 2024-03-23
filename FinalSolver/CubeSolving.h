#pragma once
#include <stdint.h>
#include "Cube.h"
#include "IO.h"
//various solving algorythms

// #define SvPSDebug
// #define SvDebug
class Formula {
  uint8_t code;
public:
  enum Application : uint8_t {
    WHOLE,
    MIDDLE,
    DEEP,
    NORMAL
  };
  Formula() {}
  Formula(char code) {
    this->code = code;
  }
  Formula(const Rotation& rotation, Application application) {
    code = uint8_t(rotation);
    code |= uint8_t(application) << 5;
  }
  const static Formula Fp();
  const static Formula F2();
  const static Formula Fc();
  const static Formula fp();
  const static Formula f2();
  const static Formula fc();
  const static Formula Bp();
  const static Formula B2();
  const static Formula Bc();
  const static Formula bp();
  const static Formula b2();
  const static Formula bc();
  const static Formula Rp();
  const static Formula R2();
  const static Formula Rc();
  const static Formula rp();
  const static Formula r2();
  const static Formula rc();
  const static Formula Lp();
  const static Formula L2();
  const static Formula Lc();
  const static Formula lp();
  const static Formula l2();
  const static Formula lc();
  const static Formula Up();
  const static Formula U2();
  const static Formula Uc();
  const static Formula up();
  const static Formula u2();
  const static Formula uc();
  const static Formula Dp();
  const static Formula D2();
  const static Formula Dc();
  const static Formula dp();
  const static Formula d2();
  const static Formula dc();
  const static Formula Mp();
  const static Formula M2();
  const static Formula Mc();
  const static Formula Ep();
  const static Formula E2();
  const static Formula Ec();
  const static Formula Sp();
  const static Formula S2();
  const static Formula Sc();
  const static Formula Xp();  //Y
  const static Formula Xc();
  const static Formula Yp();  //Z
  const static Formula Yc();
  const static Formula Zp();  //X
  const static Formula Zc();

  Application getApplication() const {
    return code >> 5;
  }
  explicit operator CubeOperation() const {
    return CubeOperation(code);
  }
  explicit operator Rotation() const {
    return Rotation(code);
  }
  void print() const;

  operator char() const {
    return code;
  }
};

class OperationStack : public ValueStack<CubeOperation> {
public:
  OperationStack() {}
  void print() {
    if (this != NULL) {
#ifdef SvDebug
      poutN(F("Printing solution:"));
#endif
      StackNode* temp = head;
      while (temp != NULL) {
        temp->value.print();
        temp = temp->prev;
      }
      pout(F("[END]"));
    } else {
      pout(F("NULL solution!"));
    }
  }
  OperationStack& scramble(uint8_t moves, long seed = 0);
  void pushOptimised(const CubeOperation& cOp);
};



class CubeSolver {
protected:
  CubeState origin;
  OperationStack solution;
public:
  CubeSolver(const CubeState& state) {
    origin = state;
  }
  CubeSolver(const Cube& state) {
    origin = state;
  }

  void generateSolution();

  uint8_t getSolutionSize() {
    return solution.getSize();
  }
  OperationStack* getSolution() const {
    uint8_t solutionSize = getSolutionSize();
    if (solutionSize) {
#ifdef SvDebug
      pout(F("Getting solution "));
      pout(solutionSize);
      poutN(F(" long."));
#endif
      return solution.Clone();
    }
  }
  void eraseSolution() {
    solution.clear();
  }
protected:
  friend void setup();
};
