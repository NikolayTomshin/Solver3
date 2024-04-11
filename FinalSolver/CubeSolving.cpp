#include "Arduino.h"
#include "CubeSolving.h"

const Formula Formula::Fp() {
  return Formula(Rotation(0, 3), Application::NORMAL);
}
const Formula Formula::F2() {
  return Formula(Rotation(0, 2), Application::NORMAL);
}
const Formula Formula::Fc() {
  return Formula(Rotation(0, 1), Application::NORMAL);
}
const Formula Formula::fp() {
  return Formula(Rotation(0, 3), Application::DEEP);
}
const Formula Formula::f2() {
  return Formula(Rotation(0, 2), Application::DEEP);
}
const Formula Formula::fc() {
  return Formula(Rotation(0, 1), Application::DEEP);
}
const Formula Formula::Bp() {
  return Formula(Rotation(3, 3), Application::NORMAL);
}
const Formula Formula::B2() {
  return Formula(Rotation(3, 2), Application::NORMAL);
}
const Formula Formula::Bc() {
  return Formula(Rotation(3, 1), Application::NORMAL);
}
const Formula Formula::bp() {
  return Formula(Rotation(3, 3), Application::DEEP);
}
const Formula Formula::b2() {
  return Formula(Rotation(3, 2), Application::DEEP);
}
const Formula Formula::bc() {
  return Formula(Rotation(3, 1), Application::DEEP);
}
const Formula Formula::Rp() {
  return Formula(Rotation(1, 3), Application::NORMAL);
}
const Formula Formula::R2() {
  return Formula(Rotation(1, 2), Application::NORMAL);
}
const Formula Formula::Rc() {
  return Formula(Rotation(1, 1), Application::NORMAL);
}
const Formula Formula::rp() {
  return Formula(Rotation(1, 3), Application::DEEP);
}
const Formula Formula::r2() {
  return Formula(Rotation(1, 2), Application::DEEP);
}
const Formula Formula::rc() {
  return Formula(Rotation(1, 1), Application::DEEP);
}
const Formula Formula::Lp() {
  return Formula(Rotation(4, 3), Application::NORMAL);
}
const Formula Formula::L2() {
  return Formula(Rotation(4, 2), Application::NORMAL);
}
const Formula Formula::Lc() {
  return Formula(Rotation(4, 1), Application::NORMAL);
}
const Formula Formula::lp() {
  return Formula(Rotation(4, 3), Application::DEEP);
}
const Formula Formula::l2() {
  return Formula(Rotation(4, 2), Application::DEEP);
}
const Formula Formula::lc() {
  return Formula(Rotation(4, 1), Application::DEEP);
}
const Formula Formula::Up() {
  return Formula(Rotation(2, 3), Application::NORMAL);
}
const Formula Formula::U2() {
  return Formula(Rotation(2, 2), Application::NORMAL);
}
const Formula Formula::Uc() {
  return Formula(Rotation(2, 1), Application::NORMAL);
}
const Formula Formula::up() {
  return Formula(Rotation(2, 3), Application::DEEP);
}
const Formula Formula::u2() {
  return Formula(Rotation(2, 2), Application::DEEP);
}
const Formula Formula::uc() {
  return Formula(Rotation(2, 1), Application::DEEP);
}
const Formula Formula::Dp() {
  return Formula(Rotation(5, 3), Application::NORMAL);
}
const Formula Formula::D2() {
  return Formula(Rotation(5, 2), Application::NORMAL);
}
const Formula Formula::Dc() {
  return Formula(Rotation(5, 1), Application::NORMAL);
}
const Formula Formula::dp() {
  return Formula(Rotation(5, 3), Application::DEEP);
}
const Formula Formula::d2() {
  return Formula(Rotation(5, 2), Application::DEEP);
}
const Formula Formula::dc() {
  return Formula(Rotation(5, 1), Application::DEEP);
}
const Formula Formula::Mp() {
  return Formula(Rotation(1, 1), Application::MIDDLE);
}
const Formula Formula::M2() {
  return Formula(Rotation(1, 2), Application::MIDDLE);
}
const Formula Formula::Mc() {
  return Formula(Rotation(1, 3), Application::MIDDLE);
}
const Formula Formula::Ep() {
  return Formula(Rotation(2, 1), Application::MIDDLE);
}
const Formula Formula::E2() {
  return Formula(Rotation(2, 2), Application::MIDDLE);
}
const Formula Formula::Ec() {
  return Formula(Rotation(2, 3), Application::MIDDLE);
}
const Formula Formula::Sp() {
  return Formula(Rotation(0, 3), Application::MIDDLE);
}
const Formula Formula::S2() {
  return Formula(Rotation(0, 2), Application::MIDDLE);
}
const Formula Formula::Sc() {
  return Formula(Rotation(0, 1), Application::MIDDLE);
}
const Formula Formula::Xp() {
  return Formula(Rotation(1, 3), Application::WHOLE);
}  //Y
const Formula Formula::Xc() {
  return Formula(Rotation(1, 1), Application::WHOLE);
}
const Formula Formula::Yp() {
  return Formula(Rotation(2, 3), Application::WHOLE);
}  //Z
const Formula Formula::Yc() {
  return Formula(Rotation(2, 1), Application::WHOLE);
}
const Formula Formula::Zp() {
  return Formula(Rotation(0, 3), Application::WHOLE);
}  //X
const Formula Formula::Zc() {
  return Formula(Rotation(0, 1), Application::WHOLE);
}

