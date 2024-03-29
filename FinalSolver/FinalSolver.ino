#include "HardwareSerial.h"
#include "BitCoding.h"
#include "LowResGeometry.h"
#include "Cube.h"
#include "CubeSolving.h"
#include "SCom.h"
#include "RStates.h"

//System components
RobotMotorics rm(6 /*7*/);  //Motorics object

SecondaryListener cm2;                    //detecting nextion AWakenings
PortListener cm(&Serial1, NXBAUD, &cm2);  //nextion main communications + secondary listener cm2
PortListener pcm(&Serial, PCBAUD);        //pc communications

EEPROM_register reg;

void fullSystemUpdate() {  //update with communications
  rm.update();             //robot motorics

  cm.update();
  pcm.update();  //port listeners

  RobotState::updateActive();     //update robot state
  NextionScreen::updateActive();  //update current screen;
}
void screenInputEvent() {  //call input event function on active screen
  NextionScreen::getActiveScreen()->inputEvent();
}

//Command void batches [{declarations, array},...]
//standard void
comIterator standardVoids(uint8_t i) {
  return &screenInputEvent;
}
void startStart();  //start voids
comIterator startVoids(uint8_t i) {
  return &startStart;
}
void scannerServoGoD();  //scannerServo voids
comIterator scannerServoVoidsD(uint8_t i) {
  switch (i) {
    case 0: return &scannerServoGoD;
    default: return NULL;
  }
}
void fym();  //demo voids
void fyp();
void fzm();
void fzp();
void frc();
void fr();
void fdc();
void fd();
void fg();
void fes();
comIterator demoVoids(uint8_t i) {
  switch (i) {
    case 0: return &fym;
    case 1: return &fyp;
    case 2: return &fzm;
    case 3: return &fzp;
    case 4: return &frc;
    case 5: return &fr;
    case 6: return &fdc;
    case 7: return &fd;
    case 8: return &fg;
    case 9: return &fes;
    default: return NULL;
  }
}
void beginCubeControlD();  //beg voids
comIterator begVoidsD(uint8_t i) {
  switch (i) {
    case 0: return &beginCubeControlD;
    default: return NULL;
  }
}
void dialogueYes();
void dialogueCancel();
comIterator retVoidsD(uint8_t i) {
  switch (i) {
    case 0: return &dialogueCancel;
    case 1: return &dialogueYes;
    default: return NULL;
  }
}
void pcTest();  //pc test void
comIterator pcVoid(uint8_t i) {
  switch (i) {
    case 0: return &pcTest;
    default: return NULL;
  }
}

//Command char[][] arrays
const char PROGMEM editorChars[1][3] = { "ED\0" };
const char PROGMEM shortDChars[1][3] = { "SD\0" };
const char PROGMEM biosChars[1][3] = { "BS\0" };
const char PROGMEM settingsChars[1][5] = { "LS\0\0\0" };  //CnX
const char PROGMEM startChars[1][2] = { "AW" };
const char PROGMEM scannerServoCharsD[1][3] = { "S\0\0" };
const char PROGMEM demoChars[10][2] = { "Y-", "Y+", "Z-", "Z+", "R-", "R+", "D-", "D+", "GG", "GO" };
const char PROGMEM begCharsD[1][2] = { "B\0" };
const char PROGMEM retCharsD[2][1] = { 'C', 'Y' };
const char PROGMEM pcChars[1][1] = { "\0" };

//Command sets(CS_size, command_length, &comIterator, char[][]_ptr)
const CommandSet editorSet() {
  return CommandSet(1, 3, &standardVoids, &editorChars[0][0]);
}
const CommandSet shortDSet() {
  return CommandSet(1, 3, &standardVoids, &shortDChars[0][0]);
}
const CommandSet biosSet() {
  return CommandSet(1, 3, &standardVoids, &biosChars[0][0]);
}
const CommandSet settingsSet() {
  return CommandSet(1, 5, &standardVoids, &settingsChars[0][0]);
}
const CommandSet startSet() {
  return CommandSet(1, 2, &startVoids, &startChars[0][0]);
}
const CommandSet scannerServoSetD() {
  return CommandSet(1, 3, &scannerServoVoidsD, &scannerServoCharsD[0][0]);
}
const CommandSet demoSet() {
  return CommandSet(10, 2, &demoVoids, &demoChars[0][0]);
}
const CommandSet begSet() {
  return CommandSet(1, 2, &begVoidsD, &begCharsD[0][0]);
}
const CommandSet retSet() {
  return CommandSet(2, 1, &retVoidsD, &retCharsD[0][0]);
}
const CommandSet pcSet() {
  return CommandSet(1, 1, &pcVoid, &pcChars[0][0]);
}