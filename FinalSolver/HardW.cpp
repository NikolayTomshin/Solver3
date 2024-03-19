#include "HardW.h"



void IReady::waitReady() {
  while (!ready()) fullSystemUpdate();
}
void IReady::wait(void (*updateFunc)()) {
  while (!ready()) {
    updateFunc();
  }
}
void TimeStamp::setNow() {
  value = millis();
}
void TimeStamp::printSince() const {
  Serial.print(timeSince());
}
bool TimeStamp::isTimePassed(unsigned long ms) const {
  return timeSince() >= ms;
}
static void TimeStamp::waitFor(unsigned long time) {
  TimeStamp temp = now();
  while (!temp.isTimePassed(time))
    hardwareUpdate();
}
static void TimeStamp::waitUntil(TimeStamp time) {
  if (millis() < time)
    waitFor(time - millis());
}
unsigned long TimeStamp::timeSince() const {
  unsigned long now = millis();
  return (now > value) ? now - value : 0;
}
TimeStamp::operator unsigned long() const {
  return value;
}
unsigned long& TimeStamp::operator*() {
  return value;
}
const unsigned long& TimeStamp::operator*() const {
  return value;
}
TimeStamp& TimeStamp::operator+=(unsigned long time) {
  value += time;
  return *this;
}
TimeStamp& TimeStamp::operator-=(unsigned long time) {
  value -= time;
  return *this;
}

bool TimeStamp::operator>(const TimeStamp& other) const {
  return value > other.value;
}
bool TimeStamp::operator<(const TimeStamp& other) const {
  return value < other.value;
}
bool TimeStamp::operator>=(const TimeStamp& other) const {
  return value >= other.value;
}
bool TimeStamp::operator<=(const TimeStamp& other) const {
  return value <= other.value;
}
bool TimeStamp::operator==(const TimeStamp& other) const {
  return value == other.value;
}

void waitTime(unsigned long time) {
  TimeStamp::waitFor(time);
}
void waitUntil(TimeStamp time) {
  TimeStamp::waitUntil(time);
}

Timer::Timer(const TimeStamp& starting, unsigned long period) {
  started = starting;
  this->period = period;
}
Timer::Timer(unsigned long period) {
  reset();
  this->period = period;
}
Timer::Timer(const TimeStamp& ending) {
  reset();
  this->period = ending - started;
}
bool Timer::ready() const {
  return started.timeSince() >= period;
}
void Timer::waitReset() {
  waitReady();
  reset();
}
long Timer::timeLeft() const {
  return started - millis();
}
float Timer::relativeTimeLeft() const {
  return float(timeLeft()) / period;
}

void Timer::reset() {
  started = TimeStamp::now();
}

void StoppableTimer::setTimer(bool countingTime) {
  if (this->countingTime != countingTime) {
    if (countingTime)
      started += started.timeSince();
    else
      pausedTime = started.timeSince();
    this->countingTime = countingTime;
  }
}
bool StoppableTimer::isCounting() const {
  return countingTime;
}
bool StoppableTimer::ready() const {
  if (countingTime) {
    return ::Timer::ready();
  }
  return pausedTime;
}

LinChase::LinChase(uint16_t unitsSec, uint8_t u = 100, uint8_t d = 100) {  //create with defined move speed
  setSpeed(unitsSec, u, d);
}
void LinChase::setSpeed(uint16_t unitsPerSec, uint8_t u = 100, uint8_t d = 100) {  //set speed
  speed = unitsPerSec;
  upMod = u;
  dMod = d;
}
void LinChase::assumeDirection() {  //establish position relative to target
  underTarget = (x < target);
}
void LinChase::teleport(float place) {  //instantly change position
  x = place;
}
void LinChase::go(int16_t position) {  //tell point to move to position
  target = position;                   //set target
  assumeDirection();                   //know where to move
  going = true;                        //decide to go
  startedTime.setNow();                //reset timer
}
void LinChase::update() {                            //put in loop to precisely catch moment of arrival
  if (going) {                                       //if going
    unsigned long deltaT = startedTime.timeSince();  //count delta time
    // Serial.print("dT=");
    // Serial.println(deltaT);
    if (deltaT) {  //if not 0
      float step = assignSign<float>(underTarget,
                                     float(speed) * (float(upMod) / 100.0 * underTarget + float(dMod) / 100.0 * !underTarget) * deltaT / 1000.0);
      // Serial.print(deltaT);
      // Serial.write('\t');
      // Serial.print(step*1000);
      // Serial.write('\t');
      // Serial.print(step);
      // Serial.write('\t');
      // Serial.println(x);
      x += step;             //add delta to position
      startedTime.setNow();  //reset timer
      bool was = underTarget;
      assumeDirection();
      going = was == underTarget;  //if position relative to target didn't change than keep going
      if (going) return;
      x = float(target);  //else stop and fix position
    }
  }
}
bool LinChase::ready() const {  //ready when not going
  return (!going);
}

MyServo::MyServo(uint8_t unitsSec, int16_t startPosition, uint8_t u = 100, uint8_t d = 100)  //Construct as Servo and LinChase
  : Servo::Servo(), LinChase::LinChase(unitsSec, u, d) {
  teleport(startPosition);
}
void MyServo::write(int value) {
  Servo::write(value);
  go(value);
}

const uint8_t ClawUnit::numberOfConfigs() const {
  return 9;
}
Config ClawUnit::getConfig(uint8_t index) const {
  switch (index) {
    case 0:
      return Config(&grabPositions[0], sizeof(uint8_t), F("GbA0"), Config::Type::Uint);
    case 1:
      return Config(&grabPositions[1], sizeof(uint8_t), F("GbA1"), Config::Type::Uint);
    case 2:
      return Config(&grabPositions[2], sizeof(uint8_t), F("GbA2"), Config::Type::Uint);
    case 3:
      return Config(&grabPositions[2], sizeof(uint8_t), F("GbA2"), Config::Type::Uint);
    case 4:
      return Config(&reverseDirection, sizeof(bool), F("RD"), Config::Type::Bool);
    case 5:
      return Config(&rotShift, sizeof(uint8_t), F("RS"), Config::Type::Uint);
    case 6:
      return Config(&chaseMinPower, sizeof(uint8_t), F("MinPow"), Config::Type::Uint);
    case 7:
      return Config(&chaseBasePower, sizeof(uint8_t), F("MaxPow"), Config::Type::Uint);
    default: return Config(&chaseDecrement, sizeof(float), F("Decr"), Config::Type::Float);
  }
}

