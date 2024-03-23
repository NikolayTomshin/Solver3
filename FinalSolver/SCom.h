#pragma once
#include "HardwareSerial.h"
#include <stdint.h>
#include <Arduino.h>
#include "Utilities.h"
#include "HardW.h"

//#define SCOMdebug
//#define DIALOGUEdebug

#define PCBAUD 115200
#define NXBAUD 9600

typedef void (*comVoid)();  //comVoid is pointer to void()
typedef comVoid (*comIterator)(uint8_t);

class CommandSet {  //container of commands and actions
protected:
  comIterator funcs;
  const char* masks;
  uint8_t size;
  uint8_t length;
public:
  static uint8_t args[2];  //buffer for command args

  CommandSet(uint8_t size, uint8_t length, comIterator funcsArray, const char* masks);
  const char* getMaskPtr(uint8_t index) const;
  char getChar(uint8_t comIndex, uint8_t charIndex) const;
  uint8_t getSize() const;
  uint8_t getLength() const;
  comIterator getFuncs() const;

  friend class CommandListener;
};

class ComBuffer {
protected:
  char* buffer = NULL;
  uint8_t comSize;
  uint8_t availableChars = 0;
public:
  ComBuffer() {}
  void print() const;
  void initialize(uint8_t comSize);   //before use
  void push(char c);                  //add symbol
  void flush();                       //empty
  char& operator[](uint8_t i) const;  //access
  uint8_t available() const;          //how many
  bool full() const;
};

class SecondaryListener;

class CommandListener {
protected:
  ComBuffer buffer;                 //loaded chars
  CommandSet* commandSet;           //commands
  SecondaryListener* dumpListener;  //dumped characters go there
  comIterator actions;              //void() selector

  CommandListener(SecondaryListener* dumpListener = NULL);
public:
  void checkBuffer();
  void setCommandSet(const CommandSet& commandSet);
  void printComs() const;
  void cascadeFlush();
};

class PortListener : public CommandListener, public IUpdatable {  //class for reading fixed length commands for state
protected:
  HardwareSerial* port;
  TimeStamp lastUpdate;
  TimeStamp lastCharLoaded;
  uint8_t lastAvailable = 0;
  uint8_t charLoadingTime;
public:
  PortListener(HardwareSerial* port, uint32_t baudRate, SecondaryListener* dumpListener = NULL);
  void flushPortToSubscriber();
  void update() override;
};

class SecondaryListener : public CommandListener {
public:
  SecondaryListener(SecondaryListener* dumpListener = NULL);
  void push(char c);  //load char externally
};

//nextion output
void comEnd();                                                                                                  //Nextion command termination
void loadSomething(const String& objName, const String& propName, const String& value, bool brackets = false);  //{objName}.{propName}={value}
void loadTxt(const String& name, const String& txt);                                                            //update text  {name}.txt="{txt}"
void loadVal(const String& name, uint16_t val);                                                                 //{name}.val={val}
void loadPic(const String& name, uint16_t pic);
void callFunction(const String& name, const String& par0 = "");  //{name} {pars}
void addParametre(const String& par);
void click(const String& name, bool press);  //click {name},[press/release]
void goNextionPage(const String& pagename);
const String& letterIndex(const String& letter, uint8_t index);
void loadSlider(uint8_t number, uint8_t value);
void setVisibility(const String& objName, bool visible);

class NextionScreen : public IUpdatable {
protected:
  class Element {  //Nextion responsive GUI element(or group)
  public:
    const String& elementNamePrefix;
    Element(const String& elementNamePrefix) {
      this->elementNamePrefix = elementNamePrefix;
    }
    virtual void load() = 0;                       //load element
    virtual void inputEvent(const char args[]) {}  //call input event with args*
  };
  class TextField {
    class Font {
      float averagePixelsPerChar;
    };
  public:
    uint8_t pixelWidth;
  };
  static NextionScreen* currentScreen = NULL;
  bool isActive = false;
public:
  static NextionScreen* getActiveScreen();
  static void updateActive() {
    if (currentScreen != NULL) currentScreen->update();
  }
  static void reloadActive();

  void setActive(PortListener& port);
  virtual void update() override {}
  static void deactivateScreen(PortListener& port);
  virtual void loadScreen() = 0;
  virtual const CommandSet& getCommandSet() const = 0;
  virtual void inputEvent() {}
};

class DialogueScreen : public NextionScreen, public IReady {
protected:
  static NextionScreen* fallBackScreen = NULL;
  static ValueStack<DialogueScreen*> screenStack;
  bool _ready = false;

  static char arg;
  static void endDialogue(char arg);
public:
  virtual bool ready() const override;
  char show(PortListener& port);
};

class PageScreen : public NextionScreen {
protected:
  class PageControl : public NextionScreen::Element {  //buttons and textfield to navigate pages
  protected:
    String pageFormatBefore;
    String pageFormatSeparator;
    String pageFormatAfter;

    uint8_t currentPage;
    uint8_t numberOfPages;  //all pages

    bool showLButton;
    bool showRButton;

    PageScreen* pageScreen;
  public:
    bool showText = true;
    virtual void load() override;

    PageControl(const String& elementNamePrefix, uint8_t numberOfPages, uint8_t startingPage,
                const String& pageFormatBefore,
                const String& pageFormatSeparator,
                const String& pageFormatAfter);
    static PageControl simplePC(const String& elementNamePrefix, uint8_t numberOfPages, uint8_t startingPage);

    void setPageScreen(PageScreen* pageScreen) {
      this->pageScreen = pageScreen;
    }
    virtual void setButtonEnabled(bool next, bool enabled);
    virtual bool goPage(uint8_t page);
    virtual bool nextPage();
    virtual bool prevPage();

    virtual void inputEvent(const char args[]) override;
  } * pageControl;
public:
  virtual bool nextPage() = 0;
  virtual bool prevPage() = 0;
  virtual void selectPage() = 0;
};
class CollectionScreen : public NextionScreen {  //screen with slots for items
protected:
  class CollectionControl : NextionScreen::Element {
  protected:
    class Item {
    public:
      virtual void loadItem(const String& collectionNamePrefix, uint8_t index) = 0;
    };
    PointerArray<Item> items;
    uint8_t startingItemIndex;
    uint8_t numberOfVisibleItems;

    virtual void hideItem(uint8_t index) = 0;
  public:
    void loadItems() const;
    void setItemVisible(uint8_t i, bool visible) const;
  } * collectionControl;
public:
  virtual void inputEvent() = 0;
};