// #include "ClawUnit.h"
// #include "TimeManager.h"
// #include "CsT.h"
#include "States.h"
#include "stdlib.h"

void setup() {
  n1 = ClawUnit(4, 5, 9, A1, A0);   //pins
  n2 = ClawUnit(7, 6, 10, A3, A2);  //pins
  n2.changeRotshift(true);
  n2.changeRotshift(true);    //fix right rotation
  n1.SetAngles(0, 138, 152);  //angles left
  n2.SetAngles(4, 140, 155);  //angles right
  n1.setServoSpeed(300, 97, 100);
  n2.setServoSpeed(300, 97, 100);
  n1.setChasePower(255, 130, 0.92);  //low
  n2.setChasePower(255, 160, 0.95);
  scanner = Scanner(8, 400.0);
  scanner.goPosition(0);
  // Serial1.begin(115200);
  Serial.begin(9600);
  //tcs.begin();
  // while (!Serial1)
  //   ;
  while (!Serial)
    ;
    
  //n2.setGrab(2);
  
}
void systemUpdate() {
  n2.update();
  n1.update();
  scanner.update();
  // serialUpd();
}
void loop() {
  systemUpdate();
}
