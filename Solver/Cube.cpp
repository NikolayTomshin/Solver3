#include "Cube.h"

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

Piece RealPieces[20];

void resetRealPieces() {  //each piece on it's default pos and def pos set as unfolded linear index
  for (uint8_t i = 0; i < 20; i++) {
    RealPieces[i] = Piece(unfoldLinIndex(i), 0);
  }
  Serial.println("RealPieces set to solved");
}

State::State() {  //собранное состояние
  for (uint8_t i = 0; i < 13; i++) {
    scs[i] = 0;
  }
  updateCAI();
}

uint8_t State::getscs(uint8_t index) {
  return (bitCoding::getBased(20, index, scs));
}

void State::setscs(uint8_t pieceIndex, uint8_t newSCsIndex) {
  bitCoding::writeBased(24, newSCsIndex, pieceIndex, scs);
}

void State::showscs() {
  for (uint8_t i = 0; i < 20; i++) {
    Serial.print(i);
    Serial.print("\t");
  }
  Serial.println();
  for (uint8_t i = 0; i < 20; i++) {
    Serial.print(getscs(i));
    Serial.print("\t");
  }
  Serial.println();
}

void State::setCAI(uint8_t pieceindex, uint8_t linearindex) {                     //задать ссылку индексом детали по линейным координатам
  bitCoding::writeBased(20, pieceindex, linearindex, coordinateArrangedIndexes);  //запись индекса по линейному индексу в координатный массив
}

void State::updateCAI() {              //обновление массива индексов по координатам
  for (uint8_t i = 0; i < 20; i++) {   //для каждой i реальной детали
    Vec temp = RealPieces[i].defPos;   //берём координаты её правильного положения
    SCS::transform(&temp, getscs(i));  //трансформируем координаты по ориентации в текущем состоянии
    setCAI(i, linearIndex(temp));      //запись индекса в соответствующий по координатам элемент
  }
}

uint8_t State::getCAI(uint8_t index) {  //get CAI value(piece index) by linear index
  return (bitCoding::getBased(24, index, coordinateArrangedIndexes));
}

void State::showcai() {  //print cai string
  for (uint8_t i = 0; i < 20; i++) {
    Serial.print(i);
    Serial.print("\t");
  }
  Serial.println();
  for (uint8_t i = 0; i < 20; i++) {
    Serial.print(getCAI(i));
    Serial.print("\t");
  }
  Serial.println();
}

uint8_t State::getscsByVec(Vec vec) {  //get scs index of piece by vector in this state
  return (getscs(getCAI(linearIndex(vec))));
}

static uint8_t State::getCAIcopy(uint8_t index) {  //get CAI value(piece index) by linear index from copied array
  return (bitCoding::getBased(24, index, cAIcopy));
}

void State::applyOperation(Operation op) {  //modify state's scs and CAI according to operation(rotation of one side)
  CsT opCs(op.ortoVector, 2);               //cs with k vector same as op vector
  // Serial.print("opCs:");  //making sure opCs third component same as op vector
  // opCs.print();
  // Serial.println();
  int8_t x, y;
  for (x = 0; x < 13; x++) {
    cAIcopy[x] = coordinateArrangedIndexes[x];  //copy preparation
  }
  for (x = -1; x < 2; x++) {
    for (y = -1; y < 2; y = y + 1 + (x == 0)) {  //+2 если x=0 что пропускает центр квадрата 3x3
      Vec iterationVec(x, y, 1);                 //перебор координат деталек относящихся к стороне на стороне +z
      // iterationVec.Cords();
      // Serial.print("/iter\t");        //локальные координаты
      iterationVec.Transform(&opCs);  //координаты преобразуются в координаты стороны операции
      // iterationVec.Cords();
      // Serial.print("/Abs\t");                                      //реальные координаты
      uint8_t pieceIndex = getCAIcopy(linearIndex(iterationVec));  //код детальки по вектору
      // Serial.print(pieceIndex);
      // Serial.print("pI\t");
      uint8_t newSCsIndex = SCS::getPostOpearationIndex(getscs(pieceIndex), op);  //новый код scs детальки
      // Serial.print(newSCsIndex);
      // Serial.print("nSCS\t");
      setscs(pieceIndex, newSCsIndex);                   //запись новой ориантации
      iterationVec.rotate(op.ortoVector, op.ortoAngle);  //запись нового расположения в итерационный вектор
      // iterationVec.Cords();
      // Serial.println("/newplace");
      setCAI(pieceIndex, linearIndex(iterationVec));
      //запись индекса детальки в коорд массив
      //т.к. ссылки деталей находятся по отдельной копии координатного массива, изменение в оригинальном массиве не портит результат
    }
  }
  //в итоге итераций по 8 деталькам стороны операции, поворачиваются их ориентации в массиве состояния
  //и меняются ссылки в координатном массиве в соответствии с этим
}

void State::printSliced(bool piOrSc, CsT cubeCs) {  //show scs kinda in 3d
  Serial.println();
  Serial.print("Slice");
  int8_t x, y, z, o;
  for (x = -1; x < 2; x++) {
    for (z = 1; z >= -1; z--) {
      for (o = (1 - x); o > 0; o--) Serial.print("\t");  //отступы
      for (y = -1; y < 2; y++) {
        Vec iterationVec(x, y, z);
        iterationVec.Untransform(&cubeCs);
        o = (x == 0) + (y == 0) + (z == 0);
        if (o < 2) {  //if piece is present
          if (piOrSc) {
            Serial.print(getCAI(linearIndex(iterationVec)));  //print piece index
          } else {
            Serial.print(getscsByVec(iterationVec));  //print scs
          }
        } else if (o == 2) {
          if (x == 1) Serial.print("+X");
          if (x == -1) Serial.print("-x");
          if (y == 1) Serial.print("+Y");
          if (y == -1) Serial.print("-y");
          if (z == 1) Serial.print("+Z");
          if (z == -1) Serial.print("-z");
        }
        Serial.print("\t");
      }
      Serial.println();
    }
    Serial.println("-----------------------");
  }
}

}//ns Cube