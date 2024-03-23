#include "SCom.h"
class RobotState : public IUpdatable {
protected:
  static RobotState* currentState;
  virtual void activate() = 0;
  virtual void disable() = 0;
public:
  static void setActive(const RobotState& rs) {
    if (currentState != NULL)
      currentState->disable();
    currentState = &rs;
    rs.activate();
  }
  static void updateActive() {
    if (currentState != NULL)
      currentState->update();
  }
  static RobotState* getActiveState() {
    return currentState;
  }
};
RobotState* RobotState::currentState = NULL;

class ReadyState : public RobotState {
protected:
  virtual void activate() override {}
  virtual void disable() override {}
};

class PassiveState : public RobotState {  //RS without special activation or disability
public:
  virtual void update() override {}
};

class DoNothing : public ReadyState {
public:
  virtual void update() override {}
  DoNothing() {}
};

#include "CubeSolving.h"

class CO : public NextionScreen {  //control operations
  String topMessage;
  bool controlEnabled = false;

  virtual void loadScreen() override;
  virtual const CommandSet& getCommandSet() const override;
public:
  void updateTopMessage(const String& message);
  void updateControl(bool enabled);
  void loadControl();
  void loadMessage();
};
class BE : public DialogueScreen {
  virtual void loadScreen() override;
  virtual const CommandSet& getCommandSet() const override;

  friend void beginCubeControlD();
};
class EE : public DialogueScreen {
  virtual void loadScreen() override;
  virtual const CommandSet& getCommandSet() const override;

  friend void dialogueYes();
  friend void dialogueCancel();
};
class SES : public DialogueScreen {
  virtual void loadScreen() override;
  virtual const CommandSet& getCommandSet() const override;

  friend void scannerServoGoD();
};

class FestControl : public PassiveState {
  virtual void activate() override;
  virtual void disable() override;
protected:
  CO co;
public:
  OperationStack ops;
  void scrambleCube(uint8_t moves);
  void proceedRet();
};



class SD : public NextionScreen {
  virtual void loadScreen() override;
  virtual const CommandSet& getCommandSet() const override;
};
class CD : public NextionScreen {
  virtual void loadScreen() override;
  virtual const CommandSet& getCommandSet() const override;
};
class CC : public NextionScreen {
  virtual void loadScreen() override;
  virtual const CommandSet& getCommandSet() const override;
};

class BiosInvite : public DialogueScreen {
  virtual void loadScreen() override;
  virtual const CommandSet& getCommandSet() const override;
};
class Bios : public DialogueScreen {
  virtual void loadScreen() override;
  virtual const CommandSet& getCommandSet() const override;
};
