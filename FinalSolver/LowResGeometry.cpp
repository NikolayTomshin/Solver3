#include "LowResGeometry.h"

Vec::Vec(int8_t x = 0, int8_t y = 0, int8_t z = 0) {
  component[0] = x;
  component[1] = y;
  component[2] = z;
}

const int8_t& Vec::operator[](const uint8_t other) const {  //const
  return component[other];
}
int8_t& Vec::operator[](const uint8_t other) {  //not const
  return component[other % 3];
}

int8_t Vec::operator*(const Vec& other) const {  //scalar
  return int16_t(component[0]) * other[0] + component[1] * other[1] + component[2] * other[2];
}
static Vec xP(const Vec& first, const Vec& second) {
  return Vec(first[1] * second[2] - first[2] * second[1],
             -first[0] * second[2] + first[2] * second[0],
             first[0] * second[1] - first[1] * second[0]);
}  //cross product

Vec& Vec::operator+=(const Vec& other) {
  component[0] += other[0];
  component[1] += other[1];
  component[2] += other[2];
  return *this;
}
Vec Vec::operator+(const Vec& other) const {
  Vec sumVec = *this;
  sumVec += other;
  return sumVec;
}  //sum
Vec Vec::operator-() const {
  Vec negative = *this;
  negative[0] *= -1;
  negative[1] *= -1;
  negative[2] *= -1;
  return negative;
}
Vec Vec::operator-(const Vec& other) const {
  Vec difference = *this;
  difference -= other;
  return difference;
}  //sum
Vec& Vec::operator-=(const Vec& other) {
  component[0] -= other[0];
  component[1] -= other[1];
  component[2] -= other[2];
  return *this;
}
Vec& Vec::operator*=(int8_t other) {
  component[0] *= other;
  component[1] *= other;
  component[2] *= other;
  return *this;
}
Vec Vec::operator*(int8_t other) const {
  Vec temp = *this;
  temp *= other;
  return temp;
}  //multiplication
Vec operator*(int8_t i, const Vec& other) {
  return other * i;
}

Vec Vec::operator*(const Cs& other) const {  //transform
  Vec temp(0, 0, 0);
  for (uint8_t csAxIndex = 0; csAxIndex < 3; csAxIndex++) {  //for each cs component get vector component and add to dimension of csc with flipped sign if negative direction
    const Axis curAx = other[csAxIndex];
    temp[curAx.getDIndex()] += flipSign<int8_t>(!curAx.isPositive(), component[csAxIndex]);
  }
  return temp;
}
Vec Vec::operator/(const Cs& other) const {  //untransform
  Vec temp(0, 0, 0);
  for (uint8_t csAxIndex = 0; csAxIndex < 3; csAxIndex++) {  //get
    const Axis curAx = other[csAxIndex];
    temp[csAxIndex] += flipSign<int8_t>(!curAx.isPositive(), component[curAx.getDIndex()]);
  }
  return temp;
}
Vec& Vec::operator*=(const Cs& other) {  //transform this
  *this = (*this) * other;
  return *this;
}
Vec& Vec::operator/=(const Cs& other) {  //untransform this
  *this = (*this) / other;
  return *this;
}

static void Vec::ortoRotate(int8_t& x, int8_t& y, OrtAng angle) {  //apply rotation to coordinates
  uint8_t oa = angle;
  int8_t tempX;
  switch (oa) {
    case 1:
      tempX = x;
      x = -y;
      y = tempX;
      break;
    case 2:
      x = -x;
      y = -y;
      break;
    case 3:
      tempX = x;
      x = y;
      y = -tempX;
      break;
  }
}
Vec& Vec::operator*=(const Rotation& other) {  //rotate this
  Axis rotationAxis = other.getAx();
  uint8_t dIndex = rotationAxis.getDIndex();
  OrtAng rotationAngle = other.getOA();
  if (!rotationAxis.isPositive()) rotationAngle.reverse();
#ifdef RotDebug
  poutN(F("Rotating vector"));
  print();
#endif
  ortoRotate(component[Mod3(dIndex + 1)], component[Mod3(dIndex + 2)], rotationAngle);
#ifdef RotDebug
  pout(F("\tto\t"));
  print();
  pout("\n");
  rotationAxis.printLetter();
  pout("\t");
  pout(int8_t(rotationAngle));
  if (isOneDimensional()) {
    pout(F("\tAssuming axis:"));
    Axis temp = Axis(*this);
    temp.printLetter();
    pnl();
  }
  poutN(F("End of message"));
#endif
  return *this;
}
Vec& Vec::operator/=(const Rotation& other) {  //reverse rotate this
  return operator*=(-other);
}
Vec Vec::operator*(const Rotation& other) const {  //rotation result
  Vec temp = *this;
  temp *= other;
  return temp;
}
Vec Vec::operator/(const Rotation& other) const {  //reverse rotation result
  return operator*(-other);
}

