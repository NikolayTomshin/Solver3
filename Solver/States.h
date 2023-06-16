#pragma once
#include "Awareness.h"

class NoState : public IRstate {
public:
  uint8_t validateBuffer(ValidBuffer* buffer) {
    if (buffer->available()) {
      switch (buffer->read()) {
        case 'Z':
          Serial.println("Za...");
          return 2;
        default:;
      }
    }
    return 0;
  }
  void update() {
  }
};
bool isNumber(char kar, uint8_t greaterEq = 0, uint8_t lesserEq = 9) {
  kar -= 48;
  return (greaterEq >= kar) && (kar <= lesserEq);
}
enum class EP { shift = 0,  //ClawUnit
                gHold = 1,
                gTouch = 2,
                gOpen = 3,
                dec = 4,
                sus = 5,   //sustainable
                fric = 6,  //
                mP = 7,
                cS = 8,
                eA = 9,
                eB = 10,
                mD = 11,
                reverse = 12,

                sPlace = 0,
                saCentre = 1,
                saEdge = 2,
                saCorner = 3,
                saFolded = 4,  //scanner

                col0 = 0,
                col1 = 2,
                col2 = 4,
                col3 = 6,
                col4 = 8,
                col5 = 10,  //cube profile
                cState = 12,

                orientation = 0 };
enum class EPS { c1 = 0,
                 c2 = 13,
                 s = 26,
                 p1 = 32,
                 p2 = 57,
                 p3 = 82,
                 orientation = 107 };
uint16_t ePShift = 0;                                      //no shift by default
uint16_t epAd(uint8_t subIndex, uint16_t fragmentIndex) {  //addres of eeprom cell
  return (ePShift + fragmentIndex + subIndex);
}
class StandBy : public IRstate {  //menus
  uint8_t activeNumber = 0;
  ClawUnit* active;
  ClawSetting copyBuffer;  //copy here
  uint8_t claw1 = 0, claw2 = 0;
  uint8_t clawStreamIndex = 0;  //0-no, 1-active(shift screen), 2-both(status screen)
  uint8_t selectedProfileIndex = 0;
public:
  void beginCustomAssembly();
  void beginFullAssembly();
  void stopScreenUpdates() {
    clawStreamIndex = 0;
  }
  void copyAngles();
  void copyDynamics();
  void copyPins();
  void beginMotorControl();
  void beginActionControl();
  void beginOperationControl();
  void defCube();
  void goServo(uint8_t place);
  void goScaner(uint8_t place);
  void beginSingleUpdate(bool left) {
    activeNumber = 2 - left;
    clawStreamIndex = 1;
  }
  void beginBothUpdate() {
    activeNumber = 0;
    clawStreamIndex = 2;
  }
  void loadClawAngles();
  void loadDynamics();
  void loadColor(uint8_t ortoindex);
  void loadPins();
  void loadPallet(uint8_t profileIndex);
  void loadProfiles();
  void loadScannerAngles();
  void updateScannerPos(uint8_t index);
  void beginFullScan(uint8_t profileIndex);
  void beginPalletScan(uint8_t profileIndex);

