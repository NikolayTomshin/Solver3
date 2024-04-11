#pragma once
#include "HardwareSerial.h"
#include <stdint.h>
#include <Arduino.h>
#include "Utilities.h"
#include "HardW.h"

#define SCOMdebug
#define DIALOGUEdebug

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
  static uint8_t args[3];  //buffer for command args

  CommandSet() {}
  CommandSet(uint8_t size, uint8_t length, comIterator funcsArray, const char* masks);
  const char* getMaskPtr(uint8_t index) const;
  char getChar(uint8_t comIndex, uint8_t charIndex) const;
  uint8_t getSize() const;
  uint8_t getLength() const;
  comIterator getFuncs() const;

  void printComs() const;

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
  ~CommandListener();
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
String letterIndex(const String& letter, uint8_t index);
void loadSlider(uint8_t number, uint8_t value);
void setVisibility(const String& objName, bool visible);

class NextionScreen : public IUpdatable {
protected:
  class Inputable {
  public:
    virtual void inputEvent(const char args[]) {}  //call input event with args*
  };
  class Element : public Inputable {  //Nextion responsive GUI element(or group)
  public:
    Element();
    String elementNamePrefix;
    Element(const String& elementNamePrefix) {
      this->elementNamePrefix = elementNamePrefix;
    }
    virtual void load() = 0;  //load element
  };
  class TextField : public Element {
  protected:
    void loadMultiline();
    void loadMultilineCentered();
  public:
    class Font {
    protected:
      uint8_t charWidthAverageDpx;
      uint8_t bigLetterAdditionalPercents;
      uint8_t heightPixels;
    public:
      Font(uint8_t heightPixels, const float& avgCharWidthPixels, const float& avgBigLetterScale) {
        this->heightPixels = heightPixels;
        charWidthAverageDpx = uint8_t(avgCharWidthPixels * 10);
        bigLetterAdditionalPercents = uint8_t((avgBigLetterScale - 1) * 100);
      }
      static const Font& screenFonts(uint8_t fontID);         //font from Nextion by ID
      static const Font& screenFontsSizeDesc(uint8_t index);  //font by size in descending order
      static const uint8_t numberOfFonts();

      uint8_t lengthOfLine(const uint16_t& pixels);  //how many characters fit in pixels
    };

    String currentText;
    uint8_t fontID;
    uint16_t widthInPixels;
    enum class LoadMode : uint8_t {
      Line,
      Multiline,
      MultilineCentered
    } loadMode;

    TextField()
      : Element() {}
    TextField(const String& textFieldNamePrefix, uint8_t fontID, uint16_t widthInPixels, LoadMode loadMode);
    virtual void load() override;  //load element

    const Font& getFont() const;

    void setString(const String& string);
    void loadString(const String& string);
    void loadFit(const String& string);
    void changeFont(uint8_t fontID);
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
  virtual const CommandSet getCommandSet() const = 0;

  virtual void inputEvent() {}
};

class DialogueScreen : public NextionScreen, public IReady {
protected:
  static NextionScreen* fallBackScreen = NULL;
  static Stack<DialogueScreen*> screenStack;
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
    void print() const;
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

    uint8_t getCurrentPage() const;
    uint8_t getNumberOfPages() const;  //all pages
  } * pageControl;
public:
  ~PageScreen();
  virtual void onNextPage() = 0;
  virtual void onPrevPage() = 0;
  virtual void onSelectPage() = 0;
};
class TitledScreen : public NextionScreen {
protected:
  String title;
  TitledScreen() {}
public:
  TitledScreen(const String& title);
  void loadTitle() const;
  void loadTitle(const String& title);
};
class ExitableScreen : public NextionScreen {
protected:
  bool exitAllowed = true;
  //Update visibility of exit components
  virtual void updateExitComponents() const;
  virtual void onExit() = 0;
public:
  void setExitAllowed(bool allowed);
};
class CollectionScreen : public NextionScreen {  //screen with slots for items
protected:
  typedef void (*itemHider)(const String& collectionPrefix, uint8_t i);
  static void hideElement(const String& collectionPrefix, uint8_t i);
  class CollectionControl : public NextionScreen::Element {
  public:
    class Item : public NextionScreen::Inputable {
    public:
      virtual void loadItem(const String& collectionNamePrefix, uint8_t index) = 0;
    };
  protected:
    Array<Item*> items;
    uint8_t startingItemIndex;
    uint8_t numberOfVisibleItems;
    uint8_t upperLimitOfAccess() const;

    itemHider hideItem;  //void (uint) to hide item on screen
  public:
    CollectionControl(const String& collectionPrefix, uint8_t sizeOfCollection, uint8_t startingItemIndex, uint8_t numberOfVisibleItems, itemHider hideItem);

    void loadFromIndex(uint8_t startingItemIndex);
    void shiftLoadIndex(int8_t startingItemIndexShift);

    virtual void load() override;
    virtual void inputEvent(const char args[]) override;

    uint8_t getNumberOfVisibleItems() const;
    Item*& operator[](uint8_t i);

    void setItemVisible(uint8_t i, bool visible) const;
  } * collectionControl;
public:
  ~CollectionScreen();
};
class ListScreen : public PageScreen, public CollectionScreen {
protected:
  //nill
public:
  virtual void onNextPage() override;
  virtual void onPrevPage() override;
  virtual void onSelectPage() override;
};

class ShortDialogue : public DialogueScreen, TitledScreen {
protected:
  char answers[3] = { '\0' };
  const String l;
  const String m;
  const String r;

  const String& stringOf(uint8_t i) const;
  static char letterOf(uint8_t i);

  TextField* message = NULL;

  void initialize(const String& message,
                  char l, const String& messageL,
                  char m, const String& messageM,
                  char r, const String& messageR);
public:
  enum class Type : uint8_t {
    Notice,
    Warning,
    Error
  };
  enum class Options : uint8_t {
    Proceed,
    YesNo,
    YesNoCancel
  };
  ~ShortDialogue();
  ShortDialogue(Type type, Options options, const String& message);
  ShortDialogue(const String& title, const String& message,
                char l, const String& messageL,
                char m, const String& messageM,
                char r, const String& messageR);

  static ShortDialogue* notice(const String& message);
  static ShortDialogue* yn(const String& message);
  static ShortDialogue* ync(const String& message);
  static ShortDialogue* err(const String& message);

  virtual void loadScreen() override;
  virtual const CommandSet getCommandSet() const override;
  virtual void inputEvent() override;
};