Timer ClawUnit::grabDelay = Timer(60);
const bool ClawUnit::discMap[2][4] = { { 0, 1, 1, 0 },
                                       { 0, 0, 1, 1 } };  //map of claw disk sectors (half)
ClawUnit::DiscoData* ClawUnit::disco = NULL;

void ClawUnit::setReverse(bool _reverse) {
  reverseDirection = _reverse;
}
uint8_t ClawUnit::getArrival() const {
  return arrivalState;
}
bool ClawUnit::isArrived(uint8_t howMuch) const {
  return (arrivalState >= howMuch);
}

ClawUnit::ClawUnit(uint8_t md, uint8_t mp, uint8_t sc, uint8_t a, uint8_t b) {
  mD = md;  //Rotary DC motor direction control pin;
  mP = mp;  //Rotary DC motor power control pin;
  eA = a;   //Pin for encoder A channel (lower track);
  eB = b;   //Pin for encoder B channel (upper track);
  servoGrab = MyServo(360, 240, 65, 100);
  servoGrab.teleport(0);
  reattach(sc);
  pinMode(a, INPUT);
  pinMode(b, INPUT);
  pinMode(mD, OUTPUT);  //Set dc control
  pinMode(mP, OUTPUT);  //pins to output;
  rampUpLerp = Lerp1<uint8_t>(chaseMinPower, chaseBasePower);
}
void ClawUnit::reattach(uint8_t sc) {
  servoGrab.detach();
  sC = sc;
  servoGrab.attach(sc);  //Attach servo to sc pin, it grabs cube;
}
void ClawUnit::setServoSpeed(uint16_t unitsPerSec, uint8_t u = 100, uint8_t d = 100) {
  servoGrab.setSpeed(unitsPerSec, u, d);
}
// set(uint8_t pin, uint8_t value) {
//   switch (pin) {
//     case 0:
//       mP = value;
//       break;
//     case 1:
//       cS = value;
//       break;
//     case 2:
//       eA = value + 18;
//       break;
//     case 3:
//       eB = value + 18;
//       break;
//     case 4:
//       mD = value;
//       break;
//     default:
//   }
// A
// 18
// 19
// 20
// 21
// 22
// 23
void ClawUnit::SetAngles(uint8_t releas, uint8_t hold, uint8_t grab) {  //set angles in degrees for states of grabbing
  grabPositions[0] = releas;
  grabPositions[1] = hold;
  grabPositions[2] = grab;
}
void ClawUnit::runRotationMotor(bool clockwise, uint8_t force) {
  // Serial.print("clockwise=");
  // Serial.println(clockwise);
  // Serial.print('\t');
  // Serial.println(force);

  analogWrite(mP, force);
  digitalWrite(mD, clockwise == reverseDirection);
  currentMotorForce = flipSign<float>(!clockwise, force);
}
void ClawUnit::setServo(uint8_t angle) {
  grabDelay.waitReset();
  servoGrab.write(angle);
  // Serial.println(angle);
}
void ClawUnit::setGrab(uint8_t positionIndex, uint8_t minusDelta = 0) {
  setServo(grabPositions[0] + positionIndex ? grabPositions[positionIndex] - minusDelta : 0);
  grabState = positionIndex;
  // Serial.print("SG");
  // Serial.println(grabState);
}
void ClawUnit::ease() {
  if (grabState > 0)
    setGrab(1 + (grabState % 2));
}
// void toggleGrab() {
//   setGrab(!(grabState > 0));
// }
void ClawUnit::orientationUpdate() {                           //updates encoder
  bool _ab[2] = { digitalRead(eA), digitalRead(eB) };          //remember current values
  uint8_t difference = (_ab[0] != ab[0]) + (_ab[1] != ab[1]);  //count differenses
  if (difference) {
    // Serial.print(_ab[0]);  //log AB channels
    // Serial.print(_ab[1]);
    // Serial.print(" ");
    switch (difference) {
      case 1:
        for (uint8_t i = 0; i < 2; i++) {                                      //step length
          uint8_t potential = Mod8(currentRotation + flipSign<int8_t>(i, 1));  //potential rotation index
          uint8_t searchIndex = Mod4(potential - rotShift);                    //potential index on disc map
          if (_ab[0] == discMap[0][searchIndex])
            if (_ab[1] == discMap[1][searchIndex])  //if potential channels = current
            {
              currentRotation = potential;  //potential is current
              break;                        //stop search
            }
        }
        break;
      default /*2*/: currentRotation = Mod8(currentRotation + flipSign<int8_t>(currentMotorForce < 0, 2));  //assume rotated with direction of motor power
    }
    ab[0] = _ab[0];
    ab[1] = _ab[1];  //new channel values assigning to static
    // Serial.print(mD);
    // Serial.print(currentRotation);
    // Serial.println(targetRotation);
    shouldCheckIfActionIsNeeded = true;
  }
}
void ClawUnit::setChase(bool _chase) {
  enableChase = _chase;
  if (_chase) {
    discoMode(false);
  }
}
void ClawUnit::setTarget(int8_t _target, bool enableJam = true) {
  targetRotation = Mod(8, _target);
  resetChase();
  shouldCheckIfActionIsNeeded = true;
  jamStatus = enableJam;  //initiate jam catching
  jamTimer.setNow();
}
void ClawUnit::increaseTarget(int8_t delta) {
  setTarget(targetRotation + delta);
  update();
}
void ClawUnit::resetChase() {
  oscilationNumber = 0;
  chaseRoutine = 0;
  arrivalState = 0;
}
uint8_t ClawUnit::oscilationPower() const {  // power to the motor vs oscilation number
  return (uint8_t(255 * pow(chaseDecrement, oscilationNumber - 1 + oscilationSmooth)));
}
void ClawUnit::setOscilationSmooth(uint8_t smooth = 0) {
  oscilationSmooth = smooth;
}
void ClawUnit::chase() {              //chases target
  if (shouldCheckIfActionIsNeeded) {  //if updated sector
    // Serial.print("Chasing ");
    if (jamStatus == 1) {  //reset jam timer if catching
      jamTimer.setNow();
    }
    if (currentRotation == targetRotation) {  //if over target
      // Serial.print("over :)");

      jamStatus = 0;
      arrivalState = 1;        //save it
      runRotationMotor(0, 0);  //turn off motor
      chaseRoutine = 1;        //turn on timer to arrival
      arrivalTime.setNow();    //reset timer for proper work
    } else {
      chaseDirection = (cycleDistanceVector(currentRotation, targetRotation, 8) > 0);  //if not over target define direction to target
      // Serial.print("dir");
      // Serial.println(cycleDistanceVector(currentRotation, targetRotation, 8));
      if (arrivalState) {         //if was over target recently
        if (arrivalState == 2) {  //if moved from outside forces
          // Serial.print("moved off :\\");

          jamStatus = 0;
          setChaseRampUp();  //start rampUpRoutin
        } else {             //else if just arrived and oscilated before millis passed
          // Serial.print("oscilating :\\");

          oscilationNumber++;                                   //count oscilation
          runRotationMotor(chaseDirection, oscilationPower());  //run motor with decreased power
          oscilationStall.setNow();                             //run routine for stall prevention
          chaseRoutine = 3;
        }
        arrivalState = 0;  //update arrival state
      } else {             //if is not and wasn't over target
        // Serial.print("Started run");
        // Serial.println(chaseBasePower);
        runRotationMotor(chaseDirection, chaseBasePower);
      }
    }
    shouldCheckIfActionIsNeeded = false;
  }
  if (chaseRoutine)  //chase routines
  {
    switch (chaseRoutine) {
      case 1:  //Arrival
        // Serial.print('a');
        if (arrivalTime.isTimePassed(150)) {  //if 300 millis over target assume it's stopped and arrived
          chaseRoutine = 0;                   //stop routines
          arrivalState = 2;                   //affirm arrival
          oscilationNumber = 0;               //reset oscilations
          oscilationSmooth = false;
          // Serial.print("Arrived");
        }
        break;
      case 2:  //RampUp
        // Serial.print('r');
        jamTimer.setNow();
        if (rampUpTime.ready()) {
          chasePower = 255;
          chaseRoutine = 0;
          jamStatus = 1;
          // Serial.print("MXP");
        } else {
          chasePower = rampUpLerp.getVal(rampUpTime.relativeTimeLeft());
        }
        runRotationMotor(chaseDirection, chasePower);
        break;
      case 3:  //oscilation stall control
        // Serial.print('s');
        jamTimer.setNow();
        if (oscilationStall.isTimePassed(200)) {
          resetChase();
          shouldCheckIfActionIsNeeded = true;
          jamStatus = 1;
          // Serial.print("Stall reset");
        }
        break;
      default:;
    }
  }
}
void ClawUnit::setChaseRampUp(uint8_t startPercent = 0) {
  chaseRoutine = 2;
  startPercent = limits<uint8_t>(startPercent, 100);
  rampUpTime = Timer(TimeStamp(millis() - 3 * startPercent), 300);
}
void ClawUnit::assumeRotation() {
  ab[0] = digitalRead(eA);
  ab[1] = digitalRead(eB);                                             //read encoder
  for (currentRotation = 0; currentRotation < 3; currentRotation++) {  //go through discmap 0..2
    uint8_t searchIndex = Mod4(currentRotation - rotShift);
    if ((discMap[0][searchIndex] == ab[0]) && (discMap[1][searchIndex] == ab[1])) {
      // Serial.print("Assumed: ");
      // Serial.println(searchIndex);
      return;
    }
  }
  // Serial.print("Assumed: ");
  currentRotation = 7;  //if 3 call it 7
  // Serial.print(currentRotation);
}
void ClawUnit::changeRotshift(bool positive) {
  int8_t delta = flipSign<int8_t>(!positive, 1);
  rotShift = Mod4(int8_t(rotShift) + delta);
  currentRotation = Mod4(currentRotation - delta);
  shouldCheckIfActionIsNeeded = true;
}
void ClawUnit::logEncoder() const {
  Serial.print(digitalRead(eA));
  Serial.println(digitalRead(eB));
}
void ClawUnit::update() {
  // Serial.print(getArrival());
  orientationUpdate();  //check orientation
  if (enableChase) {
    switch (jamStatus) {
      default:
        chase();  //chase in case
        break;
      case 1:
        chase();  //chase and try catch jam
        bool jamDetected;
        if (currentRotation % 2) {
          jamDetected = jamTimer.isTimePassed(300);  //1 diagonal
        } else {
          jamDetected = jamTimer.isTimePassed(100);  //0 straight
        }
        if (jamDetected) {
          // Serial.println("JAM");
          jamStatus = 2;
          jamTimer.setNow();
          runRotationMotor(!chaseDirection, chaseBasePower);
        }
        break;
      case 2:                              //unjam maneuver
        if (jamTimer.isTimePassed(250)) {  //stop unjaming timer
          jamTimer.setNow();
          jamStatus = 1;
          runRotationMotor(0, 0);
          shouldCheckIfActionIsNeeded = true;
        }
    }
  } else if (arrivalState == 3) {  //discoMod
    // Serial.print("d");
    discUpdate();
  }
  servoGrab.update();  //update servo prediction
}
void ClawUnit::setChasePower(uint8_t base, uint8_t min, float dec) {
  chaseBasePower = base;
  chaseMinPower = min;
  chaseDecrement = dec;
  rampUpLerp = Lerp1<uint8_t>(chaseMinPower, chaseBasePower);
}
void ClawUnit::changeLimit(int8_t i, int8_t d) {
  grabPositions[i] += d;
}
void ClawUnit::getTogether() {
  assumeRotation();
  setChase(true);
  setTarget(0, false);
}
bool ClawUnit::servoReady() const {
  return servoGrab.ready();
}
void ClawUnit::open() {
  setGrab(1, 20);
}
void ClawUnit::retractPartially() {
  setGrab(2, (grabPositions[2] - grabPositions[0]) * 0.85);
}
bool ClawUnit::nextQ() const {
  return !(disco->getQIndex(1 - ((currentRotation / 2) - disco->startOA)));
}
void ClawUnit::discUpdate() {                    //read timings and control speed
  if (shouldCheckIfActionIsNeeded && nextQ()) {  //after rotation update
    // Serial.println("NextQ");
    disco->setQTime(&arrivalTime);
    arrivalTime.setNow();         //enter time for past Q
    disco->incQIndex();           //inc Q index
    if (disco->getQIndex() == 0)  //if full circle inc rotations
    {
      disco->rotations++;
    }
    uint16_t sum = arSum<uint16_t>(disco->quarters, 4);
    uint16_t delta = sum - disco->lastRound;
    disco->lowError = abs(delta) <= 1;
    disco->lastRound = sum;
  }
}
bool ClawUnit::isTimeToShoot(uint8_t targetOA, float phaseShiftRelativeToQuarter, float sizeRelativeToQuarter) const {
  //                                     Middle of target window
  //                                              |
  //                          window left border  |  window right border
  //                                           |<-+->|=sizeRelativeToQuarter
  //                                           |  T  |
  //         phaseShiftRelativeToQuarter=|<----|->|  |
  //           ------[-1]---------------[0]----[windw]----[1]--
  //           previous=current       targetOA            next
  //localeRelativeTime=|------->|        |
  //                            |<-------|=timePassed+distanceToCurrent(=-1)
  if (disco->lowError && disco->lastLowError())  //condition of low error at this moment
  {
    int8_t currIndex = disco->getQIndex();                                                                //current index of disco
    float localeRelativeTime = (float(arrivalTime.timeSince()) + colorLag) / disco->quarters[currIndex];  //relative time in current Q
    sizeRelativeToQuarter /= 2;                                                                           //make half of width
    localeRelativeTime += cycleDistanceVector(targetOA, currIndex, 4);                                    //time in target Q axis
    //    0  1    T+1C
    //           -1 -2
    //    3  2    C+2
    //if locale is left of target it will decrease, else increase
    return ((phaseShiftRelativeToQuarter - sizeRelativeToQuarter) <= localeRelativeTime) && (localeRelativeTime <= (phaseShiftRelativeToQuarter + sizeRelativeToQuarter));
    //if [localeRelativeTime] between window borders that means time to shoot cause time window is NOW
  }
  return false;
}
bool ClawUnit::isDisco() const {
  return arrivalState == 3;
}
void ClawUnit::discoMode(bool ON) {
  if (isDisco() != ON)
    if (ON) {                                      //turning ON must be arrived at even rotation
      disco = new DiscoData(currentRotation / 2);  //create DD
      Serial.println("DISCO MODE");
      enableChase = false;  //turn off chase
      arrivalState = 3;     //mark DM
      runRotationMotor(1, 220);
      waitTime(300);
      runRotationMotor(1, 255);
      arrivalTime.setNow();
    } else {         //turniong OFF
      delete disco;  //delete DD
      resetChase();
      runRotationMotor(0, 0);  //turn of rotation
      waitTime(500);           //wait for claw to stop
    }
}


