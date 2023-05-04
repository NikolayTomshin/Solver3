#include "ClawUnit.h"

ClawUnit n1;  //left claw 1
ClawUnit n2;  //right claw 2
Timer buttonTimer(20);

CsT n1Cs;       //ориентация модуля 1 z сонаправлена клешне, x смотрит в сторону энкодера
CsT n2Cs;       //ориентация модуля 2 z сонаправлена клешне, x смотрит в сторону энкодера
CsT scannerCs;  //x-смотрит на куб, y-в сторону откидывания

CsT cubeCs;            //ориентация куба изначально не важна, закреплена за центральными клетками которые не считаются деталями

path::Branch solveTree[20];  //кодирование пути сборки

Cube::State currentState;  //current state
Cube::State newState;      //new state for fast checking


