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
static uint8_t State::edgePenaltyByDefault(uint8_t i) {
  if (i == 0) return 0;
  if (i < 10) {
    return (1 + 2 * (i % 3 == 2));
  }
  if (i > 16) {
    switch (i) {
      default: return (3);
      case 17:
      case 23: return (2);
    }
  }
  switch (i) {
    case 10:
    case 17: return (2);
    default:
      switch (i) {
        case 12:
        case 13: return (3);
        default: return (1);
      }
  }
}
uint8_t State::statePenalty() {
  uint8_t penalty = 0;
  for (int8_t lin = 0; lin < 12; lin++) {
    uint8_t edgePenalty = 1;
    uint8_t lindex = edgeLindex[lin];
    uint8_t pieceIndex = getCAI(lindex);
    uint8_t scs = getscs(pieceIndex);  //scs of piece
    Vec tempVec = unfoldLinIndex(lindex);
    // Serial.print(lindex);
    // Serial.print("\t");
    uint8_t edgeOrt;
    for (edgeOrt = 0; edgeOrt < 3; edgeOrt++) {  //finding edge axis
      if (tempVec.c[edgeOrt] == 0) {             //component=0 is along edge
        break;
      }
    }
    CsT edgeCst = CsT(edgeOrt, 1);  //CsT( i,  j,  k, uint8_t missingComponent)
    switch (Vec::Scal(edgeCst.getOrt(0), &tempVec)) {
      case -1: edgeCst.rotate(edgeOrt, 2);  //180
      case 1:                               //match
        break;
      default:
        edgeCst.rotate(edgeOrt, -Vec::Scal(edgeCst.getOrt(2), &tempVec));
    }  //get edgeCst
    Vec edgeVec = Ovecs[edgeCst.getComponent(1)];

    // Serial.print("E");       //
    // edgeCst.printNumbers();  //

    CsT pseudoOrigin = edgeCst;                   //copy ECst
    pseudoOrigin.untransform(&SCS::getCsT(scs));  //reorient back to origin
    // Serial.print(";\t O");                        //
    // pseudoOrigin.printNumbers();              //

    edgeCst.untransform(&pseudoOrigin);  //how it looks

    // Serial.print(";\t U");  //
    // edgeCst.printNumbers();  //

    edgePenalty += edgePenaltyByDefault(SCS::getSCindex(edgeCst));  //orientation penalty
    // Serial.println(edgePenalty);

    uint8_t lengthPenalty = 1;
    for (int8_t y1 = -1; y1 < 2; y1 += 2) {
      Vec temp2Vec = tempVec;
      temp2Vec.Add(&edgeVec, y1);
      if (getscsByVec(temp2Vec) != scs) lengthPenalty++;
    }
    edgePenalty *= lengthPenalty;  //multiply
    // Serial.print("\t");
    // Serial.println(edgePenalty);
    if (edgePenalty > 1) { penalty += edgePenalty; }
  }
  return penalty;
}

uint8_t* indexedTiles;
Color* colorTiles;     //colors for all tiles
Color colorPallet[6];  //6 x color

uint8_t colorLindex(Vec vector) {  //vector around 3x3 (normak=1) cube
  uint8_t oa;
  for (oa = 0; oa < 3; oa++) {
    int8_t val = vector.c[oa];
    if (abs(val) == 2) {
      break;
    }
  }
  return (9 * oa + 3 * (vector.c[(oa + 1) % 3] + 1) + vector.c[(oa + 2) % 3] + 1);
}
Vec numberOfSimilar(uint8_t palletIndex, Color* delta) {
  Vec n(0, 0, 0);
  for (uint8_t i = 0; i < 54; i++) {
    Color* colorP = &colorTiles[i];
    for (uint8_t c = 0; c < 3; c++) {
      if (abs(colorP->component[c] - colorPallet[palletIndex].component[c]) <= delta->component[c]) {
        n.c[c]++;
      }
    }
  }
  return n;
}

}  //ns Cube