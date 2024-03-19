#include "Cube.h"
#include "Arduino.h"

void CubeOperation::print() {
  pout("[");
  if (uint8_t oa = getOA()) {
    String side;
    switch (uint8_t(getAx())) {
      case 0:
        side = "F";
        break;
      case 1:
        side = "R";
        break;
      case 2:
        side = "U";
        break;
      case 3:
        side = "B";
        break;
      case 4:
        side = "L";
        break;
      case 5:
        side = "D";
        break;
    }
    pout(side);
    switch (oa) {
      case 1:
        side = "'";  //because need to be clockwise looking from outside
        break;
      case 2:
        side = "2";
        break;
      case 3:
        side = " ";
    }
    pout(side);
  }
  pout("]");
}

CubeOperation::operator Rotation() const {
  return Rotation(code);
}
#include <Arduino.h>
void ICubeState::scramble(uint8_t moves) {
  Axis ax(random(0, 5));
  for (uint8_t m = 0; m < moves; m++) {
    ax = Axis(ax + random(1, 5));
    CubeOperation move(ax, random(1, 3));
#ifdef ScrambleLog
    pnl();
    pout((m + 1));
    pout("\t ");
    move.print();
    pnl();
#endif
    operator*=(move);

#ifdef ScrambleLog
    printAxonometric(0);
#endif
  }
}
char ICubeState::letterOfSide(const Axis& side) const {
  switch (side) {
    case 0: return 'F';
    case 1: return 'R';
    case 2: return 'U';
    case 3: return 'B';
    case 4: return 'L';
    case 5: return 'D';
    default: return 'X';
  }
}

