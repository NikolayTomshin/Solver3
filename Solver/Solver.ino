#include <Nextion.h>

#include "ClawUnit.h"
#include "TimeManager.h"

NexButton b0 = NexButton(0, 1, "b0");
NexButton b1 = NexButton(0, 2, "b1");  //страница дисплея, id и имя объекта
NexButton b2 = NexButton(0, 3, "b2");
NexButton b3 = NexButton(0, 4, "b3");  //страница дисплея, id и имя объекта
NexButton b4 = NexButton(0, 5, "b4");
NexButton b5 = NexButton(0, 6, "b5");


ClawUnit n1;
ClawUnit n2;
NexTouch *ManualList[] = { &b0, &b1, &b2, &b3, &b4, &b5 };  //массив ссылок на кнопки

void b0PushCallback(void *ptr) {
}
void b2PushCallback(void *ptr) {
}

void b1PushCallback(void *ptr) {
}
void b4PushCallback(void *ptr) {
}
void b1PopCallback(void *ptr) {
}
void b4PopCallback(void *ptr) {
}

void b3PushCallback(void *ptr) {
}
void b5PushCallback(void *ptr) {
}
void b3PopCallback(void *ptr) {
}
void b5PopCallback(void *ptr) {
}

void setup() {
  Serial1.begin(9600);
  delay(100);
  n1=ClawUnit(4, 5, 10, A0, A1);
  n2=ClawUnit(7, 6, 9, A2, A3);
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
}
void loop() {
  nexLoop(ManualList);
}
