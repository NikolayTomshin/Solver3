#include "Arduino.h"
#include <string.h>
#include <avr/pgmspace.h>
#pragma once
#include "USBAPI.h"
#include <stdint.h>
#include "ClawUnit.h"


ClawUnit n1;  //left claw 1
ClawUnit n2;  //right claw 2
bool clawsReady(uint8_t targetArrival, uint8_t claws) {
  switch (claws) {
    case 0:
      return n1.isArrived(targetArrival);
      break;
    case 1:
      return n2.isArrived(targetArrival);
      break;
    default:
      return n1.isArrived(targetArrival) && n2.isArrived(targetArrival);
  }
}
Timer nextionTimer(30);

#include <Wire.h>
#include "Adafruit_TCS34725.h"
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_1X);
CsT scannerCs = CsT(5, 0, 1);  //x-смотрит на куб, y-в сторону откидывания
class Scanner : public IReady, public IUpdatable {
public:
  MyServo servo;
  bool invertAngles = false;
  uint8_t servoAngles[4] = { 5, 143, 153, 167 };  //folded 0,1,2,3 centre
  // uint8_t positionCode;//unused
  // void setPosition(uint8_t posCode){//unused
  // }
  Scanner() {}
  Scanner(uint8_t sC, uint16_t degreesPerSec) {
    servo.attach(sC);
    servo.teleport(-180.0);
    servo.setSpeed(degreesPerSec);
  }
  void goAngle(uint8_t degrees) {
    Serial.print("A");
    Serial.println(degrees);
    uint8_t angle;
    if (invertAngles) {
      angle = 180 - degrees;
    } else {
      angle = degrees;
    }
    servo.write(angle);
  }
  void goPosition(uint8_t position) {
    Serial.print("SP");
    Serial.println(position);
    goAngle(servoAngles[position]);
  }
  bool ready() {
    return servo.ready();
  }
  void update() {
    servo.update();
  }
};
Scanner scanner;
bool servosReady(uint8_t claws3Scanner) {
  switch (claws3Scanner) {
    case 0:
      return n1.servoReady();
      break;
    case 1:
      return n2.servoReady();
      break;
    case 2:
      return (n1.servoReady() && n2.servoReady());
      break;
    default:
      return scanner.ready();
  }
}
void waitScanner() {
  scanner.wait(systemUpdate);
}
void waitAnything(uint8_t arrival3Grab /*0,1,2|3*/, uint8_t one2Both3Scanner) {
  // Serial.write('(');
  // Serial.print(arrival3Grab);
  // Serial.write(';');
  // Serial.print(one2Both3Scanner);
  // Serial.write(')');
  if (arrival3Grab < 3) {
    // Serial.println("Waiting arrival");
    while (!clawsReady(arrival3Grab, one2Both3Scanner))  //arrival
      systemUpdate();
  } else if (one2Both3Scanner < 4) {  //3 servos //claw or both
    // Serial.println("Waiting grabs");
    while (!servosReady(one2Both3Scanner)) {
      // Serial.print("wG");
      systemUpdate();
    }
  } else {  //wait for all simulteneously why ever?
    // Serial.println("Waiting all servos");
    while (!(servosReady(2) && servosReady(3)))
      systemUpdate();
  }
}
void waitAll() {
  waitAnything(3, 2);
  Serial.print("S");
  waitScanner();
  Serial.print("-S");
  waitAnything(2, 2);  //arrival
  Serial.println("rot");
}
void syncGrab(uint8_t position, uint8_t minusDelta = 0) {
  n1.setGrab(position, minusDelta);
  n2.setGrab(position, minusDelta);
  waitAnything(3, 2);
}
void open() {
  syncGrab(1, 15);
}
void setChase(bool chase) {
  n1.setChase(chase);
  n2.setChase(chase);
}
//nextion work
void comEnd() {  //Nextion command rermination
  Serial1.print("\xFF\xFF\xFF");
}
void loadSomething(String objName, String propName, String value, bool brackets = false) {  //{objName}.{propName}={value}
  comEnd();
  Serial1.print(objName);
  Serial1.write('.');
  Serial1.print(propName);
  Serial1.write('=');
  if (brackets) {
    Serial1.write('"');
    Serial1.print(value);
    Serial1.write('"');
  } else
    Serial1.print(value);
  comEnd();
}
void loadTxt(String name, String txt) {  //update text  {name}.txt="{txt}"
  loadSomething(name, "txt", txt, true);
}
void loadVal(String name, uint16_t val) {  //{name}.val={val}
  loadSomething(name, "val", String(val), false);
}
void callFunction(String name, String par0 = "") {  //{name} {pars}
  comEnd();
  Serial1.print(name);
  Serial1.write(' ');
  Serial1.print(par0);
}
void addParametre(String par) {
  Serial1.write(',');
  Serial1.print(par);
}
void click(String name, bool press) {  //click {name},[press/release]
  callFunction("click", name);
  addParametre(String(press));
  comEnd();
}
void goPage(String pagename) {
  callFunction("page", pagename);
  comEnd();
}
String letterIndex(String letter, uint8_t index) {
  return (letter + String(index));
}
void loadSlider(uint8_t number, uint8_t value) {
  String name = letterIndex("h", number);
  loadVal(name, value);
  click(name, false);
}

