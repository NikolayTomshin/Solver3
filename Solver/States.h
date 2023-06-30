#pragma once
#include <stdint.h>
#include "Awareness.h"
#include <EEPROM.h>

//  uint8_t numberOfCommands() override{}
//  String* getString(uint8_t index)override{}
//  void doCommand(uint8_t index)override{}
/////////////////eP locale addres
const uint8_t ePshift = 0;  //ClawUnit //local indexes
const uint8_t ePgHold = 1;
const uint8_t ePgTouch = 2;
const uint8_t ePgOpen = 3;
const uint8_t ePdec = 4;
const uint8_t ePsus = 5;   //sustainable
const uint8_t ePfric = 6;  //
const uint8_t ePmP = 7;
const uint8_t ePsC = 8;
const uint8_t ePeA = 9;
const uint8_t ePeB = 10;
const uint8_t ePmD = 11;
const uint8_t ePreverse = 12;

const uint8_t ePsPlace = 0;  //unused
const uint8_t ePsaCentre = 1;
const uint8_t ePsaEdge = 2;
const uint8_t ePsaCorner = 3;
const uint8_t ePsaFolded = 4;  //scanner

const uint8_t ePcol0 = 0;
const uint8_t ePcol1 = 2;
const uint8_t ePcol2 = 4;
const uint8_t ePcol3 = 6;
const uint8_t ePcol4 = 8;
const uint8_t ePcol5 = 10;  //cube profile
const uint8_t ePcState = 12;

const uint8_t ePorientation = 0;
////////////////////////////////global segment addres
const uint8_t ePSc1 = 0;  //segments
const uint8_t ePSc2 = 13;
const uint8_t ePSs = 26;
const uint8_t ePSp1 = 32;
const uint8_t ePSp2 = 57;
const uint8_t ePSp3 = 82;
const uint8_t ePSorientation = 107;  //first 108 bytes

uint16_t ePShift = 0;  //no shift by default

uint16_t epAd(uint8_t subIndex, uint16_t fragmentIndex) {  //addres of eeprom cell
  return (ePShift + fragmentIndex + subIndex);
}
template<class T>
uint8_t gEp(T a) {  //bad enum conversion
  return (uint8_t(a));
}
void ClawSetting::readEP(bool right) {         //copy values from eeprom to buffer
  uint16_t begIndex = epAd(0, right * ePSc2);  //calculate beginning index

  rotShift = EEPROM.read(begIndex + ePshift);

  mD = EEPROM.read(begIndex + ePmD);
  mP = EEPROM.read(begIndex + ePmP);
  eA = EEPROM.read(begIndex + ePeA);
  eB = EEPROM.read(begIndex + ePeB);
  sC = EEPROM.read(begIndex + ePsC);                     //Pins
  reverseDirection = EEPROM.read(begIndex + ePreverse);  //reverse direction
  for (uint8_t i = 0; i < 3; i++)
    grabPositions[i] = EEPROM.read(begIndex + ePgOpen - i);  //gHold <- gTouch <- gOpen <=

  decrement = float(EEPROM.read(begIndex + ePdec)) / 100;
  sustainable = EEPROM.read(begIndex + ePsus);
  min = EEPROM.read(begIndex + ePfric);  //unused
}
void ClawSetting::writeEP(bool right) {        //copy values from eeprom to buffer
  uint16_t begIndex = epAd(0, right * ePSc2);  //calculate beginning index

  EEPROM.update(begIndex + ePshift, rotShift);

  EEPROM.update(begIndex + ePmD, mD);
  EEPROM.update(begIndex + ePmP, mP);
  EEPROM.update(begIndex + ePeA, eA);
  EEPROM.update(begIndex + ePeB, eB);
  EEPROM.update(begIndex + ePsC, sC);                     //Pins
  EEPROM.update(begIndex + ePreverse, reverseDirection);  //reverse direction
  for (uint8_t i = 0; i < 3; i++)
    EEPROM.update(begIndex + ePgOpen - i, grabPositions[i]);  //gHold <- gTouch <- gOpen <=

  EEPROM.update(begIndex + ePdec, uint8_t(decrement * 100));
  EEPROM.update(begIndex + ePsus, sustainable);
  EEPROM.update(begIndex + ePfric, min);  //unused
}

