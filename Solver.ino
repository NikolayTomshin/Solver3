#include <Nextion.h>  // подключаем библиотеку
#include "ClawUnit.h"
#include "TimeManager.h"
uint8_t s;
ClawUnit n1(7, 6, 115, 9, A0, A1);
ClawUnit n2(4, 5, 130, 10, A2, A3);
bool a = 0;


NexButton b0 = NexButton(0, 1, "b0");
NexButton b1 = NexButton(0, 2, "b1");         //страница дисплея, id и имя объекта
NexButton b2 = NexButton(0, 3, "b2");
NexButton b3 = NexButton(0, 4, "b3");         //страница дисплея, id и имя объекта
NexButton b4 = NexButton(0, 5, "b4");
NexButton b5 = NexButton(0, 6, "b5");         //страница дисплея, id и имя объекта
NexTouch *nex_listen_list[] = { &b0, &b1, &b2, &b3, &b4, &b5 };  //массив ссылок на кнопки

void b0PushCallback(void *ptr) {
  n1.toggle();
}
void b1PushCallback(void *ptr) {
  n1.go(true);
}
void b2PushCallback(void *ptr) {
  n2.toggle();
}
void b3PushCallback(void *ptr) {
  n2.go(true);
}
void b4PushCallback(void *ptr) {
  n1.go(false);
}
void b5PushCallback(void *ptr) {
  n2.go(false);
}
void setup() {
  
  Serial.begin(9600);
  n2.release();
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