class IRState : public IUpdatable {
public:
  virtual void exe(uint8_t commandIndex);
};
IRState* robotState;

void hardUpdate() {
  n2.update();
  n1.update();
}
void serialUpd() {
  if (nextionTimer.isLoop())
    robotState->update();
}
void stateUpd() {
  robotState->update();
}


path::Branch solveTree[20];  //кодирование пути сборки

Cube::State currentState;  //current state
Cube::State newState;      //new state for fast checking
void opDemo() {
  Cube::resetRealPieces();
  currentState.updateCAI();
  while (true) {
    currentState.printSliced(false);
    Serial.print("Penalty:");
    Serial.println(currentState.statePenalty());
    // SCS::Space[i].basis.print();

    Serial.println();
    while (Serial.available() < 2) {}  //waiit for input
    uint8_t ov = Serial.read() - 48, oa = Serial.read() - 48;
    if (ov + oa == 18) break;
    Operation op(ov, oa);
    // i = SCS::getPostOpearationIndex(i, op);
    op.print();
    currentState.applyOperation(op);
  }
}


uint8_t unsignedOAngle(int8_t angle) {  //to unsigned for storage
  return uint8_t(Mod(4, angle));
}
int8_t signedOAngle(uint8_t angle) {  //to signed for usage
  if (angle == 3) return -1;
  return angle;
}

class SubOperation {  //physical operation with cube
  uint8_t code = 0;   //{whole[1],axis[1],angle[2]/CodedExit[4]}
  //if exit only allows rotation/        /if0->no op/if>8 any exit
public:
  SubOperation() {}
private:
  SubOperation(uint8_t _code) {
    code = _code;
  }
public:
  void setOperation(bool whole, uint8_t axis, int8_t angle) {  //set operation pars
    setWhole(whole);                                           //seeting whole
    setAxis(axis);                                             //setting axis
    setAngle(angle);                                           //seting angle(better not zero!)
  }
  void setAnyExit() {    //stop exit not important
    code |= 0b11110000;  //make any exit codition
  }
  SubOperation(bool whole, uint8_t axis, int8_t angle) {  //minimal operation
    setOperation(whole, axis, angle);                     //set op
    setAnyExit();                                         //make any exit
  }
  void setExit(int8_t tX, int8_t tY) {  //encode motorics coordinate
    bitCoding::writeBits(4, 4, ((tX + 1) + (tY + 1) * 3), &code);
  }
  SubOperation(int8_t tX, int8_t tY, bool letRotate = false) {  //no op only exit
    setExit(tX, tY);                                            //set exit
    setAngle(0);                                                //no operation
    if (letRotate) {
      setWhole(true);  //allow rotation
    }
  }
  SubOperation(bool whole, uint8_t axis, int8_t angle, int8_t tX, int8_t tY)  //full suboperation
  {
    setOperation(whole, axis, angle);
    setExit(tX, tY);
  }
  void setWhole(bool whole) {  //rotate whole or side
    bitCoding::writeBits(0, 1, whole, &code);
  }
  void setAxis(uint8_t axisOV) {  //absolute Cs axis ortoindex (pointing from claw) left-Z-2-false; right-~y-4-true
    bitCoding::writeBits(1, 1, axisOV / 4, &code);
  }
  void setAngle(int8_t angle) {  //
    bitCoding::writeBits(2, 2, unsignedOAngle(angle), &code);
  }
  int8_t getAngle() {  //get angle
    return signedOAngle(bitCoding::getBits(2, 2, &code));
  }
  bool isRight() {
    return bitCoding::getBits(1, 1, &code);
  }
  uint8_t getAxis() {
    return (isRight() * 2 + 2);
  }
  bool getWhole() {
    return bitCoding::getBits(0, 1, &code);
  }
  bool isOperation() {
    return getAngle();
  }
  bool exitRequired() {
    return (code >> 4) <= 0b00001000;
  }
  bool isEmpty() {
    return !isOperation() && !exitRequired();
  }
  static SubOperation empty() {
    return SubOperation(0b11110000);
  }
  int8_t getX() {
    return (int8_t(bitCoding::getBits(4, 4, &code) % 3) - 1);
  }
  int8_t getY() {
    return (int8_t(bitCoding::getBits(4, 4, &code) / 3) - 1);
  }
};

