#include "Arduino.h"
#include <stdint.h>
#pragma once
#include "Path.h"
//file for dealing with virtual cube

namespace Cube {
uint8_t linearIndex(Vec v) {
  //это сложная часть. Представим куб как верхний и нижний слой. В обоих слоях есть по 4 места по прямой и диагонали.
  //Кроме слоёв есть 4 "колонны" их соединяющие по углам. Предлагаю удлинить колонны до слоёв сделав их высоту в 3 детали.
  // Посмотрев на x y координаты можно понять находится деталь в колонне или одном из слоёв.
  //В слоях кодировка (низ или вверх)*(одно из 4 направлений)+ в колоннах (координата по высоте из 3 вариантов)*(одно из 4 направлений)
  //Индексы слоёв могут занять места от 0 до 7. Затем по 4 индекса с низу вверх влоть до 7+4*4=19=20-1 всё сходится
  int8_t x = v.c[0];
  int8_t y = v.c[1];
  if (x * y) {  //колонна
    return (8 + 4 * (v.c[2] + 1) + 1 + x + (y + 1) / 2);
  } else {  //слой
    return ((v.c[2] + 1) * 2 + (x + 1) + y * (y + 1));
  }
}
Vec unfoldLinIndex(uint8_t linindex) {
  Vec temp;
  int8_t c;
  if (linindex < 8) {
    temp.c[2] = (linindex / 4) * 2 - 1;
    c = linindex % 4;
    temp.c[0] = (c - 1) % 2;
    temp.c[1] = (c - 2) % 2;
  } else {
    linindex -= 8;
    temp.c[2] = (linindex / 4) - 1;
    linindex = linindex % 4;
    temp.c[0] = (linindex / 2) * 2 - 1;
    temp.c[1] = (linindex % 2) * 2 - 1;
  }
  return (temp);
}

Piece RealPieces[20];  //real cube pieces 20 подвижных деталек которые двигаются в системе куба

void resetRealPieces() {
  for (uint8_t i = 0; i < 20; i++) {
    RealPieces[i] = Piece(unfoldLinIndex(i), 0);
  }
  Serial.println("RealPieces set to solved");
}


struct State {
  uint8_t scs[13];                        //compressed array of SCS indexes of pieces ordered by real array
  uint8_t coordinateArrangedIndexes[13];  //compressed array of real pieces indexes in order of 3d lineared coordinates
  inline static uint8_t cAIcopy[13];      //cAI copy

  State() {  //собранное состояние
    for (uint8_t i = 0; i < 13; i++) {
      scs[i] = 0;
    }
  }
  uint8_t getscs(uint8_t index) {
    return (bitCoding::getBased(20, index, scs));
  }
  void showscs() {
    for (uint8_t i = 0; i < 20; i++) {
      Serial.print(getscs(i));
      Serial.print(",");
    }
    Serial.println();
  }
  void setCAI(uint8_t pieceindex, uint8_t linearindex) {                            //задать ссылку индексом детали по линейным координатам
    bitCoding::writeBased(20, pieceindex, linearindex, coordinateArrangedIndexes);  //запись индекса по линейному индексу в координатный массив
  }
  void updateCAI() {                     //обновление массива индексов по координатам
    for (uint8_t i = 0; i < 20; i++) {   //для каждой i реальной детали
      Vec temp = RealPieces[i].defPos;   //берём координаты её правильного положения
      SCS::transform(&temp, getscs(i));  //трансформируем координаты по ориентации в текущем состоянии
      setCAI(i, linearIndex(temp));      //запись индекса в соответствующий по координатам элемент
    }
  }
  uint8_t getCAI(uint8_t index) {
    return (bitCoding::getBased(24, index, coordinateArrangedIndexes));
  }
  void showcai() {
    for (uint8_t i = 0; i < 20; i++) {
      Serial.print(getCAI(i));
      Serial.print(",");
    }
    Serial.println();
  }
  uint8_t getscsByVec(Vec vec) {
    return (getscs(getCAI(linearIndex(vec))));
  }
  static uint8_t getCAIcopy(uint8_t index) {
    return (bitCoding::getBased(24, index, cAIcopy));
  }
  void applyOperation(path::Operation op) {
    CsT opCs(op.ortoVector, 2);  //cs with k vector same as op vector
    int8_t x, y;
    for (x = 0; x < 13; x++) {
      cAIcopy[x] = coordinateArrangedIndexes[x];  //copy preparation
    }
    for (x = -1; x < 2; x++) {
      for (y = -1; y < 2; y = y + 1 + (x == 0)) {  //+2 если x=0 что пропускает центр квадрата 3x3
        Vec iterationVec(x, y, 1);                 //перебор координат деталек относящихся к стороне на стороне +z
        iterationVec.Transform(&opCs);             //координаты преобразуются в координаты стороны операции
        uint8_t pieceIndex = getCAIcopy(linearIndex(iterationVec));                 //код детальки
        uint8_t newSCsIndex = SCS::getPostOpearationIndex(getscs(pieceIndex), op);  //новый код scs детальки
        bitCoding::writeBased(24, newSCsIndex, pieceIndex, scs);                    //запись новой ориантации

        SCS::transform(&iterationVec, newSCsIndex);  //запись нового расположения в итерационный вектор
        bitCoding::writeBased(20, pieceIndex, linearIndex(iterationVec), coordinateArrangedIndexes);
        //запись индекса детальки в коорд массив
        //т.к. ссылки деталей находятся по отдельной копии координатного массива, изменение в оригинальном массиве не портит результат
      }
    }
    //в итоге итераций по 8 деталькам стороны операции, поворачиваются их ориентации в массиве состояния
    //и меняются ссылки в координатном массиве в соответствии с этим
  }
  void printSliced(CsT cubeCs) {  //show scs kinda in 3d
    int8_t x, y, z;
    for (x = -1; x < 2; x++) {
      for (uint8_t o = (1 - x) * 2; o > 0; o--) Serial.print(" ");
      for (z = 1; z > -1; z = z - 1 - (x == 0)) {
        for (y = -1; y < 2; y = y + 1 + (x == 0) | (z == 0)) {
          Vec iterationVec(x, y, z);
          Serial.print(getscs(linearIndex(iterationVec)));
          Serial.print(" ");
        }
      }
      Serial.println();
    }
  }
};

}
