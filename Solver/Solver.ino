// #include "ClawUnit.h"
// #include "TimeManager.h"
// #include "CsT.h"
#include "States.h"

void ipause() {
  waitAll();
  setLed(1);
  delay(500);
  setLed(0);
}

void setup() {
  n1 = ClawUnit(7, 6, 11, A2, A3);  //pins
  n2 = ClawUnit(4, 5, 10, A0, A1);  //pins
  n2.changeRotshift(true);
  n2.changeRotshift(true);    //fix right rotation
  n1.SetAngles(0, 138, 152);  //angles left
  n2.SetAngles(4, 140, 155);  //angles right
  n1.setServoSpeed(300, 97, 100);
  n2.setServoSpeed(300, 97, 100);
  n1.setChasePower(255, 130, 0.92);  //low
  n2.setChasePower(255, 160, 0.95);
  scanner = Scanner(9, 400.0);
  scanner.goPosition(0);
  Serial1.begin(115200);
  Serial.begin(9600);
  robotState = new StandBy;
  tcs.begin();
  // while (!Serial1)
  //   ;
  while (!Serial)
    ;
  // waitIn();
  
  CsT test;
  test.printLetters();
  test.rotate(4,1);
  test.printLetters();
  
  setLed(0);
  n1.getTogether();
  n2.getTogether();
  waitAnything(2, 2);
  open();
  motorics.setState(0, 0);
  waitIn();
  // colorArray = new Cube::CubeArray;
  motorics.getCube()->printLetters();
  // motorics.scanCurrentSide();
  motorics.go(SubOperation(true,4,1));
  motorics.getCube()->printLetters();
  // motorics.scanCurrentSide();
  // fullScan();
  // colorArray->printInitialization();
  motorics.go(SubOperation(0,0));
  open();

  // for (uint8_t i = 0; i < 24; i++) {
  //   Serial.print(i);
  //   OperationPathStack solution = getShortestPathEdge(i);
  //   Serial.print("\t");
  //   SCS::getCsT(i).printNumbers();
  //   Serial.print("\t");
  //   Serial.print(solution.length);
  //   Serial.print("\t");
  //   Serial.print(Cube::State::edgePenaltyByDefault(i));
  //   solution.clear();
  //   sPnl();
  // }
}
void systemUpdate() {
  n2.update();
  n1.update();
  scanner.update();
  // serialUpd();
}
void loop() {
  systemUpdate();
}