Axis ICubeState::sideOfSelection(const Vec& selection) {
#ifdef PrintDebug
  pout(F("\Selecting "));
  temp.print();
#endif

  Axis side = Axis::maxDimension(selection);
#ifdef PrintDebug
  pout(F("; Side "));
  pout(letterOfSide(side));
  pout(F(" side Vec="));
  side.getUnitVec().print();
  pout(F("\n Piece at "));
  (selection - (side.getUnitVec())).print();
  pout(F(" have Cs "));
  getOrientation(selection - side.getUnitVec()).print();
  pout(F("\n result in"));
#endif
  if (selection.isOneDimensional())  //center
    return side;
  return getOrientation(selection - side.getUnitVec()).coincidentComponent(side);
}
class Vec12Iterator : public Vec8Iterator {
protected:
public:
  Vec12Iterator() {}
  Vec12Iterator(int8_t z_, int8_t shift_ = 0) {
    z = z_;
    index = 0;
    shift = shift_;
    updateVec();
    ptr = &value;
  }
  bool isEnd() const override {
    return index >= 12;
  }
  bool isLoop() override {
    if (isEnd()) {
      index %= 12;
      return true;
    }
    return false;
  }
protected:
  void updateVec() override {
    Axis side(Axis::nY);
    uint8_t ind = Mod(12, index + shift);
    side *= Rotation(Axis::Z, ind / 3);  //rotate
    Cs temp(Axis::Z, side);              // i=z, j=side, k along side left to right
    value = (side.getUnitVec() * 2) + (temp[2].getUnitVec() * (int8_t(ind % 3) - 1));
    value[2] = z;
  }
};
void ICubeState::printUnfolded(const Orientation& rotateAs) {
  /*
                                 s
it 0 1 2 3 4 5 6 7 8 9 10 11
        {U U U} x=-1            |0
        {U U U} Penalty=XXX     |1  i=0
        {U U U} x= 1            |2
  {L L L[F F F]R R R|B B B} z= 1|3
  {L L L[F F F]R R R|B B B} z= 0|4  i=1
  {L L L[F F F]R R R|B B B} z=-1|5
        {D D D} x= 1            |6
        {D D D} x= 0            |7  i=2
        {D D D} x=-1            |8
    \|/  | | |  \|/  | | L
  y -1  -1 0 1   1   1 0 -1
  */
  pnl();
  auto f = [this](const Vec& selection) {
    pout(letterOfSide(sideOfSelection(selection)));
  };
  for (uint8_t s = 0; s < 9; s++) {
    Vec selecting;
    uint8_t i = s / 3;
    if (i != 1) {
      pout(F("      "));
    }
    pout("{");
    //print line
    if (i != 1) {
      bool up = !i;
      int8_t z = flipSign(up, -2);
      int8_t x = up ? int8_t(s) - 1 : 7 - s;
      for (int8_t y = -1; y < 2; y++) {
        f(Vec(x, y, z) * rotateAs);
        if (y < 1) pout(" ");
      }
    } else {
      int8_t z = 4 - s;
      for (Vec12Iterator it(z, 0); !it.isEnd(); it++) {
        f((*it) * rotateAs);
        switch (it.getIteration()) {
          case 2:
            pout('[');
            break;
          case 5:
            pout(']');
            break;
          case 8:
            pout('|');
            break;
          default:
            pout(' ');
            break;
          case 11:;
        }
      }
    }
    pout("}");
    if (s == 1) {
      pout(F(" Penalty="));
      pout(statePenalty());
    }
    pnl();
  }
}
class AxonometricIterator : public IIterator<Vec> {
  Vec value;
  uint8_t index;
  Cs viewCs;
public:
  uint8_t getIndex() {
    return index;
  }
  AxonometricIterator(const Orientation& orientation) {
    index = 0;
    viewCs = orientation.getCs();
    updValue();
    ptr = &value;
  }
  AxonometricIterator& operator+=(int8_t other) {
    index += other;
    updValue();
    return *this;
  }
  AxonometricIterator& operator-=(int8_t other) {
    index -= other;
    updValue();
    return *this;
  }
  bool isEnd() const {
    return index >= 24;
  }
  bool isLoop() {
    if (isEnd()) {
      index = 0;
      return true;
    }
    return false;
  }
  Vec oneGo() {
    Vec temp = value;
    operator++();
    return temp;
  }
protected:
  /*  ,___________.      .___________,      
    ,/_0_/_1_/_2,/|      | 3 | 4 | 5 |\     
  ,/_6_/_U_/_7,/|8|      |___|___|___|9|\   
 /10_/11_/12,/13|/|      |14 | B |15 |\16|\ 
|17 |18 |19 20|/|21      |___|___|___22|\|23
|___|___|___|/|R|/|      |24 |25 |26 |\|L|\|
|27 | F |28 29|/30|      |___|___|___31|\|32
|___|___|___|/|33/       `\34_\35_\36_\37|\|
|38 |39 |40 |41/           `\42_\_D_\43_\|44
|___|___|___|/  Penalty=XXX  `\45_\46_\47_\| */
  void updValue() {
    switch (index) {
      case 0:
        value = Vec(-1, -1, 2);
        break;
      case 1:
        value = Vec(-1, 0, 2);
        break;
      case 2:
        value = Vec(-1, 1, 2);
        break;
      case 3:
        value = Vec(-2, 1, 1);
        break;
      case 4:
        value = Vec(-2, 0, 1);
        break;
      case 5:
        value = Vec(-2, -1, 1);
        break;
      case 6:
        value = Vec(0, -1, 2);
        break;
      case 7:
        value = Vec(0, 1, 2);
        break;
      case 8:
        value = Vec(-1, 2, 1);
        break;
      case 9:
        value = Vec(-1, -2, 1);
        break;
      case 10:
        value = Vec(1, -1, 2);
        break;
      case 11:
        value = Vec(1, 0, 2);
        break;
      case 12:
        value = Vec(1, 1, 2);
        break;
      case 13:
        value = Vec(0, 2, 1);
        break;
      case 14:
        value = Vec(-2, 1, 0);
        break;
      case 15:
        value = Vec(-2, -1, 0);
        break;
      case 16:
        value = Vec(0, -2, 1);
        break;
      case 17:
        value = Vec(2, -1, 1);
        break;
      case 18:
        value = Vec(2, 0, 1);
        break;
      case 19:
        value = Vec(2, 1, 1);
        break;
      case 20:
        value = Vec(1, 2, 1);
        break;
      case 21:
        value = Vec(-1, 2, 0);
        break;
      case 22:
        value = Vec(-1, -2, 0);
        break;
      case 23:
        value = Vec(1, -2, 1);
        break;
      case 24:
        value = Vec(-2, 1, -1);
        break;
      case 25:
        value = Vec(-2, 0, -1);
        break;
      case 26:
        value = Vec(-2, -1, -1);
        break;
      case 27:
        value = Vec(2, -1, 0);
        break;
      case 28:
        value = Vec(2, 1, 0);
        break;
      case 29:
        value = Vec(1, 2, 0);
        break;
      case 30:
        value = Vec(-1, 2, -1);
        break;
      case 31:
        value = Vec(-1, -2, -1);
        break;
      case 32:
        value = Vec(1, -2, 0);
        break;
      case 33:
        value = Vec(0, 2, -1);
        break;
      case 34:
        value = Vec(-1, 1, -2);
        break;
      case 35:
        value = Vec(-1, 0, -2);
        break;
      case 36:
        value = Vec(-1, -1, -2);
        break;
      case 37:
        value = Vec(0, -2, -1);
        break;
      case 38:
        value = Vec(2, -1, -1);
        break;
      case 39:
        value = Vec(2, 0, -1);
        break;
      case 40:
        value = Vec(2, 1, -1);
        break;
      case 41:
        value = Vec(1, 2, -1);
        break;
      case 42:
        value = Vec(0, 1, -2);
        break;
      case 43:
        value = Vec(0, -1, -2);
        break;
      case 44:
        value = Vec(1, -2, -1);
        break;
      case 45:
        value = Vec(1, 1, -2);
        break;
      case 46:
        value = Vec(1, 0, -2);
        break;
      case 47:
        value = Vec(1, -1, -2);
        break;
    }
    value *= viewCs;
  }
};
void ICubeState::printAxonometric(const Orientation& rotateAs) {
  /*  ,___________.      .___________,      
    ,/_U_/_U_/_U,/|      | B | B | B |\     
  ,/_U_/_U_/_U,/|R|      |___|___|___|L|\   
 /_U_/_U_/_U,/|R|/|      | B | B | B |\|L|\ 
| F | F | F |R|/|R|      |___|___|___|L|\|L|
|___|___|___|/|R|/|      | B | B | B |\|L|\|
| F | F | F |R|/|R|      |___|___|___|L|\|L|
|___|___|___|/|R|/       `\_D_\_D_\_D_\|L|\|
| F | F | F |R|/           `\_D_\_D_\_D_\|L|
|___|___|___|/  Penalty=XXX  `\_D_\_D_\_D_\| 
*/
  AxonometricIterator it(rotateAs);
  auto f = [&it, this]() -> char {
#ifdef PrintDebug
    pout(F("Printing Iteration "));
    poutN(it.getIndex());
#endif
    return letterOfSide(sideOfSelection(it.oneGo()));
  };

#ifndef PrintDebug
  poutN(F("      ,___________.      .___________,      "));  //0
  pout(F("    ,/_"));                                        //1
  pout(f());                                                 /*0*/
  pout(F("_/_"));
  pout(f()); /*1*/
  pout(F("_/_"));
  pout(f()); /*2*/
  pout(F(",/|      | "));
  pout(f()); /*3*/
  pout(F(" | "));
  pout(f()); /*4*/
  pout(F(" | "));
  pout(f()); /*5*/
  poutN(F(" |\\     "));
  pout(F("  ,/_"));  //2
  pout(f());         /*6*/
  pout(F("_/_"));
  pout(letterOfSide(Axis::Z)); /*U*/
  pout(F("_/_"));
  pout(f()); /*7*/
  pout(F(",/|"));
  pout(f()); /*8*/
  pout(F("|      |___|___|___|"));
  pout(f()); /*9*/
  poutN(F("|\\   "));
  pout(F(" /_"));  //3
  pout(f());       /*10*/
  pout(F("_/_"));
  pout(f()); /*11*/
  pout(F("_/_"));
  pout(f()); /*12*/
  pout(F(",/|"));
  pout(f()); /*13*/
  pout(F("|/|      | "));
  pout(f()); /*14*/
  pout(F(" | "));
  pout(letterOfSide(Axis::nX)); /*B*/
  pout(F(" | "));
  pout(f()); /*15*/
  pout(F(" |\\|"));
  pout(f()); /*16*/
  poutN(F("|\\ "));
  pout(F("| "));  //4
  pout(f());      /*17*/
  pout(F(" | "));
  pout(f()); /*18*/
  pout(F(" | "));
  pout(f()); /*19*/
  pout(F(" |"));
  pout(f()); /*20*/
  pout(F("|/|"));
  pout(f()); /*21*/
  pout(F("|      |___|___|___|"));
  pout(f()); /*22*/
  pout(F("|\\|"));
  pout(f()); /*23*/
  poutN(F("|"));
  pout(F("|___|___|___|/|"));  //5
  pout(letterOfSide(Axis::Y)); /*R*/
  pout(F("|/|      | "));
  pout(f()); /*24*/
  pout(F(" | "));
  pout(f()); /*25*/
  pout(F(" | "));
  pout(f()); /*26*/
  pout(F(" |\\|"));
  pout(letterOfSide(Axis::nY)); /*L*/
  poutN(F("|\\|"));
  pout(F("| "));  //6
  pout(f());      /*27*/
  pout(F(" | "));
  pout(letterOfSide(Axis::X)); /*F*/
  pout(F(" | "));
  pout(f()); /*28*/
  pout(F(" |"));
  pout(f()); /*29*/
  pout(F("|/|"));
  pout(f()); /*30*/
  pout(F("|      |___|___|___|"));
  pout(f()); /*31*/
  pout(F("|\\|"));
  pout(f()); /*32*/
  poutN(F("|"));
  pout(F("|___|___|___|/|"));  //7
  pout(f());                   /*33*/
  pout(F("|/       `\\_"));
  pout(f()); /*34*/
  pout(F("_\\_"));
  pout(f()); /*35*/
  pout(F("_\\_"));
  pout(f()); /*36*/
  pout(F("_\\|"));
  pout(f()); /*37*/
  poutN(F("|\\|"));
  pout(F("| "));  //8
  pout(f());      /*38*/
  pout(F(" | "));
  pout(f()); /*39*/
  pout(F(" | "));
  pout(f()); /*40*/
  pout(F(" |"));
  pout(f()); /*41*/
  pout(F("|/           `\\_"));
  pout(f()); /*42*/
  pout(F("_\\_"));
  pout(letterOfSide(Axis::nZ)); /*D*/
  pout(F("_\\_"));
  pout(f()); /*43*/
  pout(F("_\\|"));
  pout(f()); /*44*/
  poutN(F("|"));

  String pen = String(statePenalty());
  while (pen.length() < 3) pen.concat(' ');
  pout("|___|___|___|/  Penalty=");  //9
  pout(pen);
  pout("  `\\_");
  pout(f()); /*45*/
  pout("_\\_");
  pout(f()); /*46*/
  pout("_\\_");
  pout(f()); /*47*/
  poutN("_\\|");
#else
  for (uint8_t i = 0; i < 48; i++) poutN(f());
#endif
}
void CubeState::printList() {
  for (uint8_t i = 0; i < 20; i++) {
    pout('[');
    pout(getValue(pieceIndexesByPosition, i));
    Orientation(getValue(orientationsByPosition, i)).print();
    pout(']');
  }
}
void Cube::printList() {
  for (uint8_t i = 0; i < 20; i++) {
    pout('[');
    pout(i);
    cubelets[i].print();
    pout(']');
  }
}