ClawUnit::DiscoData::DiscoData(uint8_t _startOA) {
  startOA = _startOA;
}
ClawUnit::DiscoData::~DiscoData() {
  Serial.println(F("Disco deleted"));
}
uint8_t ClawUnit::DiscoData::getQIndex(int8_t delta = 0) const {
  return Mod4(delta + quarterIndex);
}
void ClawUnit::DiscoData::incQIndex(int8_t delta = 1) {
  quarterIndex = getQIndex(delta);
}
long ClawUnit::DiscoData::setQTime(TimeStamp& time, int8_t delta = 0) {
  unsigned long elapsedTime = time.timeSince();
  time.setNow();
  quarters[quarterIndex + delta] = elapsedTime;
  return (elapsedTime);
}
void ClawUnit::DiscoData::printQ() const {
  for (uint8_t i = 0; i < 4; i++) {
    Serial.print(quarters[i]);
    Serial.write('\t');
  }
  Serial.write('\n');
}
bool ClawUnit::DiscoData::lastLowError() const {
  return abs(quarters[getQIndex(-1)] - lastRound / 4) == 1;
}
void setLed(bool on) {
  // Serial.print("Set ");
  // Serial.print(on);
  digitalWrite(13, on);
}

Scanner::Scanner(uint8_t sC, uint16_t degreesPerSec) {
  setLed(0);
  servo.attach(sC);
  servo.teleport(-180.0);
  servo.setSpeed(degreesPerSec);
}
void Scanner::goAngle(uint8_t degrees) {
  // Serial.print("A");
  // Serial.println(degrees);
  uint8_t angle;
  if (invertAngles) {
    angle = 180 - degrees;
  } else {
    angle = degrees;
  }
  servo.write(angle);
}
void Scanner::goPosition(uint8_t _position) {
  // Serial.print("SP");
  // Serial.println(position);
  if (position != _position) {
    goAngle(servoAngles[_position]);
    position = _position;
  }
}
bool Scanner::ready() const {
  return servo.ready();
}
void Scanner::update() {
  servo.update();
}
void Scanner::snap(Color& color) {
  tcs.begin();
  uint16_t r, g, b, trash;
  setLed(true);
  tcs.getRawData(&r, &g, &b, &trash);
  delay(colorLag - 2);
  tcs.getRawData(&r, &g, &b, &trash);
  setLed(false);
  color = Color(r, g, b);
  // Serial.println("DONE");
}

