// #include "ClawUnit.h"
// #include "TimeManager.h"
// #include "CsT.h"
#include "Awareness.h"

void setup() {
  Serial.begin(9600);
  // Serial1.begin(9600);
  // n1 = ClawUnit(4, 5, 10, A0, A1);
  // n2 = ClawUnit(7, 6, 11, A2, A3);
  // n1.SetAngles(5, 128, 145);
  // n2.SetAngles(11, 140, 152);  //!!
  // delay(500);
  // n1.setGrab(0);
  // delay(100);
  // n2.setGrab(2);
  // n1.setChase(true);  //enable chase
  // n2.setChase(true);
  // n1.setChasePower(255, 160, 0.95);
  // n2.setChasePower(255, 100, 0.84);
  // n1.allignRotation();
  // delay(1000);
  // n2.allignRotation();
  while (!Serial)
    ;

  Serial.println("begin");
  for (uint8_t i = 0; i < 24; i++) {
    OperationPathStack solution = getShortestPathEdge(i);
    Serial.print(i);
    SCS::getCsT(i).print();
    solution.print();
    sPnl();
    // Serial.println("Printed");
    // Serial.println("Deleted");
  }
  Serial.println("the end.");
}
void loop() {
  // // n1.logEncoder();
  // n2.update();
  // n1.update();
}
