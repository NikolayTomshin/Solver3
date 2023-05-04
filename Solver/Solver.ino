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

  // CsT temp;
  // for (uint8_t i = 0; i < 20; i++) {
  //   Serial.print(i);
  //   Serial.print("=");
  //   Vec checkV = Cube::unfoldLinIndex(i);
  //   Serial.println(Cube::linearIndex(checkV));
  // }
  uint8_t ov=1;
  bool showI=true;
  Cube::resetRealPieces();
  currentState.updateCAI();
  currentState.printSliced(showI);
  // SCS::getCsT(currentState.getscsByVec(Vec(1, 1, 1))).print();
  path::Operation op(ov, -1);
  currentState.applyOperation(op);
  currentState.printSliced(showI);
  op.set(ov+1,-1);
  currentState.applyOperation(op);
  currentState.printSliced(showI);
  // op=path::Operation(2,-1);
  // currentState.applyOperation(op);
  // currentState.applyOperation(op);
  // currentState.printSliced();
  // SCS::getCsT(currentState.getscsByVec(Vec(1, 1, 1))).print();
  // SCS::getCsT(currentState.getscsByVec(Vec(1, -1, 1))).print();
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
