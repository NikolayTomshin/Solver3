#pragma once

class IUpdatable {
public:
  virtual void update() {}
};

typedef void(Upd)();   //loop
typedef bool(Cond)();  //condition
class IReady;
void jdun(IReady* subject, Upd* _loop);

class IReady {
public:
  virtual bool ready();
  void wait(Upd* _loop) {
    jdun(this, _loop);
  }
};

void jdun(IReady* subject, Upd* _loop) {
  // Serial.println("jdoo");
  while (!(subject->ready())) {  //waiting condition
    _loop();                       //do while waiting
  }
}
void jdunStatic(Cond* condition, Upd* _loop) {
  while (!(condition)) {  //waiting condition
    _loop;                //do while waiting
  }
}
void systemUpdate();
void waitStatic(Cond* condition) {
  jdunStatic(condition, systemUpdate);
}