ClawUnit* getBlock(bool right) {
  if (right) return &n2;
  else return &n1;
}
uint8_t getClawAxis(bool right) {
  return 2 << right;
}
void grabAction(bool right, uint8_t grabState, uint8_t minusDelta = 0) {
  getBlock(right)->setGrab(grabState, minusDelta);
  waitAnything(3, right);
}
void reGrab() {
  open();
  waitAnything(3, 2);
  syncGrab(1);
  waitAnything(3, 2);
}
void waitIn() {
  Serial.print("waiting");
  while (!Serial.available())
    systemUpdate();
  Serial.read();
}
class ClawMotorics {  //this is very complicated and requires documentation
  /*
  left servo
     -1   ||  0  ||   1  X
          ||     ||
  1  [;]-z||z-R-z||z-[;]
      I   ||  I  ||   I
     <->  ||  |> ||   z
      I   ||  I  ||   I
  0  [;]-z||z-M-z||z-[;]
      I   ||  I  ||   I
      z   || <-> ||  <|>
      I   ||  I  ||   I
  -1 [;]-z||z-L-z||z-[;]
  Y       ||     ||
                 right servo
  
  || - servo action(grab/ungrab) border
  [;] - state where cube is ready to be rotated
  R,M,L - states where sides of cube can be rotated. R and L let rotate only one side
  --,I - possible path between states
  <-> - left side rotation
  |> - right side rotation
  <=> - left claw cube rotation
  <|> - right claw cube rotation
  z - path without performing actions
  */
  int8_t x = 0, y = 0;  //current state
  int8_t stabilityPoints = 20;
  int8_t discoStatus = -1;
  CsT cubeCs;  //ориентация куба изначально не важна, закреплена за центральными клетками которые не считаются деталями
  void checkStability() {
    if (stabilityPoints < 0)
      if (!x) {
        Serial.print("regrab");
        reGrab();
        stabilityPoints = 20;
      }
  }
  void perfectGrab(bool rightMain, bool goingOutside) {
    if (goingOutside) {
      syncGrab(2);               //grab both
      grabAction(rightMain, 0);  //release main
    } else {
      grabAction(rightMain, 2);  //grab good
      syncGrab(1);               //hold both
    }
  }
public:
  ClawMotorics() {}
  bool isDisco() {
    return discoStatus >= 0;
  }
  void setState(uint8_t _x = 0, uint8_t _y = 0) {
    x = _x;
    y = _y;
  }
  void go(SubOperation targetOperation) {  //go do operation
    int8_t tX = 0, tY = 0;                 //temp variables for target cords
    Serial.print("Starting pos");
    stopDisco();
    waitAnything(3, 2);
    waitAnything(1, 2);
    printCords();
    if (targetOperation.isOperation()) {  //do operation via shortest path
      int8_t angle = targetOperation.getAngle();
      bool oddAngle = angle % 2;  //determine angle for later
      bool rightClaw = targetOperation.isRight();
      bool wholeRotation = targetOperation.getWhole();
      Serial.print("Op right:");
      Serial.print(rightClaw);
      tX = BSign(rightClaw, wholeRotation);  //X corresponds with operation type
      Serial.print("\t Cube:");
      Serial.print(wholeRotation);
      Serial.print("\t Angle:");
      Serial.println(angle);
      if (wholeRotation)  //if whole cube then simply same y
        tY = y;
      else  //same y if sign = claw, else 0
        tY = y * (rightClaw == (y > 0));
      zMove(tX, tY);  //go to cords for operation preparation

      checkStability();
      Serial.print("Doing op");
      getBlock(rightClaw)->increaseTarget(2 * angle);  //main movement, because it's ready
      if (wholeRotation) {
        cubeCs.rotate(getClawAxis(rightClaw), angle);
        int8_t relativeY = BSign(rightClaw, y);  //rightClaw=1, leftClaw=-1, opposit to far coordinate
        if (relativeY == 1) {
          getBlock(!rightClaw)->increaseTarget(-2);  //rotate opposite claw to stay in 3x3 state map, if not desirable exit pathfinding will correct this decision
          y = -y;                                    //assign new y
          goto skipYIncrement;
        }
      } else waitAnything(2, rightClaw);
      y += BSign(y == rightClaw, oddAngle);
skipYIncrement:
      printCords();
    }
    if (targetOperation.exitRequired()) {
      tX = targetOperation.getX();
      tY = targetOperation.getY();
      if (x)
        Serial.print("Going to exit");
      Serial.print('(');
      Serial.print(targetOperation.getX());
      Serial.print(';');
      Serial.print(targetOperation.getY());
      Serial.println(')');
      zMove(tX, tY);
      checkStability();
    }
    // waitIn();
  }
  void printCords() {
    Serial.print('(');
    Serial.print(x);
    Serial.print(';');
    Serial.print(y);
    Serial.println(')');
  }
  void stopDisco() {
    if (isDisco()) {
      ClawUnit* subject = getBlock(discoStatus);
      subject->discoMode(false);  //stop discoMode
      subject->setChase(true);
      waitAnything(2, discoStatus);
      discoStatus = -1;
    }
  }
  void goDisco(bool right) {
    if (!isDisco()) {
      int8_t tX = BSign(right, 1);
      go(SubOperation(0, -tX));
      reGrab();
      go(SubOperation(tX, -tX));
      discoStatus = right;
      getBlock(right)->discoMode(true);
    } else Serial.println("Can't start second disco!");
  }
private:
  static uint8_t zIndex(int8_t x, int8_t y) {  //zIndex by coordinates
    if (y) return ((y + 2) * 3 - (x + 2));
    else return (4 + x);  //index of state in snake pattern
  }
  uint8_t currentZIndex() {
    return zIndex(x, y);
  }
  void zMove(int8_t tX, int8_t tY) {  //z-move go to target
    uint8_t targetZIndex = zIndex(tX, tY);
    while (true) {
      uint8_t currentIndex = currentZIndex();
      if (currentIndex == targetZIndex) break;
      zAction(targetZIndex > currentIndex);
    }
  }
  void zAction(bool up) {  //move state through Z path
    uint8_t index = currentZIndex();
    Serial.print("Currently ");
    Serial.println(index);
    printCords();
    //             right servo
    //  [8]|(7)|[7]|(6)|[6]
    //     |||||   |||||(5)
    //  [3]|(3)|[4]|(4)|[5]
    //  (2)|||||   |||||
    //  [2]|(1)|[1]|(0)|[0]
    //left servo
    index -= !up;  //index = code of bridge
    Serial.print("Bridge ");
    Serial.println(index);
    if (index <= 7)  //ok operation
      switch (index) {
        case 2:  //left-Z-2-false; right-~y-4-true
        case 5:  //cases of rotating claw that is not holding cube
          getBlock(index / 5)->increaseTarget(2);
          waitAnything(1, 2);
          y -= BSign(up, 1);
          break;
        default:                                            //case of performing hold/grab action
                                                            //1=7=!3; 0=6=!4;
          bool rightMain = !(index % 2);                    //right servo making big action in a sense that going between 0 and 1 grabState
          bool midleRow = ((index / 3) % 2);                //bridge in middle row meaning direction is reversed
          bool goingOutside = up != midleRow != rightMain;  //going outside when up not midle and left, any change will inverse result
          perfectGrab(rightMain, goingOutside);
          x = x - BSign(rightMain == goingOutside, 1);
          stabilityPoints--;
          break;
      }
    else Serial.println("Impossible Z move!");  //not ok operation
    printCords();
    Serial.println(currentZIndex());
    // waitIn();
  }
};
ClawMotorics motorics;