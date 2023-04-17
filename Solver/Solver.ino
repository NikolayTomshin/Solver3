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

  CsT temp;
  Vec checkV(1, 1, 1);
  Cube::resetRealPieces();
  currentState.updateCAI();
  currentState.showscs();
  currentState.showcai();
  SCS::getCsT(currentState.getscsByVec(Vec(1, 1, 1))).print();
  path::Operation op(1, -1);
  currentState.applyOperation(op);
  currentState.showscs();
  currentState.showcai();
  SCS::getCsT(currentState.getscsByVec(Vec(1, 1, 1))).print();
  // int8_t x, y, z;
  // for (x = -1; x < 2; x++) {
  //   for (z = 1; z > -2; z = z - 1 - (x == 0)) {
  //     for (y = -1; y < 2; y = y + 1 + ((x == 0) || (z == 0))) {
  //       Vec iterationVec(x, y, z);
  //       iterationVec.Cords();
  //       uint8_t ind = Cube::linearIndex(iterationVec);
  //       Serial.print(ind);
  //       Serial.print("->");
  //       Cube::unfoldLinIndex(ind).Cords();
  //       Serial.println();
  //     }
  //   }
}
void loop() {
  // // n1.logEncoder();
  // n2.update();
  // n1.update();
}
