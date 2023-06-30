// #include "ClawUnit.h"
// #include "TimeManager.h"
// #include "CsT.h"
#include "States.h"

void testThing() {  //do in beginning
}

void setup() {
  n1 = ClawUnit(4, 5, 11, A3, A2);
  n2 = ClawUnit(7, 6, 10, A1, A0);
  n1.SetAngles(90, 100, 110);
  n2.SetAngles(90, 100, 110);
  n1.assumeRotation();
  n2.assumeRotation();
  n1.setGrab(2);
  n2.setGrab(2);
  scanner = Scanner(9);
  scanner.goPosition(0);
  Serial1.begin(115200);
  Serial.begin(9600);
  while (!Serial1)
    ;
  while (!Serial)
    ;
  StandBy defaultState(true);
  stateStack.addState(&defaultState);
  testThing();
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
void loop() {
  n2.update();
  n1.update();
  if (nextionTimer.isLoop()) router.update();
  robotState->update();
}