Vec& Vec::operator*=(const Orientation& other) {
  return operator*=(other.getCs());
}
Vec& Vec::operator/=(const Orientation& other) {
  return operator/=(other.getCs());
}
Vec Vec::operator*(const Orientation& other) const {
  return operator*(other.getCs());
}
Vec Vec::operator/(const Orientation& other) const {
  return operator/(other.getCs());
}
bool Vec::operator==(const Vec& other) const {
  for (uint8_t i = 0; i < 3; i++)
    if (component[i] != other.component[i]) return false;
  return true;
}
bool Vec::isOneDimensional() const {
  return bool(component[0]) + bool(component[1]) + bool(component[2]) == 1;
}
bool Vec::isTwoDimensional() const {
  return bool(component[0]) + bool(component[1]) + bool(component[2]) == 2;
}

void Vec::print() {
  pout("(");
  for (uint8_t i = 0; i < 3; i++) {
    pout(component[i]);
    if (i < 2) pout(",\t");
  }
  pout(")");
}

Axis::Axis(Direction dir) {
  code = dir;
}
Axis::Axis(uint8_t ax) {
  code = ax % 6;
}
Axis::Axis(uint8_t dimensionIndex, bool positive) {
  code = dimensionIndex + 3 * (!positive);
#ifdef AxisDebug
  pout(F("New axis DI="));
  pout(dimensionIndex);
  pout(F("\t positive="));
  pout(positive);
  printLetter();
  pnl();
#endif
}
Axis::Axis(const Vec& oneDimensionalVector) {
  uint8_t dIndex = 0;
  int8_t temp;
  do {
    if (dIndex > 2) {
      poutN(F("Can't cast zero-vector to direction!"));
      break;
    }
    temp = oneDimensionalVector[dIndex];
#ifdef AxisDebug
    pout(F("Searching component="));
    poutN(temp);
#endif
    dIndex++;
  } while (temp == 0);
  dIndex--;
#ifdef AxisDebug
  pout(F("Vec axis DI="));
  pout(dIndex);
  pout(F("\t positive="));
  poutN(temp > 0);
#endif
  *this = Axis(dIndex, temp > 0);
#ifdef AxisDebug
  pout(F("Cur code:"));
  printLetter();
#endif
}

static Axis Axis::throughZeroDimension(const Vec& flat) {
  for (uint8_t i = 0; i < 3; i++) {
    if (!flat[i]) return Axis(i);
  }
  poutN(F("Error vector has no Zero-components!"));
  return Axis(0);
}
static Axis Axis::rightOrt(const Axis& i, const Axis& j) {
  return i * Rotation(j, -1);
}
static Axis Axis::lefttOrt(const Axis& i, const Axis& j) {
  return rightOrt(j, i);
}

Axis Axis::operator=(const Axis& other) {  //for override in Cs
  code = other.code;
  return *this;
}
Axis Axis::operator=(uint8_t other) {  //for override in Cs
  code = other;
  return *this;
}
Axis Axis::operator*=(const Rotation& other) {
  *this = Axis(getUnitVec() * other);
  return *this;
}
Axis Axis::operator/=(const Rotation& other) {
  *this = Axis(getUnitVec() / other);
  return *this;
}
Axis Axis::operator*(const Rotation& other) const {
  Axis temp = *this;
  temp *= other;
  return temp;
}
Axis Axis::operator/(const Rotation& other) const {
  Axis temp = *this;
  temp /= other;
  return temp;
}
Axis Axis::operator*=(const Cs& other) {
  *this = Axis(getUnitVec() * other);
  return *this;
}
Axis Axis::operator/=(const Cs& other) {
  *this = Axis(getUnitVec() / other);
  return *this;
}
Axis Axis::operator-() {
  return Axis(code + 3);
}
Axis::operator uint8_t() const {
  return code;
}