CubeOperation::CubeOperation(const Axis& ax, const OrtAng& oa = 1)
  : Rotation(ax, oa) {}
CubeOperation::CubeOperation(const Rotation& other)
  : Rotation(other) {}

Cube::Piece::Piece(const Vec& origin_, const Orientation& orientation)
  : Orientation(orientation) {
  origin = origin_;

#ifdef CubeStateOperationDebug
  pout(F("My origin is "));
  origin.print();
  pout(F("\t my orientation "));
  print();
  pnl();
#endif
}

const Vec& Cube::Piece::getOrigin() const {
  return origin;
}
Vec Cube::Piece::getPosition() const {
  Vec temp = origin;
  temp *= Orientation(*this);
  // #ifdef CubeStateOperationDebug
  //   pout("My position is ");
  //   temp.print();
  //   pnl();
  // #endif
  return temp;
}

uint8_t Cube::operator[](const Vec& pos) {
  for (uint8_t i = 0; i < 20; i++) {
    if (cubelets[i].getPosition() == pos) return i;
  }
  pout(F("Error invalid pos vector to search cubelet(returned default)! "));
  pos.print();
  return 0;
}
const Cube::Piece& Cube::operator[](uint8_t index) const {
  return cubelets[index];
}
Cube::Piece& Cube::operator[](uint8_t index) {
  return cubelets[index];
}
bool Cube::isSolved() const {
  const Orientation solved;
  for (uint8_t i = 0; i < 20; i++)
    if (!cubelets[i].isNeutral()) return false;
  return true;
}
void Cube::reset() {
  for (uint8_t i = 0; i < 20; i++)
    cubelets[i] = Piece(vectorOf(i), Orientation());
}