  uint8_t validateBuffer(ValidBuffer* buffer) {
    char c1, c2;
    if (buffer->available()) {
      switch (buffer->read()) {
        case '-':
          active->changeRotshift(false);  //negative
          active->assumeRotation();
          return 2;
        case '+':
          active->changeRotshift(true);  //positive
          active->assumeRotation();
          return 2;  //valid
        case 'B':
          if (buffer->available()) {
            c1 = buffer->read();  //store next
            switch (c1) {         //B-
              case 'A':           //BA-
                if (buffer->available()) {
                  c1 = buffer->read();  //c1 - 3rd symbol
                  switch (c1) {         //BA
                    case 'C':
                      beginCustomAssembly();
                      return 2;
                    case 'F':  //BF
                      beginFullAssembly();
                      return 2;
                    default:
                      if (isNumber(c1, 1, 3)) {  //c1 BA[1..3]
                        if (buffer->available()) {
                          c2 = buffer->read();                                 //BA[1..3]#
                          active->grabPositions[3 - (c1 - 48)] = uint8_t(c2);  //assign angle
                        } else return 1;
                      }  //not valid number
                  }
                } else return 1;
              case 'i':  //Bi
                stopScreenUpdates();
                return 2;
              default:                     //it's not over!
                if (isNumber(c1, 1, 3)) {  //c1 is number [1..3]
                  if (buffer->available()) {
                    c2 = buffer->read();  //B[1..3]#
                    switch (c1) {         //
                      case '1':
                        active->decrement = float(uint8_t(c2)) / 100;
                        return 2;
                      case '2':
                        active->chaseMinPower = uint8_t(c2);
                        return 2;
                      case '3':  //no use currenly
                        return 2;
                      default:
                    }
                  } else return 1;
                }
            }
          } else return 1;
        case 'b':  //b
          if (buffer->available()) {
            if (buffer->read() == 'i')    //bi
              if (buffer->available()) {  //bi#
                active->reverseDirection = uint8_t(buffer->read());
                return 2;
              } else return 1;
          } else return 1;
        case 'C':  //C
          if (buffer->available()) {
            switch (buffer->read()) {
              case 'B':  //CB
                if (buffer->available()) {
                  switch (buffer->read()) {
                    case 'A':  //CBA
                      copyAngles();
                      return 2;
                    case 'N':  //CBN
                      copyDynamics();
                      return 2;
                    case 'P':  //CBP
                      copyPins();
                      return 2;
                    default:;
                  }
                } else return 1;
              case 'M':  //CM
                beginMotorControl();
                return 2;
              case 'A':  //CA
                beginActionControl();
                return 2;
              case 'O':  //CO
                beginOperationControl();
                return 2;
              default:;
            }
          } else return 1;
        case 'D':  //D
          if (buffer->available()) {
            if (buffer->read() == 'C') {  //DC
              defCube();
              return 2;
            }
          } else return 1;
        case 'G':  //G
          if (buffer->available()) {
            switch (buffer->read()) {
              case 'B':
                if (buffer->available()) {
                  switch (buffer->read()) {
                    case '1':
                      active->setGrab(2);
                      return 2;
                    case '2':
                      active->setGrab(1);
                      return 2;
                    case '3':
                      active->setGrab(0);
                      return 2;
                    default:;
                  }
                } else return 1;
              case 'S':  //GS
                if (buffer->available()) {
                  switch (buffer->read()) {
                    case '0':
                      scanner.setPosition(3);
                      return 2;
                    case '1':
                      scanner.setPosition(2);
                      return 2;
                    case '2':
                      scanner.setPosition(1);
                      return 2;
                    case '3':
                      scanner.setPosition(0);
                      return 2;
                    default:;
                  }
                } else return 1;
              default:;
            }
          } else return 1;
      case 'H':
      case 'L':
      case 'p':
      case 'P':
      case 'S':
        default:;
      }
    }
    return 0;  //invalid
  }
  void updActive(uint8_t number) {
    switch (activeNumber) {
      case 2:
        active = &n2;
        break;
      case 1:
        active = &n1;
        break;
      default:;
    }
  }
  void updClaws() {
    claw1 = Mod(4, &n1.currentRotation);
    claw2 = Mod(4, &n2.currentRotation);
  }
  void updScreenClaw(uint8_t number, bool single) {  //number-claw block number
    uint8_t oldRotation;                             //old value
    uint8_t newRotation;
    if (number == 1) {  //set values
      oldRotation = claw1;
      newRotation = n1.currentRotation;
    } else {
      oldRotation = claw2;
      newRotation = n2.currentRotation;
    }
    newRotation = Mod(4, newRotation);

    if (oldRotation != newRotation) {  //if changed, update screen pic
      Serial1.print("b");              //b
      if (single) {
        Serial1.print("c");  //bc
      } else {
        Serial1.print("l");
        Serial1.print(number);  //bl<1,2>
      }
      Serial1.print(".pic=");
      newRotation += 12;
      Serial1.print(newRotation);  //+.pic={12+угол}
      comEnd();
    }
    newRotation -= 12;
    if (number = 1) {
      claw1 = newRotation;
    } else {
      claw2 = newRotation;
    }  //update coresponding rot value
  }

  void update() {
    switch (clawStreamIndex) {  //screen update coroutine
      case 2:
        updScreenClaw(1, false);
        updScreenClaw(2, false);
        break;
      case 1:
        updScreenClaw(activeNumber, true);
      default:;
    }
  }
};