void waitIn() {
  Serial.print(F("waiting"));
  while (!Serial.available())
    hardwareUpdate();
  Serial.read();
}

SubOperation::SubOperation(uint8_t _code) {
  code = _code;
}
void SubOperation::setOperation(bool whole, const Axis& axis, const OrtAng& angle) {  //set operation pars
  setWhole(whole);                                                                    //seeting whole
  setAngle(angle);                                                                    //seting angle(better not zero!)
  setAxis(axis);                                                                      //setting axis
}
void SubOperation::setAnyExit() {  //stop exit not important
  code |= 0b11110000;              //make any exit codition
}
SubOperation::SubOperation(bool whole, const Axis& axis, const OrtAng& angle) {  //minimal operation
  setOperation(whole, axis, angle);                                              //set op
  setAnyExit();                                                                  //make any exit
}
SubOperation::SubOperation(bool whole, bool rightClaw, const OrtAng& angle) {
  setWhole(whole);  //seeting whole
  setAngle(angle);  //seting angle(better not zero!)
  BitCoding::writeBit(code, rightClaw, 1);
  setAnyExit();  //make any exit
}
void SubOperation::setExit(int8_t tX, int8_t tY) {  //encode motorics coordinate
  BitCoding::writeBits(code, ((tX + 1) + (tY + 1) * 3), 4, 4);
}
SubOperation::SubOperation(int8_t tX, int8_t tY, bool letRotate = false) {  //no op only exit
  setExit(tX, tY);                                                          //set exit
  setAngle(0);                                                              //no operation
  if (letRotate) {
    setWhole(true);  //allow rotation
  }
}
SubOperation::SubOperation(bool whole, const Axis& axis, const OrtAng& angle, int8_t tX, int8_t tY)  //full suboperation
{
  setOperation(whole, axis, angle);
  setExit(tX, tY);
}
void SubOperation::setWhole(bool whole) {  //rotate whole or side
  BitCoding::writeBits(code, whole, 0, 1);
}
void SubOperation::setAxis(const Axis& axisOV) {  //absolute Cs axis ortoindex (pointing from claw) left-Z-2-false; right-~y-4-true
  bool right = axisOV.getDIndex() & 1;
  BitCoding::writeBits(code, right, 1, 1);
  if (axisOV.isPositive() == right) setAngle(-getAngle());
}
void SubOperation::setAngle(const OrtAng& angle) {  //
  BitCoding::writeBits(code, uint8_t(angle), 2, 2);
}
OrtAng SubOperation::getAngle() const {  //get angle
  return OrtAng(BitCoding::readBits(code, 2, 2));
}
bool SubOperation::isRight() const {
  return BitCoding::readBits(code, 1, 1);
}
Axis SubOperation::getAxis() const {
  return Axis(isRight() * 2 + 2);
}
bool SubOperation::getWhole() const {
  return BitCoding::readBits(code, 0, 1);
}
bool SubOperation::isOperation() const {
  return uint8_t(getAngle());
}
bool SubOperation::exitRequired() const {
  return (code >> 4) <= 0b00001000;
}
bool SubOperation::isEmpty() const {
  return !isOperation() && !exitRequired();
}
SubOperation SubOperation::empty() {
  return SubOperation(0b11110000);
}
CubeOperation SubOperation::forCube(const Orientation& cubeCs) const {
  if (isOperation()) {
    // Serial.print("forCube=");
    // cubeCs.print();
    // Serial.print(" ax=");
    Axis ax = getAxis();
    // ax.printLetter();
    // Serial.print(" side=");
    ax = -ax;
    ax /= cubeCs.getCs();
    // (ax).printLetter();
    // Serial.println();
    return CubeOperation(ax, -getAngle());
  }
  return CubeOperation(0, 0);
}
int8_t SubOperation::getX() const {
  return (int8_t(BitCoding::readBits(code, 4, 4) % 3) - 1);  //{whole[1],axis[1],angle[2]/CodedExit[4]}
}
int8_t SubOperation::getY() const {
  return (int8_t(BitCoding::readBits(code, 4, 4) / 3) - 1);
}
void SubOperation::print() const {
  Serial.print(F("SO: "));
  BitCoding::printByte(code);
  Serial.print(" ");
  if (isOperation()) {
    if (getWhole()) {
      Serial.print(F("whole"));
    } else {
      Serial.print(F("side "));
    }
    Serial.print(" ax:");
    getAxis().printLetter();
    Serial.print(" ");
    if (isRight()) {
      Serial.print(F("right"));
    } else {
      Serial.print(F(" left"));
    }
    Serial.print(F(" ang="));
    Serial.print(uint8_t(getAngle()));
  } else {
    if (isEmpty()) {
      Serial.print(F("empty."));
      return;
    }
    Serial.print(F("no operation "));
    if (exitRequired())
      if (getWhole())
        Serial.print(F("may rotate"));
      else
        Serial.print(F("rotation prohibited"));
  }
  Serial.print(F(" exit "));
  if (exitRequired()) {
    Serial.print(F("tX="));
    Serial.print(getX());
    Serial.print(F("\ttY="));
    Serial.print(getY());
  } else {
    Serial.print(F("not required."));
  }
}
//MOTORICS=================================================================================================================================
const uint8_t RobotMotorics::numberOfConfigs() const {
  return 4;
}
Config RobotMotorics::getConfig(uint8_t index) const {
  switch (index) {
    case 0: return Config(&cubeSizeDelta, sizeof(uint8_t), F("sizeD"), Config::Type::Uint);
    case 1: return Config(&stabilityHealth, sizeof(uint8_t), F("stabHP"), Config::Type::Uint);
    case 2: return Config(&maxSmooth, sizeof(uint8_t), F("manSm"), Config::Type::Uint);
    default: return Config(&autoSmooth, sizeof(uint8_t), F("autoSm"), Config::Type::Uint);
  }
}