void Scanner::saveSettings() {
  Serial.println("SAVING SCANNER");
  for (uint8_t i = 0; i < 4; i++) {  //angles
    EEPROM.update(epAd(ePsaFolded - i, ePSs), servoAngles[i]);
  }
}
void Scanner::loadSettings() {
  for (uint8_t i = 0; i < 4; i++) {  //angles
    servoAngles[i] = EEPROM.read(epAd(ePsaFolded - i, ePSs));
  }
}

ClawUnit* getBlock(bool right) {
  if (right) return &n2;
  else return &n1;
}
//Команды исходного состояния
const char cMin[] = "-";       //Уменьшить сдвиг
const char cPlus[] = "+";      //Увеличить сдвиг
const char cBp[] = "B[13#";    //Set параметры
const char cBAp[] = "BA[13#";  //Set угол блока
const char cBAC[] = "BAC";     //Начать настроенную сборку
const char cBAF[] = "BAF";     //Начать полную сборку
const char cBi[] = "Bi";       //Остановить все обновления
const char cbip[] = "bi#";     //Set инверсию мотора
const char cCBA[] = "CBA";     //Копировать углы блока
const char cCBN[] = "CBN";     //Копировать параметры блока
const char cCBP[] = "CBP";     //Копировать пины
const char cCM[] = "CM";       //Начать управление моторами
const char cCA[] = "CA";       //Начать управление действиями
const char cCO[] = "CO";       //Начать управление операциями
const char cDC[] = "DC";       //Определить куб
const char cGB[] = "GB[13";    //Повернуть серво блока на угол
const char cGS[] = "GS[03";    //Повернуть сканер на параметр
const char cH[] = "H[12";      //Начать обновление одного блока
const char cHi[] = "Hi";       //Начать обновление блоков
const char cLB1[] = "LB1";     //загрузить углы блока
const char cLB2[] = "LB2";     //загрузить параметры блока
const char cLCp[] = "LC[05";   //Загрузить значения цвета
const char cLp[] = "Lp";       //загрузить параметры пинов
const char cLP[] = "LP[13";    //Загрузить палитру слота
const char cLS[] = "LS";       //Загрузить слоты
const char cLs[] = "Ls";       //Загрузить углы сканера
const char cpp[] = "p[04#";    //Set номер пина
const char cPBA[] = "PBA";     //Вставить углы блока
const char cPBN[] = "PBN";     //Вставить параметры блока
const char cPBP[] = "PBP";     //Вставить пины
const char cS[] = "S[14";      //Положение сканера
const char cSAp[] = "SA[03#";  //Set угол сканера
const char cSF[] = "SF[13";    //Начать полное сканирование
const char csip[] = "si#";     //Set инверсия сканера
const char cSP[] = "SP[13";    //Начать сканирование палитры
const char cSS[] = "SS[13";    //Выбор слота