Cube::Cube() {
  reset();
}
Cube& Cube::operator=(const CubeState& other) {
  other.setCube(*this);
}
Orientation Cube::getOrientation(const Vec& position) const {
  return Orientation(cubelets[operator[](position)]);
}

void Cube::operator*=(const CubeOperation& other) {
  uint8_t indexes[8];
  for (CubeSidePieceIterator pos(other.getAx()); !pos.isEnd(); pos++) {
    indexes[pos.getIteration()] = operator[](*pos);
  }
  for (uint8_t i = 0; i < 8; i++)
    cubelets[indexes[i]] *= other;
  // #ifdef CubeStateOperationDebug
  //     pout(" which changed to ");
  //     temp.print();
  //     pnl();
  // #endif
}
void Cube::operator/=(const CubeOperation& other) {
  operator*=(-other);
}


Vec8Iterator::Vec8Iterator(int8_t z_, int8_t shift_ = 0) {
  z = z_;
  index = 0;
  shift = shift_;
  updateVec();
  ptr = &value;
}
void Vec8Iterator::operator=(uint8_t other) {
  index = other;
}
Vec8Iterator& Vec8Iterator::operator+=(int8_t other) {
  index += other;
#ifdef IteratorDebug
  // pout(index);
#endif
  updateVec();
  return *this;
}
Vec8Iterator& Vec8Iterator::operator-=(int8_t other) {
  index -= other;
  updateVec();
  return *this;
}
bool Vec8Iterator::isEnd() const {
  return index > 7;
}
bool Vec8Iterator::isLoop() {
  if (isEnd()) {
    *this = 0;
    return true;
  }
  return false;
}
uint8_t Vec8Iterator::getIteration() {
  return index;
}
void Vec8Iterator::updateVec() {
  switch (Mod8(shift + index)) {
    case 0:
      value = Vec(1, 0, z);
      break;
    case 1:
      value = Vec(1, 1, z);
      break;
    case 2:
      value = Vec(0, 1, z);
      break;
    case 3:
      value = Vec(-1, 1, z);
      break;
    case 4:
      value = Vec(-1, 0, z);
      break;
    case 5:
      value = Vec(-1, -1, z);
      break;
    case 6:
      value = Vec(0, -1, z);
      break;
    case 7:
      value = Vec(1, -1, z);
      break;
    default:;
  }
#ifdef IteratorDebug
    // value.print();
    // pout(index);
#endif
}