bool RobotMotorics::servosReady(uint8_t claws3Scanner) {
  switch (claws3Scanner) {
    case 0:
      return left.servoReady();
    case 1:
      return right.servoReady();
    case 2:
      return (left.servoReady() && right.servoReady());
    default:
      return scanner.ready();
  }
}
bool RobotMotorics::clawsReady(uint8_t targetArrival, uint8_t clawsNumber) const {
  switch (clawsNumber) {
    case 0:
      return left.isArrived(targetArrival);
    case 1:
      return right.isArrived(targetArrival);
    default:
      return left.isArrived(targetArrival) && right.isArrived(targetArrival);
  }
}
ClawUnit& RobotMotorics::getClaw(bool right) {
  return right ? this->right : left;
}
Axis RobotMotorics::getClawAxis(bool right) const {
  //Serial.print(right ? "right=" : "left=");
  //Axis(2 << right).printLetter();
  return Axis(2 << right);
}
void RobotMotorics::waitScanner() {
  scanner.wait(&hardwareUpdate);
}
void RobotMotorics::waitAnything(uint8_t arrival3Grab /*0,1,2|3*/, uint8_t one2Both3Scanner) {
  // Serial.write('(');
  // Serial.print(arrival3Grab);
  // Serial.write(';');
  // Serial.print(one2Both3Scanner);
  // Serial.write(')');
  //waiting for arrival

  // TimeStamp test = millis();
  if (arrival3Grab < 3) {
    // Serial.println("Waiting arrival");
    while (!clawsReady(arrival3Grab, one2Both3Scanner))  //arrival
      hardwareUpdate();
    // test.printSince();
    return;
  }
  //waiting for servos
  if (one2Both3Scanner < 4) {  //3 servos //claw or both
    // Serial.println("Waiting grabs");
    while (!servosReady(one2Both3Scanner)) {
      hardwareUpdate();
    }
    // test.printSince();
    return;
  }
  //wait for all simulteneously why ever?
  // Serial.println("Waiting all servos");
  while (!(servosReady(2) && servosReady(3)))
    hardwareUpdate();
  // test.printSince();
}