Vec Axis::getUnitVec() const {
  Vec unitVec(0, 0, 0);
  unitVec[getDIndex()] = assignSign<int8_t>(isPositive(), 1);
  return unitVec;
}
uint8_t Axis::getDIndex() const {
  return code % 3;
}
bool Axis::isPositive() const {
  return code < 3;
}
void Axis::printLetter() const {
  FStr letters;
  switch (code) {
    case 0:
      letters = F("+X");
      break;
    case 1:
      letters = F("+Y");
      break;
    case 2:
      letters = F("+Z");
      break;
    case 3:
      letters = F("-x");
      break;
    case 4:
      letters = F("-y");
      break;
    case 5:
      letters = F("-z");
      break;
    default: letters = F("ER");
  }
  pout(letters);
}
static Axis Axis::maxDimension(const Vec& vec) {
  uint8_t maxIndex = 0;
  int8_t maxVal = 0;
  uint8_t maxAbs = 0;
  for (uint8_t i = 0; i < 3; i++) {
    int8_t lastVal = vec[i];
    if (abs(lastVal) > maxAbs) {
      maxIndex = i;
      maxVal = lastVal;
      maxAbs = abs(lastVal);
    }
  }
  return Axis(maxIndex, maxVal > 0);
}

OrtAng::OrtAng(int8_t oa) {
  code = Mod4(oa);
}
void OrtAng::setAngle(int8_t oa) {
  *this = OrtAng(int8_t(oa));
}
OrtAng& OrtAng::reverse() {
  setAngle(-code);
  return *this;
}
OrtAng::operator uint8_t() const {
  return code;
}
OrtAng::operator int8_t() const {
  return code < 3 ? code : -1;
}
OrtAng OrtAng::operator-() const {
  return reverse();
}

Rotation::Rotation(const Axis& ax, const OrtAng& oa = 1) {
  setAxis(ax);
  setOrtAng(oa);
}
void Rotation::setAxis(const Axis& ax) {
  BitCoding::writeBits(code, uint8_t(ax), 0, 3);
}
void Rotation::setOrtAng(const OrtAng& oa) {
  BitCoding::writeBits(code, uint8_t(oa), 3, 2);
}
OrtAng Rotation::getOA() const {
  return OrtAng(BitCoding::readBits(code, 3, 2));
}
Axis Rotation::getAx() const {
  return Axis(BitCoding::readBits(code, 0, 3));
}
Rotation Rotation::operator-() const {
  Rotation temp = *this;
  temp.setOrtAng(-getOA());
  return temp;
}
Rotation::operator uint8_t() const {
  return code;
}

Cs::Cs() {}
Cs::Cs(const Axis& i = Axis::Direction::X, const Axis& j = Axis::Direction::Y, const Axis& k = Axis::Direction::Z) {
  setComponents(i, j, k);
}
Cs::Cs(uint8_t i = Axis::Direction::X, uint8_t j = Axis::Direction::Y, uint8_t k = Axis::Direction::Z) {
  setComponents(i, j, k);
}
Cs::Cs(const Axis& i, const Axis& j, int8_t shift = 0) {
  *this = Cs(i, j, Axis::rightOrt(i, j));
  if (shift) shiftComponents(shift);
}
Cs::Cs(const Axis& k) {
  Axis i = Axis(k + 1);
  *this = Cs(i, Axis::rightOrt(k, i), k);
}
void Cs::shiftComponents(int8_t shift) {
  Cs temp = *this;
  temp.setComponents(getComponentCode(Mod3(-shift)), getComponentCode(Mod3(-shift + 1)), getComponentCode(Mod3(-shift + 2)));
  *this = temp;
}

Axis Cs::operator[](uint8_t other) const {
  return Axis(getComponentCode(other));
}
uint8_t Cs::getComponentCode(uint8_t componentIndex) const {  //axis code
  return BitCoding::readSizedElement(code, 3, componentIndex);
}

void Cs::setComponent(uint8_t index, const Axis& ax) {
  BitCoding::writeSizedElement(code, ax, 3, index);
}
void Cs::setComponents(const Axis& i, const Axis& j, const Axis& k) {
  setComponent(0, i);
  setComponent(1, j);
  setComponent(2, k);
}
void Cs::setComponents(uint8_t i, uint8_t j, uint8_t k) {
  setComponent(0, i);
  setComponent(1, j);
  setComponent(2, k);
}

Cs& Cs::operator*=(const Rotation& other) {
  for (uint8_t i = 0; i < 3; i++) {
    //Vec tempV = ;
    setComponent(i, Axis((operator[](i).getUnitVec()) * other));
  }
  return *this;
}
Cs& Cs::operator/=(const Rotation& other) {
  for (uint8_t i = 0; i < 3; i++)
    setComponent(i, Axis((operator[](i).getUnitVec()) / other));
  return *this;
}
Cs Cs::operator*(const Rotation& other) const {
  return Cs(this) *= other;
}
Cs Cs::operator/(const Rotation& other) const {
  return Cs(this) /= other;
}

