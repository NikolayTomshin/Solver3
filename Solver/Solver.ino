#include "ClawUnit.h"
#include "TimeManager.h"
ClawUnit n2;
// ClawUnit N1(7,6,10,A0,A1);
uint8_t s;
Servo myServo;
bool a = 0;
void setup() {
  Serial.begin(9600);
  n2 = ClawUnit(7, 6, 115, 9, A2, A3);
  n2.release();
  // // n2.grab();
}
void loop() {
  n2.go(1);
  delay(5000);
  n2.stop();  
  delay(10000);
  // n2.Update();
  // uint8_t ns = n2.getState();
  // if (ns != s) {
  //   Serial.println(ns);
  //   s = ns;
  // }
}