void RobotMotorics::waitAll() {
  waitAnything(3, 2);  //servos of claws
  //Serial.print("S");
  waitScanner();
  //Serial.print("-S");
  waitAnything(2, 2);  //arrival of claws
  //Serial.println("rot");
}
void RobotMotorics::setChase(bool chase) {
  left.setChase(chase);
  right.setChase(chase);
}
void RobotMotorics::syncGrab(uint8_t position, uint8_t minusDelta = 0) {
  left.setGrab(position, minusDelta);
  right.setGrab(position, minusDelta);
  waitAnything(3, 2);
}
void RobotMotorics::open() {
  syncGrab(1, 15);
}
void RobotMotorics::grabAction(bool right, uint8_t grabState, uint8_t minusDelta = 0) {
  getClaw(right).setGrab(grabState, minusDelta);
  waitAnything(3, right);
}
void RobotMotorics::scannerAction(uint8_t position) {
  scanner.goPosition(position);
  waitScanner();
}
void RobotMotorics::reGrab() {
  open();
  waitAnything(3, 2);
  syncGrab(1);
  waitAnything(3, 2);
}
void RobotMotorics::checkStability() {
  if (stabilityPoints < 0)
    if (!x) {
      // Serial.print("regrab");
      reGrab();
      stabilityPoints = stabilityHealth;
    }
}
void RobotMotorics::perfectGrab(bool rightMain, bool goingOutside) {
  if (goingOutside) {
    syncGrab(2);  //grab both
    waitAnything(3, rightMain);
    grabAction(rightMain, 0);  //release main
    return;
  }
  //going inside
  grabAction(rightMain, 2);  //grab good
  syncGrab(1);               //hold both
}