Cs& Cs::operator*=(const Cs& other) {
  for (uint8_t i = 0; i < 3; i++)
    setComponent(i, Axis(operator[](i).getUnitVec() * other));
  return *this;
}
Cs& Cs::operator/=(const Cs& other) {
#ifdef CsDebug
  pout(F("Orientation operator /= ! This is"));
  print();
  pout(F("\nOther = "));
  other.print();
  pnl();
#endif;
  for (uint8_t i = 0; i < 3; i++)
    setComponent(i, Axis(operator[](i).getUnitVec() / other));
#ifdef CsDebug
  pout(F("Orientation /= result in this "));
  print();
  pnl();
#endif;
  return *this;
}
Cs Cs::operator*(const Cs& other) const {
  Cs temp = *this;
  temp *= other;
  return temp;
}
Cs Cs::operator/(const Cs& other) const {
#ifdef CsDebug
  poutN(F("Cs operator / !"));
#endif;
  Cs temp = *this;
  temp /= other;
#ifdef CsDebug
  pout(F("Cs / result is"));
  temp.print();
  pnl();
#endif;
  return temp;
}

bool Cs::operator==(const Cs& other) const {
  return (code[0] == other.code[0]) && ((code[1] & 1) == (other.code[1] & 1));
}
bool Cs::operator!=(const Cs& other) const {
  return !operator==(other);
}

void Cs::print() {
  pout("(");
  for (uint8_t i = 0; i < 3; i++) {
    Axis(getComponentCode(i)).printLetter();
    if (i < 2) pout(",\t");
  }
  pout(")");
}
Orientation::OrientationNode::OrientationNode(const Cs& cs, uint8_t x, uint8_t y, uint8_t z, uint8_t _x, uint8_t _y, uint8_t _z)
  : Cs(cs) {
  for (uint8_t i = 0; i < 6; i++) {
    uint8_t temp;
    switch (i) {
      case 0:
        temp = x;
        break;
      case 1:
        temp = y;
        break;
      case 2:
        temp = z;
        break;
      case 3:
        temp = _x;
        break;
      case 4:
        temp = _y;
        break;
      case 5:
        temp = _z;
        break;
    }
    BitCoding::writeSizedElement(linksCode, temp, 5, i);
  }
}
const Orientation::OrientationNode Orientation::space[24] = { OrientationNode(Cs(0, 1, 2), 1, 2, 3, 4, 5, 6),
                                                              OrientationNode(Cs(0, 2, 4), 7, 8, 9, 0, 10, 11),
                                                              OrientationNode(Cs(5, 1, 0), 9, 12, 13, 14, 0, 8),
                                                              OrientationNode(Cs(1, 3, 2), 10, 9, 15, 13, 16, 0),
                                                              OrientationNode(Cs(0, 5, 1), 0, 13, 16, 7, 17, 14),
                                                              OrientationNode(Cs(2, 1, 3), 11, 0, 10, 16, 12, 17),
                                                              OrientationNode(Cs(4, 0, 2), 8, 14, 0, 17, 11, 15),
                                                              OrientationNode(Cs(0, 4, 5), 4, 18, 19, 1, 20, 21),
                                                              OrientationNode(Cs(5, 0, 4), 19, 22, 2, 6, 1, 18),
                                                              OrientationNode(Cs(1, 2, 0), 20, 19, 23, 2, 3, 1),
                                                              OrientationNode(Cs(2, 3, 4), 21, 1, 20, 3, 22, 5),
                                                              OrientationNode(Cs(4, 2, 3), 18, 6, 1, 5, 21, 23),
                                                              OrientationNode(Cs(3, 1, 5), 23, 5, 21, 22, 2, 19),
                                                              OrientationNode(Cs(5, 3, 1), 3, 23, 18, 21, 4, 2),
                                                              OrientationNode(Cs(4, 5, 0), 2, 21, 4, 20, 6, 22),
                                                              OrientationNode(Cs(3, 4, 2), 22, 20, 6, 23, 18, 3),
                                                              OrientationNode(Cs(1, 5, 3), 5, 3, 22, 18, 19, 4),
                                                              OrientationNode(Cs(2, 0, 1), 6, 4, 5, 19, 23, 20),
                                                              OrientationNode(Cs(5, 4, 3), 16, 15, 8, 11, 7, 13),
                                                              OrientationNode(Cs(1, 0, 5), 17, 16, 12, 8, 9, 7),
                                                              OrientationNode(Cs(2, 4, 0), 14, 7, 17, 9, 15, 10),
                                                              OrientationNode(Cs(4, 3, 5), 13, 11, 7, 10, 14, 12),
                                                              OrientationNode(Cs(3, 5, 4), 12, 10, 14, 15, 8, 16),
                                                              OrientationNode(Cs(3, 2, 1), 15, 17, 11, 12, 13, 9) };