CubeSidePieceIterator::CubeSidePieceIterator() {
  ptr = &value;
}
CubeSidePieceIterator::CubeSidePieceIterator(Axis side) {
  sideCs = Cs(side);
  sideIterator = Vec8Iterator(1, 0);
  updateVec();
  ptr = &value;
}
CubeSidePieceIterator& CubeSidePieceIterator::operator+=(int8_t other) {
#ifdef IteratorDebug
// pout("CubeSidePieceIterator+=!");
#endif
  sideIterator += other;
  updateVec();
#ifdef IteratorDebug
  pout(F("Added"));
#endif
  return *this;
}
CubeSidePieceIterator& CubeSidePieceIterator::operator-=(int8_t other) {
  sideIterator -= other;
  updateVec();
  return *this;
}
bool CubeSidePieceIterator::isEnd() const {
  return sideIterator.isEnd();
}
bool CubeSidePieceIterator::isLoop() {
  return sideIterator.isLoop();
}
void CubeSidePieceIterator::updateVec() {
  value = (sideIterator.value) * sideCs;
#ifdef IteratorDebug
  // (sideIterator.operator*()).print();
#endif
}
uint8_t CubeSidePieceIterator::getIteration() {
  return sideIterator.getIteration();
}

CubeState::CubeState(const Cube& model) {
  operator=(model);
}
CubeState CubeState::operator=(const Cube& other) {
  for (uint8_t i = 0; i < 20; i++) {  //i - index of piece from cube
    const Cube::Piece& ref = other[i];
    Vec position = ref.getPosition();
    setPieceIndex(position, i);
    setPieceOrientation(position, ref);
  }
  return *this;
}

Orientation CubeState::getOrientation(const Vec& position) const {
  return Orientation(getValue(orientationsByPosition, indexOf(position)));
}
uint8_t CubeState::getPieceIndex(const Vec& position) const {
  return getValue(pieceIndexesByPosition, indexOf(position));
}
void CubeState::setPieceOrientation(const Vec& position, const Orientation& orientation) {
  setValue(orientationsByPosition, uint8_t(orientation), indexOf(position));
}
void CubeState::setPieceIndex(const Vec& position, uint8_t pieceIndex) {
  setValue(pieceIndexesByPosition, pieceIndex, indexOf(position));
}

bool CubeState::isSolved() const {
  for (uint8_t i = 0; i < 20; i++) {
    if (getValue(orientationsByPosition, i)) return false;
  }
  return true;
}
void CubeState::reset() {
  for (uint8_t i = 0; i < 20; i++) {
    setValue(pieceIndexesByPosition, i, i);
    setValue(orientationsByPosition, 0, i);
  }
}

void CubeState::operator*=(const CubeOperation& other) {
  uint8_t pieceIndex[5];  //index of piece
  uint8_t posIndex[5];    //position of piece
  uint8_t orsValue[5];    //orientation of piece
  uint8_t i = 0;
  Rotation op = other;
  for (CubeSidePieceIterator pos(other.getAx()); !pos.isEnd(); pos++) {
#ifdef CubeStateOperationDebug
    pout(F("Accessing "));
    (*pos).print();
    pout(F(" having "));
    getOrientation(*pos).print();
    pout(F(" rotated to "));
    Orientation temp = getOrientation(*pos);
    (temp * op).print();
    pout(F(" moved to "));
    ((*pos) * op).print();
#endif

    uint8_t thisPosIndex = indexOf(*pos);
#ifdef CubeStateOperationDebug
    pout(F("\twriting ind "));
    pout(getValue(pieceIndexesByPosition, thisPosIndex));
    pout(F("\tin "));
    pout(indexOf((*pos) * op));
    pout(F("\twith "));
    poutN(uint8_t(Orientation(getValue(orientationsByPosition, thisPosIndex)) * op));
#endif
    BitCoding::writeSizedElement(pieceIndex, getValue(pieceIndexesByPosition, thisPosIndex), 5, i);                           //index to write
    BitCoding::writeSizedElement(posIndex, indexOf((*pos) * op), 5, i);                                                       //where to write
    BitCoding::writeSizedElement(orsValue, uint8_t(Orientation(getValue(orientationsByPosition, thisPosIndex)) * op), 5, i);  //orientation to write
    i++;
  }
  for (i = 0; i < 8; i++) {
    uint8_t posInd = BitCoding::readSizedElement(posIndex, 5, i);
    setValue(pieceIndexesByPosition, BitCoding::readSizedElement(pieceIndex, 5, i), posInd);

    setValue(orientationsByPosition, BitCoding::readSizedElement(orsValue, 5, i), posInd);
  }
}
void CubeState::operator/=(const CubeOperation& other) {
  operator*=(CubeOperation(-other));
}