const Orientation& RobotMotorics::getCube() const {
  return cubeCs;
}
bool RobotMotorics::isDisco() const {
  return discoStatus >= 0;
}
void RobotMotorics::setState(uint8_t _x = 0, uint8_t _y = 0) {
  x = _x;
  y = _y;
}
void RobotMotorics::snapColor(int8_t localX, int8_t localY, Color& targetColor) {
  if (isDisco()) {
    bool notCentre = localX || localY;
    bool edge = bool(localX) != bool(localY);
    scannerAction(3 - notCentre * (2 - edge));
    do {
      if (notCentre) {
        while (!getClaw(discoStatus).isTimeToShoot(arcQuarter(localX, localY), edge ? -0.2 : 0.3, 0.03)) {
          hardwareUpdate();
        }
      }
      // Serial.println("Ready to snap");
      scanner.snap(targetColor);
    } while (targetColor.sum() < 15);
    ClawUnit::disco->lowError = false;
  } else Serial.print(F("Can't snap without disco"));
}
void RobotMotorics::setSmooth(bool smooth) {
  rotationSmoothValue = smooth ? 3 : 2;
}
void RobotMotorics::go(const SubOperation& targetOperation) {  //go do operation
  int8_t tX = 0, tY = 0;                                       //temp variables for target cords
#ifdef MotoricsDebug
  Serial.print(F("Starting pos"));
#endif  //MotoricsDebug

  stopDisco();         //stop disco
  waitAnything(3, 2);  //wait completion of last actions
  bool isOperation = targetOperation.isOperation();
  bool rightClaw = targetOperation.isRight();
  bool wholeRotation = targetOperation.getWhole();
  if (isOperation)
    if (busyClaw == rightClaw)
      if (lastWhole == wholeRotation)
        goto skipWaiting;
  waitAnything(2, 2);  //don't wait if operation for the same claw
  lastWhole = wholeRotation;
  busyClaw = rightClaw;
skipWaiting:
  //printCords();
  if (isOperation) {  //do operation via shortest path
    OrtAng angle = targetOperation.getAngle();
    bool oddAngle = uint8_t(angle) % 2;  //determine angle for later
    ClawUnit& mainClaw = getClaw(rightClaw);
#ifdef MotoricsDebug
    Serial.print(F("Op right:"));
    Serial.print(rightClaw);
#endif  //MotoricsDebug

    tX = flipSign<int8_t>(rightClaw, wholeRotation);  //X corresponds with operation type
#ifdef MotoricsDebug
    Serial.print(F("\t Cube:"));
    Serial.print(wholeRotation);
    Serial.print(F("\t Angle:"));
    Serial.println(int8_t(angle));
#endif  //MotoricsDebug

    if (wholeRotation)  //if whole cube then simply same y
      tY = y;
    else  //same y if sign = claw, else 0
      tY = y * (rightClaw == (y > 0));
    zMove(tX, tY);  //go to cords for operation preparation
    checkStability();
#ifdef MotoricsDebug
    Serial.print(F("Doing op"));
#endif                                           //MotoricsDebug
    mainClaw.increaseTarget(2 * int8_t(angle));  //main movement, because it's ready
    if (wholeRotation) {
      mainClaw.setChaseRampUp(96);
      mainClaw.setOscilationSmooth(rotationSmoothValue);
      // cubeCs.print();
      // Serial.print("\t");
      // getClawAxis(rightClaw).printLetter();
      // Serial.print(int8_t(angle));
      // Serial.print("->");
      cubeCs *= Rotation(getClawAxis(rightClaw), angle);  //rotate cube
      // cubeCs.print();
      // Serial.println();
      if (oddAngle) {                                        //might step into forbidden state beyond 3x3
        int8_t relativeY = flipSign<int8_t>(!rightClaw, y);  //for rightClaw far=-1, leftClaw far=1
        if ((relativeY == -1)) {
          getClaw(!rightClaw).increaseTarget(-2);  //rotate opposite claw to stay in 3x3 state map, if not desirable exit pathfinding will correct this decision
          y = -y;                                  //assign new y
          goto skipYIncrement;                     //skip standard new y assignment
        }
      }
    } else {
      if (savedOPs != NULL) {  //save history
        // Serial.print("doing ");
        // targetOperation.forCube(cubeCs).print();
        // Serial.println();
        savedOPs->pushOptimised(targetOperation.forCube(cubeCs));
      }
    }
    //hard to explain optimization of movement for time saving
    y += flipSign<int8_t>(y == rightClaw, oddAngle);  //standard step, if y=0 negative if left; else y==1 and negative or y==-1!=0 and positive
skipYIncrement:
#ifdef MotoricsDebug
    printCords()
#endif  //MotoricsDebug
      ;
  }
  if (targetOperation.exitRequired()) {
    if (isOperation && !wholeRotation) waitAnything(2, rightClaw);  //wait claw only if not whole rotation to not cause time loss
    tX = targetOperation.getX();
    tY = targetOperation.getY();
#ifdef MotoricsDebug
    Serial.print(F("Going to exit"));
    Serial.print('(');
    Serial.print(targetOperation.getX());
    Serial.print(';');
    Serial.print(targetOperation.getY());
    Serial.println(')');
#endif              //MotoricsDebug
    zMove(tX, tY);  //Zmove to required exit. If whole rotation required counter rotation of opposit claw it might instantly be cancelled
    checkStability();
  }
  // waitIn();
}
void RobotMotorics::go(const CubeOperation& cubeOperation) {  //perform operation to make cube Operation
  Axis targetSideAbs = cubeOperation.getAx();
  // Serial.print("cSide=");
  // targetSideAbs.printLetter();
  targetSideAbs *= cubeCs.getCs();  //where target side is pointing
  // Serial.print("ts=");
  // targetSideAbs.printLetter();
  // Serial.print(" ");
  OrtAng oa = -cubeOperation.getOA();  //how to rotate claw for this rotation
  //whole, right, angle(oa)
  if (targetSideAbs.getDIndex() == 0) {  //rotation from X
    // Serial.print("ts alongX");
    bool rotateWithRight = targetSideAbs.isPositive();
    go(SubOperation(true, !rotateWithRight, OrtAng(1)));
    go(SubOperation(false, rotateWithRight, oa));
    return;
  }
  switch (targetSideAbs) {  //instant rotation
    case Axis::Direction::nZ:
      go(SubOperation(false, false, oa));
      return;
    case Axis::Direction::Y:
      go(SubOperation(false, true, oa));
      return;
  }
  bool prepareWithRight = targetSideAbs.isPositive();  //rotation from up, positive=Z, reverse with right
  go(SubOperation(true, prepareWithRight, OrtAng(2)));
  go(SubOperation(false, !prepareWithRight, oa));
}

void RobotMotorics::printCords() const {
  Serial.print('(');
  Serial.print(x);
  Serial.print(';');
  Serial.print(y);
  Serial.println(')');
}
void RobotMotorics::stopDisco() {
  if (isDisco()) {
    scannerAction(0);
    ClawUnit& subject = getClaw(discoStatus);
    subject.discoMode(false);  //stop discoMode
    subject.setChase(true);
    subject.resetChase();
    waitAnything(2, discoStatus);
    zMove(0, y);
    discoStatus = -1;
  }
}
void RobotMotorics::goDisco(bool right) {
  if (!isDisco()) {
    int8_t tX = flipSign<int8_t>(right, 1);
    zMove(0, -tX);  //disco claw holding cube perpendicular
    reGrab();       //grab good
    grabAction(!right, 0);
    setState(tX, -tX);
    discoStatus = right;
    getClaw(right).discoMode(true);
    return;
  }
  Serial.println(F("Can't start second disco!"));
}
void RobotMotorics::scanCurrentSide(CubeColors& cubeColors) {  //scan current side into color array(MUST EXIST)
  goDisco(false);
  Serial.print(F("Starting scan. Cube:"));
  cubeCs.print();
  spn();
  Vec cursor = Vec(0, 0, 2);  //centre
  cursor /= cubeCs;
  snapColor(0, 0, cubeColors[cursor]);
  Serial.println(F("Rotating part"));
  for (uint8_t y = 0; y < 2; y++) {       //for edges and corners
    cursor = Vec(1, y, 2);                //initial cursor
    for (uint8_t oa = 0; oa < 4; oa++) {  //all rotations of cursor
      cursor *= Rotation(Axis(2), 1);     //clockwise in scannerCs is counter clockwise
      Serial.print(F("Local:\t"));
      cursor.print();
      spn();
      Vec target = cursor;
      target *= scanner.scannerCs;  //how it looks in real life
      Serial.print(F("Real:\t"));
      target.print();
      spn();
      target /= cubeCs;  //how it looks from cube
      Serial.print(F("Cube:\t"));
      target.print();
      spn();
      snapColor(cursor[0], cursor[1], cubeColors[target]);  //snap color into array
    }
  }
  stopDisco();
}

