#include "ClawUnit.h"

ClawUnit n1;  //left claw 1
ClawUnit n2;  //right claw 2
Timer buttonTimer(20);

CsT n1Cs;       //ориентация модуля 1 z сонаправлена клешне, x смотрит в сторону энкодера
CsT n2Cs;       //ориентация модуля 2 z сонаправлена клешне, x смотрит в сторону энкодера
CsT scannerCs;  //x-смотрит на куб, y-в сторону откидывания

CsT cubeCs;  //ориентация куба изначально не важна, закреплена за центральными клетками которые не считаются деталями

path::Branch solveTree[20];  //кодирование пути сборки

Cube::State currentState;  //current state
Cube::State newState;      //new state for fast checking

void opDemo() {
  Cube::resetRealPieces();
  currentState.updateCAI();
  while (true) {
    currentState.printSliced(false);
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
