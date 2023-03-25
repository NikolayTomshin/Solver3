#include <Nextion.h>

#include "ClawUnit.h"
#include "TimeManager.h"

NexButton b0 = NexButton(0, 1, "b0");
NexButton b1 = NexButton(0, 2, "b1");  //страница дисплея, id и имя объекта
NexButton b2 = NexButton(0, 3, "b2");
NexButton b3 = NexButton(0, 4, "b3");  //страница дисплея, id и имя объекта
NexButton b4 = NexButton(0, 5, "b4");
NexButton b5 = NexButton(0, 6, "b5");

bool stop = false;
ClawUnit n1;
ClawUnit n2;
NexTouch *ManualList[] = { &b0, &b1, &b2, &b3, &b4, &b5 };  //массив ссылок на кнопки
Timer buttonTimer(20);
TimeManager test;

void b0PushCallback(void *ptr) {//grab 1
  n1.toggleGrab();
}
void b2PushCallback(void *ptr) {//grab 2
  n2.toggleGrab();
}

//left column
void b1PushCallback(void *ptr) {
  n1.runRotationMotor(true, 255);
}
void b4PushCallback(void *ptr) {
  n1.runRotationMotor(false, 255);
}
void b1PopCallback(void *ptr) {
  n1.runRotationMotor(true, 0);
}
void b4PopCallback(void *ptr) {
  n1.runRotationMotor(false, 0);
}

void b3PushCallback(void *ptr) {
  // n1.increaseTarget(2);
  n2.runRotationMotor(true, 255);
}
void b5PushCallback(void *ptr) {
  // n1.increaseTarget(-2);
  n2.runRotationMotor(false, 255);
}
void b3PopCallback(void *ptr) {
  n2.runRotationMotor(true, 0);
}
void b5PopCallback(void *ptr) {
  n2.runRotationMotor(true, 0);
}

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  n1 = ClawUnit(4, 5, 10, A0, A1);
  n2 = ClawUnit(7, 6, 11, A2, A3);
  b0.attachPush(b0PushCallback);
  b1.attachPush(b1PushCallback);
  b1.attachPop(b1PopCallback);
  b2.attachPush(b2PushCallback);
  b3.attachPush(b3PushCallback);
  b3.attachPop(b3PopCallback);
  b4.attachPush(b4PushCallback);
  b4.attachPop(b4PopCallback);
  b5.attachPush(b5PushCallback);
  b5.attachPop(b5PopCallback);
  n1.SetAngles(25, 155, 165);
  n2.SetAngles(0, 90, 150);//!!
  n1.SetChase(false);
  // n1.allignRotation();
}
void loop() {
  if (buttonTimer.isLoop()) nexLoop(ManualList);
  // n1.logEncoder();
  n1.update();
}