uint8_t RobotMotorics::zIndex(int8_t x, int8_t y) {  //zIndex by coordinates //index of state in snake pattern
  if (y) return 4 + y * 3 - x;                       //y=+-1; y*3=+-3; 6+y*3=9,3; x=-1..1; x-2=-3..-1; 9,3-x-2=8..6,2..0
  return (4 + x);
}
uint8_t RobotMotorics::currentZIndex() const {
  return zIndex(x, y);
}
void RobotMotorics::zMove(int8_t tX, int8_t tY) {  //z-move go to target
  uint8_t targetZIndex = zIndex(tX, tY);
  while (true) {
    uint8_t currentIndex = currentZIndex();
    if (currentIndex == targetZIndex) break;
    zAction(targetZIndex > currentIndex);
  }
}
void RobotMotorics::zAction(bool up) {  //move state through Z path
  uint8_t index = currentZIndex();
#ifdef MotoricsDebug
  Serial.print(F("Currently "));
  Serial.println(index);
  printCords();
#endif  //MotoricsDebug

  //             right servo
  //  [8]|(7)|[7]|(6)|[6]
  //     |||||   |||||(5)
  //  [3]|(3)|[4]|(4)|[5]
  //  (2)|||||   |||||
  //  [2]|(1)|[1]|(0)|[0]
  //left servo
  index -= !up;  //index = code of bridge->up=same index, down=index-1
#ifdef MotoricsDebug
  Serial.print(F("Bridge "));
  Serial.println(index);
#endif  //MotoricsDebug
  // waitIn();
  if (index <= 7)  //ok operation
    switch (index) {
      case 2:  //left-Z-2-false; right-~y-4-true
      case 5:  //cases of rotating claw that is not holding cube
        getClaw(index == 5).increaseTarget(2);
        waitAnything(1, 2);
        y += flipSign<int8_t>(!up, 1);
        break;
      default:                                                //case of performing hold/grab action
                                                              //1=7=!3; 0=6=!4;
        bool rightMain = !(index % 2);                        //right servo making big action in a sense that going between 0 and 1 grabState
        bool midleRow = ((index / 3) % 2);                    //bridge in middle row meaning direction is reversed
        bool goingOutside = up != midleRow != rightMain;      //going outside when up not midle and left, any change will inverse result
        perfectGrab(rightMain, goingOutside);                 //claws doing regrab
        x += flipSign<int8_t>(rightMain != goingOutside, 1);  //update state x coordinate
        --stabilityPoints;                                    //decrease stability after regrab
        break;
    }
  else Serial.println(F("Impossible Z move!"));  //not ok operation
#ifdef MotoricsDebug
  printCords();
  Serial.println(currentZIndex());
#endif
  // waitIn();
}

void RobotMotorics::update() {
  left.update();
  right.update();
  scanner.update();
}
void RobotMotorics::allignBoth() {
  left.getTogether();
  right.getTogether();
  setState(0, 0);
}
bool RobotMotorics::letGo() {
  if (holding) {
    go(SubOperation(0, 0));
    open();
    holding = false;
    cubeCs = 0;
    return true;
  }
  return false;
}
bool RobotMotorics::grab() {
  if (!holding) {
    syncGrab(1);
    reGrab();
    return holding = true;
  }
  return false;
}
void RobotMotorics::RAction(bool whole, const OrtAng& ang) {
  if (holding)
    go(SubOperation(whole, true, ang));
}
void RobotMotorics::LAction(bool whole, const OrtAng& ang) {
  if (holding)
    go(SubOperation(whole, false, ang));
}
bool RobotMotorics::isHolding() const {
  return holding;
}
void RobotMotorics::setHistorySave(OperationStack* savedOPs = NULL) {
  this->savedOPs = savedOPs;
}
void RobotMotorics::playBack() {
  if (savedOPs != NULL) {
    OperationStack* temp = savedOPs;
    savedOPs = NULL;
    while (temp->getSize()) {
      go(-temp->pop());
    }
    savedOPs = temp;
    return;
  }
  Serial.println(F("Attempted NULL playback"));
}
void RobotMotorics::scramble(uint8_t moves, long seed) {
  OperationStack randomOps;
  randomOps.scramble(moves, seed);
  while (randomOps.getSize())
    go(randomOps.pop());
}
void RobotMotorics::initialize() {
  left = ClawUnit(4, 5, 9, A1, A0);    //pins
  right = ClawUnit(7, 6, 10, A3, A2);  //pins
  right.changeRotshift(true);
  right.changeRotshift(true);                                     //fix right rotation
  left.SetAngles(1, 138 - cubeSizeDelta, 150 - cubeSizeDelta);    //angles left
  right.SetAngles(15, 143 - cubeSizeDelta, 155 - cubeSizeDelta);  //angles right
  left.setServoSpeed(300, 97, 100);
  right.setServoSpeed(300, 97, 100);
  left.setChasePower(255, 130, 0.90);  //low
  right.setChasePower(250, 130, 0.85);
  scanner = Scanner(8, 400.0);
  scanner.goPosition(0);
  //+
}