class StandBy : public IRstate {  //menus
  uint8_t lastScene = 9;
  bool touchedScanner = false;       //scanner settings changed
  uint8_t activeNumber = 2;          //0,1
  ClawUnit* active;                  //pointer to active claw unit during settings
  ClawSetting copyBuffer;            //copy here
  uint8_t clawRotation[2];           //old rotation angles of both claws
  uint8_t clawStreamIndex = 0;       //0-no, 1-active(shift screen), 2-both(status screen) If should update pics of claws on screen
  uint8_t selectedProfileIndex = 0;  //index of selected profile ツ
public:
  void reactivate() override {
    goPage(String(lastScene));
  }
  void saveAll() {
    for (uint8_t i = 0; i < 2; i++) {
      ClawSetting save;           //save
      save.copyAll(getBlock(i));  //read save of block
      save.writeEP(i);            //past save onto block
    }
  }
  StandBy(bool rdProm = false) {
    Serial.println("standing here");
    for (uint8_t i = 0; i < 2; i++) {
      ClawUnit* act = getBlock(i);  //local active block in this constructor
      clawRotation[i] = act->currentRotation;
      if (rdProm) {         //чтение из памяти
        ClawSetting save;   //save
        save.readEP(i);     //read save of block
        save.pastAll(act);  //past save onto block
      }
    }
    if (rdProm) scanner.loadSettings();
    goPage("MenuSt");
  }
  void updRS() {
    loadTxt("s", String(active->rotShift));
  }
  void beginCustomAssembly();
  void beginFullAssembly();
  void stopScreenUpdates() {
    if (clawStreamIndex == 1) {  //coming back from settings
      ClawSetting save;
      save.copyAll(active);
      save.writeEP(activeNumber);
    }
    // Serial.println("CSI=0");
    clawStreamIndex = 0;
  }
  void pastAngles();
  void pastDynamics();
  void pastPins();
  void beginMotorControl();
  void beginActionControl();
  void beginOperationControl();
  void defCube();
  void goServo(uint8_t place);
  void goScaner(uint8_t place);
  void beginSingleUpdate(bool right) {
    active = getBlock(right);
    activeNumber = right;
    updRS();
    clawStreamIndex = 1;  //one active
  }
  void beginBothUpdate() {
    // Serial.println("CSI=2");
    if (touchedScanner) {
      scanner.saveSettings();
      touchedScanner = false;
    }
    loadSomething(letterIndex("tm", 0), "en", "0");
    String name = "t3";
    loadSomething(name, "pco", "2016");
    loadTxt(name, "подключён");
    activeNumber = 2;
    clawStreamIndex = 2;
    updClaws();
    updScreenClaw(0, false);
    updScreenClaw(1, false);
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

  uint8_t numberOfCommands() override {
    return 36;
  }
  char* getString(uint8_t index) override {
    switch (index) {
      case 0: return &cMin[0];   //Уменьшить сдвиг
      case 1: return &cPlus[0];  //Увеличить сдвиг
      case 2: return &cBp[0];    // "B[13#",                                                                                                                                                                                                                                                                                                          //Set параметры
      case 3: return &cBAp[0];   //"BA[13#",                                                                                                                                                                                                                                                                                                //Set угол блока
      case 4: return &cBAC[0];   //"BAC",    //Начать настроенную сборку
      case 5: return &cBAF[0];   //"BAF",    //Начать полную сборку
      case 6: return &cBi[0];    //"Bi",     //Остановить все обновления
      case 7: return &cbip[0];   //"bi#",    //Set инверсию мотора
      case 8: return &cCBA[0];   //"CBA",    //Копировать углы блока
      case 9: return &cCBN[0];   //"CBN",    //Копировать параметры блока
      case 10: return &cCBP[0];  //"CBP",    //Копировать пины
      case 11: return &cCM[0];   //"CM",     //Начать управление моторами
      case 12: return &cCA[0];   //"CA",     //Начать управление действиями
      case 13: return &cCO[0];   //"CO",     //Начать управление операциями
      case 14: return &cDC[0];   //"DC",     //Определить куб
      case 15: return &cGB[0];   //"GB[13",  //Повернуть серво блока на угол
      case 16: return &cGS[0];   //"GS[03",  //Повернуть сканер на параметр
      case 17: return &cH[0];    // "H[12",  //Начать обновление одного блока
      case 18: return &cHi[0];   //"Hi",     //Начать обновление блоков
      case 19: return &cLB1[0];  //"LB1",    //загрузить углы блока
      case 20: return &cLB2[0];  //"LB2",    //загрузить параметры блока
      case 21: return &cLCp[0];  //"LC[05",  //Загрузить значения цвета
      case 22: return &cLp[0];   //"Lp",     //загрузить параметры пинов
      case 23: return &cLP[0];   //"LP[13",  //Загрузить палитру слота
      case 24: return &cLS[0];   //"LS",     //Загрузить слоты
      case 25: return &cLs[0];   //"Ls",     //Загрузить углы сканера
      case 26: return &cpp[0];   //"p[04#",  //Set номер пина
      case 27: return &cPBA[0];  //"PBA",    //Вставить углы блока
      case 28: return &cPBN[0];  //"PBN",    //Вставить параметры блока
      case 29: return &cPBP[0];  //"PBP",    //Вставить пины
      case 30: return &cS[0];    //"S[14",   //Положение сканера
      case 31: return &cSAp[0];  //"SA[03#", //Set угол сканера
      case 32: return &cSF[0];   //"SF[13",  //Начать полное сканирование
      case 33: return &csip[0];  //"si#",    //Set инверсия сканера
      case 34: return &cSP[0];   //"SP[13",  //Начать сканирование палитры
      case 35: return &cSS[0];   //"SS[13"   //Выбор слота
      default: return NULL;
    }
  }
  void doCommand(uint8_t index) override {
    uint8_t tempValue;
    switch (index) {
      case 0:  //-
        active->changeRotshift(false);
        updRS();
        break;
      case 1:  //+
        active->changeRotshift(true);
        updRS();
        break;
      case 2:  //B[1..3]#  //страница 2
        tempValue = comPars[1];
        switch (comPars[0]) {
          case '1':
            active->chaseDecrement = float(uint8_t(tempValue)) / 100;
            break;
          case '2':
            active->chaseMinPower = tempValue;
            break;
          case '3':
            //unused
            break;
          default:;
        }
        break;
      case 3:  //BA[1..3]# //страница 1 (углы)
        tempValue = comPars[1];
        switch (comPars[0]) {
          case '1':
            active->grabPositions[2] = tempValue;
            break;
          case '2':
            active->grabPositions[1] = tempValue;
            break;
          case '3':
            active->grabPositions[0] = tempValue;
            break;
          default:;
        }
        break;
      case 4:  //BAC
        beginCustomAssembly();
        break;
      case 5:  //BAF
        beginFullAssembly();
        break;
      case 6:  //Bi stop screen claw updates
        stopScreenUpdates();
        break;
      case 7:  //bi# set inversion
        active->reverseDirection = comPars[0];
        break;
      case 8:  //CBA
        copyBuffer.copyAngles(active);
        break;
      case 9:  //CBN
        copyBuffer.copyDynamics(active);
        break;
      case 10:  //CBP
        copyBuffer.copyPins(active);
        break;
      case 11:  //CM
        beginMotorControl();
        break;
      case 12:  //CA
        beginActionControl();
        break;
      case 13:  //CO
        beginOperationControl();
        break;
      case 14:  //DC
        defCube();
        break;
      case 15:  //GB[1..3] go active
        active->setGrab(3 - (comPars[0] - 48));
        break;
      case 16:  //GS[0..3]
        scanner.goPosition(3 - (comPars[0] - 48));
        break;
      case 17:  //H[1..2]
        beginSingleUpdate(comPars[0] == '2');
        break;
      case 18:  //Hi
        beginBothUpdate();
        break;
      case 19:  //LB1
        loadClawAngles();
        break;
      case 20:  //LB2
        loadDynamics();
        break;
      case 21:  //LC[0..5]
        loadColor(comPars[0] - 48);
        break;
      case 22:  //Lp
        loadPins();
        break;
      case 23:  //LP[1..3]
        loadPallet(comPars[0] - 48);
        break;
      case 24:  //LS
        loadProfiles();
        break;
      case 25:  //Ls
        loadScannerAngles();
        break;
      case 26:  //p[0..4]#
        tempValue = comPars[1];
        switch (comPars[0]) {
          case '0':
            active->mP = tempValue;
            break;
          case '1':
            active->reattach(tempValue);
            break;
          case '2':
            active->eA = tempValue + 18;
            break;
          case '3':
            active->eB = tempValue + 18;
            break;
          case '4':
            active->mD = tempValue;
            break;
          default:;
        }
        break;
      case 27:  //PBA past angles
        pastAngles();
        loadClawAngles();
        break;
      case 28:  //PBN
        pastDynamics();
        loadDynamics();
        break;
      case 29:  //PBP
        pastPins();
        loadPins();
        break;
      case 30:  //S[1..4]
        updateScannerPos(comPars[0] - 48 - 1);
        break;
      case 31:  //SA[0..3]#
        touchedScanner = true;
        scanner.servoAngles[3 - (comPars[0] - 48)] = comPars[1];
        break;
      case 32:  //SF[1..3]
        beginFullScan(comPars[0] - 48 - 1);
        break;
      case 33:  //si#
        scanner.invertAngles = bool(uint8_t(comPars[0]));
        break;
      case 34:  //SP[1..3]
        beginPalletScan(comPars[0] - 48 - 1);
        break;
      case 35:  //SS[1..3]
        selectedProfileIndex = comPars[0] - 48 - 1;
        break;
      default:;
    }
  }
  void updActive(uint8_t number) {
    if (number > 1)
      active = NULL;
    else
      active = getBlock(number);
  }
  void updClaws() {
    clawRotation[0] = Mod(4, &n1.currentRotation);
    clawRotation[1] = Mod(4, &n2.currentRotation);
  }
  void updScreenClaw(uint8_t number, bool single) {  //number-claw block number
    uint8_t oldRotation = clawRotation[number];      //old value
    uint8_t newRotation = Mod(4, getBlock(number)->currentRotation);
    // Serial.println(String(oldRotation) + String(newRotation));
    if (oldRotation != newRotation) {  //if changed, update screen pic
      String name = "b";
      if (single)
        name += "c";  //bc
      else
        name += letterIndex("l", number + 1);  //bk
      loadSomething(name, "pic", String(newRotation + 12));
      clawRotation[number] = newRotation;  //update coresponding rot value
    }
  }
  void update() {
    // Serial.println("rstUpd");
    switch (clawStreamIndex) {  //screen update coroutine
      case 2:
        updScreenClaw(0, false);
        updScreenClaw(1, false);
        break;
      case 1:
        updScreenClaw(activeNumber, true);
      default:;
    }
  }
};

void StandBy::beginCustomAssembly() {
}
void StandBy::beginFullAssembly() {
}
void notCopied() {
  popup(warn, " ", "Вставка не выполнена.\rЭти данные не были\rскопированны.");
}
void StandBy::pastAngles() {
  if (copyBuffer.copiedAngles) {
    copyBuffer.pastAngles(active);
  } else {
    notCopied();
  }
}
void StandBy::pastDynamics() {
  if (copyBuffer.copiedDynamics) {
    copyBuffer.pastDynamics(active);
  } else {
    notCopied();
  }
}
void StandBy::pastPins() {
  if (copyBuffer.copiedPins) {
    copyBuffer.pastPins(active);
  } else {
    notCopied();
  }
}
void StandBy::beginMotorControl() {
}
void StandBy::beginActionControl() {
}
void StandBy::beginOperationControl() {
}
void StandBy::defCube() {
}
void StandBy::goServo(uint8_t place) {
  active->setGrab(3 - place);
}
void StandBy::goScaner(uint8_t place) {
  scanner.goPosition(3 - place);
}
void StandBy::loadClawAngles() {
  for (uint8_t i = 1; i < 4; i++) {
    loadSlider(i, active->grabPositions[3 - i]);
  }
}
void StandBy::loadDynamics() {
  uint8_t tempValue;
  for (uint8_t i = 1; i < 3; i++) {
    switch (i) {
      case 1:
        // Serial.print("loading ");
        // Serial.print(uint8_t(active->chaseDecrement * 100));
        tempValue = uint8_t(active->chaseDecrement * 100);
        break;
      case 2:
        tempValue = active->chaseMinPower;
        break;
      default:
        tempValue = 0;
    }
    loadSlider(i, tempValue);
  }
}
void StandBy::loadColor(uint8_t ortoindex) {
}
void StandBy::loadPins() {
  uint8_t tempValue;
  for (uint8_t i = 0; i < 5; i++) {
    switch (i) {
      case 0:
        tempValue = active->mP;
        break;
      case 1:
        tempValue = active->sC;
        break;
      case 2:
        tempValue = active->eA - 18;
        break;
      case 3:
        tempValue = active->eB - 18;
        break;
      case 4:
        tempValue = active->mD;
        break;
    }
    loadVal('n' + String(i), tempValue);
  }
  loadVal("c0", uint8_t(active->reverseDirection));
}
void StandBy::loadPallet(uint8_t profileIndex) {
}
void StandBy::loadProfiles() {
}
void StandBy::loadScannerAngles() {
  Serial.print("loadingAngles");
  for (uint8_t i = 0; i < 4; i++) {
    loadSlider(i, scanner.servoAngles[3 - i]);
  }
}
void StandBy::updateScannerPos(uint8_t index) {
}
void StandBy::beginFullScan(uint8_t profileIndex) {
}
void StandBy::beginPalletScan(uint8_t profileIndex) {
}