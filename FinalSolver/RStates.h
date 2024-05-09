#pragma once
#include "SCom.h"
class RobotState {//: public IUpdatable {
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
  virtual void update()  {}
};

class DoNothing : public ReadyState {
public:
  virtual void update()  {}
  DoNothing() {}
};

#include "CubeSolving.h"

class CO : public NextionScreen {  //control operations
  String topMessage;
  bool controlEnabled = false;

  virtual void loadScreen() override;
  virtual const CommandSet getCommandSet() const override;
public:
  void updateTopMessage(const String& message);
  void updateControl(bool enabled);
  void loadControl();
  void loadMessage();
};
class BE : public DialogueScreen {
  virtual void loadScreen() override;
  virtual const CommandSet getCommandSet() const override;

  friend void beginCubeControlD();
};
class EE : public DialogueScreen {
  virtual void loadScreen() override;
  virtual const CommandSet getCommandSet() const override;

  friend void dialogueYes();
  friend void dialogueCancel();
};
class SES : public DialogueScreen {
  virtual void loadScreen() override;
  virtual const CommandSet getCommandSet() const override;

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
  virtual const CommandSet getCommandSet() const override;
};
class CD : public NextionScreen {
  virtual void loadScreen() override;
  virtual const CommandSet getCommandSet() const override;
};
class CC : public NextionScreen {
  virtual void loadScreen() override;
  virtual const CommandSet getCommandSet() const override;
};


#include "Settings.h"
class SettingsScreen : public DialogueScreen, public ListScreen, public TitledScreen, public ExitableScreen {
protected:
  class SettingItem : public CollectionScreen::CollectionControl::Item {
  protected:
    String name;
    Config* config;
    bool withPtr;
  public:
    SettingItem(Config* config, bool withPtr, const String& name);
    virtual void loadItem(const String& collectionNamePrefix, uint8_t index) override;
    virtual void inputEvent(const char args[]) override;

    void edit();
  };
  virtual const CommandSet getCommandSet() const override;
  SettingsScreen(){};
public:
  virtual void loadScreen() override;
  SettingsScreen(const Array<SettingItem*>& settingItems, const String& title, uint8_t numberOfVisibleItems);
  virtual void inputEvent() override;
  virtual void onExit() override;
};
class EditorScreen : public DialogueScreen, public TitledScreen {
protected:
  const Config* config;
  bool dirty = false;

  String repString;

  uint8_t cursorLimit;
  uint8_t cursor = 0;

  uint8_t flags;

  bool getBinary() const;
  bool getInt() const;
  bool getUint() const;
  //true when overtyping, false when actually inserting
  bool getInsert() const;
  bool getBool() const;
  bool getLocked() const;

  void setBinary(bool value);
  void setInt(bool value);
  void setUint(bool value);
  void setInsert(bool value);
  void setBool(bool value);
  void setLocked(bool value);
  void setFlags(bool Binary, bool Int, bool Uint, bool Insert, bool Bool, bool Locked);

  void updateString() const;

  void moveCursor(bool right);
  void enterChar(char c);
  void eraseChar();
  void negate();
  void putPoint();

  void apply();
  void cancel();
  static String elName(uint8_t i);
public:
  virtual const CommandSet getCommandSet() const override;

  ~EditorScreen();
  EditorScreen(Config* config, const String& title);
  virtual void loadScreen() override;
  virtual void inputEvent() override;
};
class BiosInvite : public DialogueScreen {
  virtual void loadScreen() override;
  virtual const CommandSet getCommandSet() const override;
  virtual void inputEvent() override;
};
class Bios : public DialogueScreen {
  virtual void loadScreen() override;
  virtual const CommandSet getCommandSet() const override;
  virtual void inputEvent() override;

  void showSettings();
};

class BiosSettings : public SettingsScreen {
  BiosSettings(const Array<SettingItem*>& settingItems, const String& title, uint8_t numberOfVisibleItems)
    : SettingsScreen(settingItems, title, numberOfVisibleItems) {}
public:
  BiosSettings();
};
