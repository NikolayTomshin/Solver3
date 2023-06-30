#include <string.h>
#include <avr/pgmspace.h>
#pragma once
#include "USBAPI.h"
#include <stdint.h>
#include "ClawUnit.h"

ClawUnit n1;  //left claw 1
ClawUnit n2;  //right claw 2
Timer nextionTimer(10);

CsT scannerCs = CsT(5, 0, 1);  //x-смотрит на куб, y-в сторону откидывания
class Scanner {
public:
  Servo servo;
  bool invertAngles = false;
  uint8_t servoAngles[4] = { 5, 143, 153, 167 };  //folded 0,1,2,3 centre
  // uint8_t positionCode;//unused
  // void setPosition(uint8_t posCode){//unused
  // }
  Scanner() {}
  Scanner(uint8_t sC) {
    servo.attach(sC);
  }
  void goAngle(uint8_t degrees) {
    uint8_t angle;
    if (invertAngles) {
      angle = 180 - degrees;
    } else {
      angle = degrees;
    }
    servo.write(angle);
  }
  void goPosition(uint8_t position) {
    goAngle(servoAngles[position]);
  }
};
Scanner scanner;

CsT cubeCs;  //ориентация куба изначально не важна, закреплена за центральными клетками которые не считаются деталями

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

class IRState:public IUpdatable{};
IRState* robotState;

void hardUpdate() {
  n2.update();
  n1.update();
}
void serialUpd() {
  
}
void stateUpd() {
  robotState->update();
}