uint16_t ICubeState::statePenalty() const {
  uint16_t penalty = 0;
  for (int8_t z = -1; z < 2; z++)
    for (Vec8Iterator pos(z, z == 0); !pos.isEnd(); pos += 2) {
#ifdef PenaltyDebug
      pout(F("Edge at"));
      (*pos).print();
#endif
      Orientation ref = getOrientation(*pos);  //getOrientation(*pos);
      Axis edgeAx = Axis::throughZeroDimension(*pos);

      uint8_t forbidden = edgeAx.getDIndex();  //calculate edge origin CS
      Axis normals[2];
      uint8_t cursor = 0;
      for (uint8_t i = 0; i < 3; i++)
        if (i != forbidden) {
          normals[cursor] = Axis(i, ((*pos)[i] > 0));
          cursor++;
        }
      Orientation edgeOrigin(normals[0], normals[1]);
      Orientation edgeNow = (edgeOrigin * ref);
      uint16_t thisEdgePenalty = 1 + edgePenalty(edgeNow / edgeOrigin);
      uint16_t lengthPenalty = 1;

#ifdef PenaltyDebug
      pout(F(" axis "));
      edgeAx.getUnitVec().print();
      pout(F(" has orientation "));
      ref.print();
      pout(F(" was"));
      edgeOrigin.print();
      pout(F("->");
      (edgeNow).print();
      pout(F(" additional penalty "));
      poutN(edgePenalty(edgeNow / edgeOrigin));
#endif

      for (int8_t e = -1; e < 2; e += 2) {
#ifdef PenaltyDebug
        pout(F("Checking corners "));
        ((*pos) + ((edgeAx.getUnitVec()) * -1)).print();
        ((*pos) + ((edgeAx.getUnitVec()) * 1)).print();
        pnl();
#endif
        Orientation cornerOr = getOrientation(*pos + ((edgeAx.getUnitVec()) * e));
        if (cornerOr != ref) {
          lengthPenalty++;
          thisEdgePenalty += cornerPenalty(cornerOr) + cornerPenalty(cornerOr / edgeNow) / 2;
        }
      }
      thisEdgePenalty *= lengthPenalty;
      if (thisEdgePenalty > 1)
        penalty += thisEdgePenalty;
      //думать об исключении
    }
#ifdef PenaltyDebug
  pout(F("Calculated penalty="));
  pout(penalty);
  pnl();
#endif
  return penalty;
}

uint8_t ICubeState::indexOf(const Vec& position) {
  if ((abs(position[0]) + abs(position[1])) == 2)                                                         //diagonal columns
    return 8 + arcQuarter(position[0], position[1]) + 4 * (1 + position[2]);                              //4 quarters * 3 z;  12 indexes
  else                                                                                                    //2 line crosses
    return arcQuarter(position[0], position[1]) + 2 * (1 + position[2] /*{-1, 1}+=1=={0,2}*=2=={0,4}*/);  //4 quarters * 2 z; 8 indexes
}
Vec ICubeState::vectorOf(uint8_t index) {
  Vec temp;
  if (index > 7) {  //diagonal column
    index -= 8;
    temp = Vec(1, 1, index / 4 - 1);        //def (1,1); index+=4*(1+z); z=index/4 - 1
  } else {                                  //line crosses
    temp = Vec(1, 0, 2 * (index / 4) - 1);  //def (1,0);
  }
  // #ifdef CubeStateOperationDebug
  //   pout("Preset vec ");
  //   temp.print();
  // #endif
  index %= 4;  //quarter index=angle z

  return temp * Rotation(Axis::Z, index);
  ;
}
void CubeState::setValue(uint8_t* array, uint8_t value, uint8_t index) {
  BitCoding::writeSizedElement(array, value, 5, index);
}
uint8_t CubeState::getValue(uint8_t* array, uint8_t index) {
  return BitCoding::readSizedElement(array, 5, index);
}
void CubeState::setCube(Cube& other) const {
  for (uint8_t i = 0; i < 20; i++) {
    other.cubelets[getValue(pieceIndexesByPosition, i)].setOrientation(getValue(orientationsByPosition, i));
  }
}


uint8_t ICubeState::edgePenalty(const Orientation& i) {  //magic
  // return uint8_t(i)  / 6;
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

uint8_t ICubeState::cornerPenalty(const Orientation& i) {
  switch (i) {
    case 0: return 0;
    case 1: return 1;
    case 2: return 1;
    case 3: return 1;
    case 4: return 1;
    case 5: return 1;
    case 6: return 1;
    case 7: return 1;
    case 8: return 3;
    case 9: return 3;
    case 10: return 3;
    case 11: return 3;
    case 12: return 1;
    case 13: return 2;
    case 14: return 2;
    case 15: return 1;
    case 16: return 2;
    case 17: return 2;
    case 18: return 3;
    case 19: return 3;
    case 20: return 3;
    case 21: return 3;
    case 22: return 2;
    case 23: return 2;
  }
}

void CubeColors::initialize() {
  array = new Color[54];
  initialized = true;
#ifdef ColorDebug
  poutN(F("\n54 Colors created"));
#endif
}
void CubeColors::deinitialize() {
  delete[] array;
  array = NULL;
  initialized = false;

  delete[] normalMap;
  normalMap = NULL;
  indexed = false;
#ifdef ColorDebug
  poutN(F("\n54 Colors deleted"));
#endif
}
Color& CubeColors::operator[](Vec selection) {
  return array[indexOf(selection)];
}

void CubeColors::fakeScan(ICubeState& cube, float noizeRelative) {
  for (uint8_t i = 0; i < 54; i++) {
    Color temp;
    switch (cube.sideOfSelection(vectorOf(i))) {
      case 0:
        temp = Color::Red();
        break;
      case 1:
        temp = Color::Green();
        break;
      case 2:
        temp = Color::Blue();
        break;
      case 3:
        temp = Color::Orange();
        break;
      case 4:
        temp = Color::White();
        break;
      case 5:
        temp = Color::Yellow();
        break;
    }
    for (uint8_t i = 0; i < 3; i++)
      //                        -100;+100    -1.;+1.    -nzR;+nzR
      temp[i] += int16_t((float(random(0, 200)) - 100) / 100.0 * noizeRelative * float(temp[i]));
    array[i] = temp;
  }
}
void CubeColors::createPallet() {
  if (!initialized) {
    poutN(F("Error can't create pallet without Color array initialization!"));
    return;
  }
  delete[] normalMap;
  normalMap = new uint8_t[21]{ 0 };
  indexed = true;
  for (uint8_t side = 5; side > 0; side--) {
#ifdef PalletDebug
    printNormalMap();
    pnl();
    pout(F("Searching side "));
    Axis(side).printLetter();
    pnl();
#endif
    uint8_t simIndexes[8];
    uint8_t thisIndex = indexOf((Axis(side).getUnitVec()) * 2);
#ifdef PalletComparisonDebug
    pout(F("Central color "));
    array[thisIndex].print();
    pout(F("\t selected "));
    ((Axis(side).getUnitVec()) * 2).print();
    pout(F("\tby index "));
    poutN(thisIndex);
#endif
    ColorCode center(array[thisIndex]);
    setNormal(side, thisIndex);
    for (uint8_t criteria = 0; criteria < 2; criteria++) {
#ifdef PalletDebug
      if (!criteria) poutN(F("\t #1 relative colors"));
      else poutN(F("\t #2 abs values"));
#endif

      for (HalfDevision<float> similarity(0.001, 0.4); !similarity.isExpired();) {
        uint8_t similarIndex = 0;

        for (uint8_t searchIndex = 0; searchIndex < 54; searchIndex++) {
          if (getNormal(searchIndex) < side)  //unmarked color
            if (center.isSimilar(array[searchIndex], criteria, *similarity)) {
              if (similarIndex > 7) break;
              simIndexes[similarIndex] = searchIndex;
              similarIndex++;  //ends with 8
            }
        }

        if (similarIndex == 8) {  //deduced
          Color avg = array[thisIndex];
          setNormal(side, thisIndex);
          for (uint8_t i = 0; i < 8; i++) {  //assign indexes and pallet color
            uint8_t ind = simIndexes[i];
            avg += array[ind];
            setNormal(side, ind);
          }
          avg /= 9;
          pallet[side] = avg;

#ifdef PalletDebug
          pout(F("Found color "));
          avg.print();
          pout(F(" of side "));
          Axis(side).printLetter();
          pnl();
#endif
          goto nextSide;
        } else {
          if (similarIndex > 8) {
#ifdef PalletDebug
            pout(F("Too much similars. Decreasing marg. from "));
            pout(*similarity);
            pout(F("\tto "));
#endif
            similarity--;
#ifdef PalletDebug
            poutN(*similarity);
#endif
          } else {
#ifdef PalletDebug
            pout(F("Too little similars. Increasing marg. from "));
            pout(*similarity);
            pout(F("\tto "));
#endif
            similarity++;
#ifdef PalletDebug
            poutN(*similarity);
#endif
          }
        }
      }  //sim search
    }    //criterias
    //expired and no similars
    pout(F("Error creating pallet can't deduce similar in"));
    Axis(side).printLetter();
    poutN("!");
    indexed = false;
nextSide:;
  }  //pallet
  if (indexed) {
#ifdef PalletDebug
    pout(F("Last colors"));
#endif

    Color last(0, 0, 0);
    for (uint8_t i = 0; i < 54; i++) {
      if (getNormal(i) == 0) {

#ifdef PalletDebug
        array[i].print();
#endif
        last += array[i];
      }
    }
    last /= 9;
    pallet[0] = last;
  }
}
void CubeColors::writeCube(Cube& cube) const {
  if (!indexed) {
    poutN(F("Error can't write cube without indexes and pallet created!"));
    return;
  }
#ifdef ColorsWritingDebug
  poutN(F("Writing setting up cube by colors"));
#endif

  for (uint8_t pieceIndex = 0; pieceIndex < 20; pieceIndex++) {
    Vec piecePosition = cube.vectorOf(pieceIndex);  //{+-1,+-1,+-1}
    Axis csComponents[3];
    uint8_t cursor = 0;
    uint8_t axisGot = 0;

#ifdef ColorsWritingDebug
    pout(F("Piece #"));
    pout(pieceIndex);
    pout(F(" by\t"));
    piecePosition.print();
    pnl();
#endif
    for (uint8_t dI = 0; axisGot < 2; dI++) {
      if (piecePosition[dI]) {
        Vec selection = piecePosition;
        selection[dI] *= 2;
        Axis side = getNormal(indexOf(selection));
        uint8_t ortDI = side.getDIndex();
        csComponents[ortDI] = Axis(dI, side.isPositive() == (selection[dI] > 0));
        cursor += ortDI;
#ifdef ColorsWritingDebug
        pout(F("\tNormal selection "));
        selection.print();
        pout("\t");
        Axis(dI, selection[dI] > 0).printLetter();
        pout(F("\t cubelet side "));
        side.printLetter();
        pout(F("\t deduced: component["));
        pout(ortDI);
        pout("]=");
        csComponents[ortDI].printLetter();

        pnl();
#endif
        axisGot++;
      }
      if (dI > 2) {                                       //cursor cursor-1    0 1 2   i j i1 i2   shift
        pout(F("Error can't find 2 sides for piece at "));//0+1= 1    0       [i,j,_]->i,j  0  1     0
        piecePosition.print();                            //0+2= 2    1       [i,_,k]->k,i  2  0     2
        poutN("!");                                       //1+2= 3    2       [_,j,k]->j,k  1  2     1
      }
    }
    axisGot = Mod3(-(cursor - 1));
    Orientation pieceOrientation(csComponents[axisGot], csComponents[(axisGot + 1) % 3], axisGot);
#ifdef ColorsWritingDebug
    pout(F("\tdI sum="));
    pout(axisGot);
    pout(F("\t[i,j]=["));
    csComponents[axisGot].printLetter();
    pout(",");
    csComponents[(axisGot + 1) % 3].printLetter();
    pout(F("]\tshift="));
    pout(axisGot);
    pout(F("\tdeduced orientation\t"));
    pieceOrientation.print();
    pout(F("\torigin "));
    (piecePosition / pieceOrientation).print();
    pnl();
#endif
    cube[pieceIndex] = Cube::Piece(piecePosition / pieceOrientation, pieceOrientation);
  }
}
Vec CubeColors::vectorOf(uint8_t i) {
  Axis side = i / 9;
  Cs tempCs = Cs(side);
  i %= 9;
  Vec local(i / 3 - 1, i % 3 - 1, 2);
  return local * tempCs;
}
void CubeColors::print() const {
  if (initialized)
    for (uint8_t i = 0; i < 6; i++) {
      for (uint8_t j = 0; j < 9; j++) {
        array[i * 9 + j].print();
        pout("\t");
      }
      pnl();
    }
}
void CubeColors::printPallet() const {
  for (uint8_t i = 0; i < 6; i++) {
    pallet[i].print();
    pout("\t");
  }
}
void CubeColors::printNormalMap() {
  if (!indexed) {
    pout(F("Can't print not indexed!"));
    return;
  }
  pout("[");
  for (uint8_t i = 0; i < 54; i++)
    pout(getNormal(i));
  pout(F("]"));
}
uint8_t CubeColors::indexOf(const Vec& selection) {
  Axis side = Axis::maxDimension(selection);
  Cs tempCs(side);
  Vec local = selection / tempCs;
  return (9 * uint8_t(side) + 3 * (1 + local[0]) + (1 + local[1]));
}
void CubeColors::setNormal(uint8_t ax, uint8_t index) {
  BitCoding::writeSizedElement(normalMap, ax, 3, index);
}
uint8_t CubeColors::getNormal(uint8_t index) const {
  return BitCoding::readSizedElement(normalMap, 3, index);
}