static int8_t Orientation::indexOf(const Cs& cs) {
#ifdef OrientationSearch
  pout(F("Comparing"));
  cs.print();
  poutN(F(" to..."));
#endif
  for (uint8_t i = 0; i < 24; i++) {
#ifdef OrientationSearch
    pout(i);
    pout("\t");
    space[i].print();
    pnl();
#endif
    if (cs == space[i]) {
      return i;
    }
  }
#ifdef OrientationSearch
  pout(F("Can't find index of\t"));
  cs.print();
  poutN(F("  not real Orientation!"));
#endif
  return -1;
}
uint8_t Orientation::OrientationNode::getLink(uint8_t rotIndex) const {
  return BitCoding::readSizedElement(linksCode, 5, rotIndex);
}
void Orientation::rotate(uint8_t rotIndex) {
  index = space[index].getLink(rotIndex);
}
Orientation::Orientation(uint8_t _index) {
  index = _index;
}
Orientation::Orientation() {
  index = 0;
}
Orientation::Orientation(const Cs& cs) {
  int8_t temp = indexOf(cs);
  if (temp < 0) {
    pout(F("Error "));
    cs.print();
    poutN(F(" is not real Orientation! Creating default."));
    index = 0;
  } else index = temp;
}
Orientation::Orientation(const Axis& i, const Axis& j, int8_t shift = 0) {

#ifdef AxisDebug
  pout(F("Constructed cs from "));
  i.printLetter();
  pout(F(" and "));
  j.printLetter();
  pnl();
  temp.print();
  pnl();
#endif
  index = indexOf(Cs(i, j, shift));
}
void Orientation::setOrientation(const Orientation& other) {
  index = other.index;
}
Orientation::operator uint8_t() const {
  return index;
}
const Cs& Orientation::getCs() const {
  return space[index];
}

Orientation& Orientation::operator*=(const Rotation& other) {
  uint8_t rotIndex = other.getAx();  //axis of clockwise rotation
  // Serial.print(rotIndex);
  // Serial.print(",");
  switch (uint8_t(other.getOA())) {  //angle
    case 2:
      // Serial.print("case2");
      rotate(rotIndex);  //180`=single+single
    case 1:
      // Serial.print("case1");
      rotate(rotIndex);  //single clockwise
      break;
    case 3:
      // Serial.print("case3");
      rotate((rotIndex + 3) % 6);  //3=-1=single(negative axis)=Mod6(rotIndex+3)
  }
  return *this;
}
Orientation& Orientation::operator/=(const Rotation& other) {
  return operator*=(-other);
}
Orientation Orientation::operator*(const Rotation& other) const {
  Orientation temp = *this;
  temp *= other;
  return temp;
}
Orientation Orientation::operator/(const Rotation& other) const {
  return (*this) * (-other);
}

Orientation& Orientation::operator*=(const Orientation& other) {
  index = indexOf(getCs() * other.getCs());  //Cs * Cs
  return *this;
}
Orientation& Orientation::operator/=(const Orientation& other) {
#ifdef OrientationDebug
  pout(F("Orientation operator /= ! This is "));
  print();
  pout(F("\nother = "));
  other.print();
  pnl();
#endif;
  index = indexOf(getCs() / (other.getCs()));  // Cs / Cs
#ifdef OrientationDebug
  pout(F("Orientation /= result in this "));
  print();
#endif;
  return *this;
}
Orientation Orientation::operator*(const Orientation& other) const {
  Orientation temp = *this;
  temp *= other;
  return temp;
}
Orientation Orientation::operator/(const Orientation& other) const {
#ifdef OrientationDebug
  poutN(F("Orientation operator / !"));
#endif;
  Orientation temp = *this;
  temp /= other;

#ifdef OrientationDebug
  poutN(F("Orientation / result is "));
  temp.print();
#endif;
  return temp;
}

Orientation Orientation::operator-() const {
#ifdef OrientationDebug
  pout(F("Orientation operator - !. This is "));
  print();
  pnl();
#endif;
  Orientation temp;
#ifdef OrientationDebug
  pout(F("Created default orientation "));
  temp.print();
  pnl();
#endif;
  temp /= *this;
  return temp;
}

bool Orientation::isNeutral() const {
  return !index;
}
Axis Orientation::coincidentComponent(const Axis& inDirection) const {
  return inDirection /= getCs();
}
void Orientation::print() const {
  getCs().print();
}