void Formula::print() const {
  pout('[');
  uint8_t ax = code & 0b00000111;
  uint8_t oa = (code >> 3) & 0b00000011;
  if (oa) {
    switch (getApplication()) {
      case Application::NORMAL:
        switch (ax) {
          case 0:
            pout('F');
            break;
          case 1:
            pout('R');
            break;
          case 2:
            pout('U');
            break;
          case 3:
            pout('B');
            break;
          case 4:
            pout('L');
            break;
          case 5:
            pout('D');
            break;
        }
        break;
      case Application::DEEP:
        switch (ax) {
          case 0:
            pout('f');
            break;
          case 1:
            pout('r');
            break;
          case 2:
            pout('u');
            break;
          case 3:
            pout('b');
            break;
          case 4:
            pout('l');
            break;
          case 5:
            pout('d');
            break;
        }
        break;
      case Application::WHOLE:
        switch (ax) {
          case 0:
            pout('Z');
            break;
          case 1:
            pout('X');
            break;
          case 2:
            pout('Y');
            break;
        }
        break;
      case Application::MIDDLE:
        switch (ax) {
          case 0:
            pout('S');
            break;
          case 1:
            pout('M');
            break;
          case 2:
            pout('E');
            break;
        }
        break;
    }
    if (getApplication() == Application::MIDDLE) {
      if (ax) {
        oa = (4 - oa) % 4;
      }
    }
    switch (oa) {
      case 3: pout(' '); break;
      case 2: pout('2'); break;
      case 1: pout('`'); break;
    }
  } else pout("  ");
  pout(']');
}

void OperationStack::pushOptimised(const CubeOperation& cOp) {
  OrtAng thisOA = cOp.getOA();
  if (uint8_t(thisOA)) {
    Axis thisAxis;
    if (getSize()) {
      Axis thisAxis = cOp.getAx();

      CubeOperation& firstLast = item();
      OrtAng firstOA = firstLast.getOA();
      Axis firstAxis = firstLast.getAx();

      if (firstAxis == thisAxis) {
        firstLast.setOrtAng(uint8_t(firstOA) + uint8_t(thisOA));
        if (uint8_t(firstLast.getOA()) == 0) {
          pop();
        }
        return;
      }
      if (getSize() > 1) {
        CubeOperation& secondLast = item(1);
        Axis secondAxis = secondLast.getAx();
        OrtAng secondOA = secondLast.getOA();
        if (firstAxis.getDIndex() == secondAxis.getDIndex()) {
          if (thisAxis == secondAxis) {
            secondLast.setOrtAng(uint8_t(secondOA) + uint8_t(thisOA));
            if (uint8_t(secondLast.getOA()) == 0) {
              CubeOperation temp = pop();  //take last
              pop();                       //remove second
              push(temp);                  //push last
            }
            return;
          }
        }
      }
    }
    Stack<CubeOperation>::push(cOp);  //push this operation finally after all checks for optimisation}
  }
}
OperationStack& OperationStack::scramble(uint8_t moves, long seed) {
  clear();
  randomSeed(seed);
  Axis ax(random(0, 5));
  while (size < moves) {
    ax = Axis(ax + random(1, 5));
    pushOptimised(CubeOperation(ax, random(1, 3)));
  }
  return *this;
}