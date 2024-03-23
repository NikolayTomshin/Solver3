#include "HardwareSerial.h"
#include <avr/pgmspace.h>
#include "SCom.h"


uint8_t CommandSet::args[2] = { 0 };
CommandSet::CommandSet(uint8_t size, uint8_t length, const comIterator funcsArray, const char* masks) {
  this->size = size;
  this->length = length;
  this->funcs = funcsArray;
  this->masks = masks;
}
const char* CommandSet::getMaskPtr(uint8_t index) const {
  return masks + index * length;
}
char CommandSet::getChar(uint8_t comIndex, uint8_t charIndex) const {
  return pgm_read_byte_near(masks + length * comIndex + charIndex);
}
uint8_t CommandSet::getSize() const {
  return size;
}
uint8_t CommandSet::getLength() const {
  return length;
}
comIterator CommandSet::getFuncs() const {
  return funcs;
}

void ComBuffer::initialize(uint8_t comSize) {
#ifdef SCOMdebug
  Serial.print(F("new command length "));
  Serial.println(comSize);
#endif
  if (buffer != NULL)
    delete[] buffer;
  buffer = new char[comSize];
  this->comSize = comSize;
}
void ComBuffer::push(char c) {
  if (availableChars < comSize) {
    buffer[availableChars] = c;
    ++availableChars;
  }
}
void ComBuffer::print() const {
  for (uint8_t i = 0; i < availableChars; ++i)
    Serial.write(buffer[i]);
}
void ComBuffer::flush() {
  availableChars = 0;
}
char& ComBuffer::operator[](uint8_t i) const {
  return buffer[i];
}
uint8_t ComBuffer::available() const {
  return availableChars;
}
bool ComBuffer::full() const {
  return availableChars == comSize;
}


CommandListener::CommandListener(SecondaryListener* dumpListener = NULL) {
  this->dumpListener = dumpListener;
}
void CommandListener::checkBuffer() {

#ifdef SCOMdebug
  Serial.println(F("checking buffer"));
  delay(10);
#endif
  if (!buffer.full()) {
#ifdef SCOMdebug
    Serial.println(F("not full"));
#endif
    return;  //if not full, return
  }
#ifdef SCOMdebug
  buffer.print();
  Serial.println(F(" full"));
#endif
  uint8_t length = commandSet->length;
  uint8_t size = commandSet->size;
  for (uint8_t i = 0; i < size; ++i) {               //foreach command in set
                                                     //check if letters in buffer are valid as command
    const char* cPtr = commandSet->getMaskPtr(i);    //get pointer to first char of mask
    char* args = &CommandSet::args[0];               //pointer for next argument to put
    bool pass = true;                                //presume valid
    for (uint8_t j = 0; j < length; j++) {           //foreach character of buffer
                                                     //compare with mask char
      char maskChar = pgm_read_byte_near(cPtr + j);  //get mask char
      if (maskChar == '\0') {                        //if \0 accept any char and take to args
        *args = buffer[j];
        ++args;
        continue;
      }
      if (buffer[j] != maskChar) {  //if symbols don't match
        pass = false;
        break;
      }
    }            //end of chars comparison
    if (pass) {  //if command passed
#ifdef SCOMdebug
      Serial.print("=");
      for (uint8_t k = 0; k < length; k++)
        Serial.write(pgm_read_byte_near(cPtr + k));
      Serial.print(F("->commiting "));
      Serial.println(i);
      delay(100);
#endif                     //SCOMdebug
      (*(*actions)(i))();  //call command function
#ifdef SCOMdebug
      Serial.println(F("flushing"));
      delay(100);
#endif
      cascadeFlush();  //empty this buffer and of all subscribers
      return;
    }
  }  //end of command iterations
  //command wasn't validated
  if (dumpListener != NULL) {  //if subscriber exist send chars to it
#ifdef SCOMdebug
    Serial.print(F("Dumping "));
    buffer.print();
    Serial.println();
#endif
    for (uint8_t i = 0; i < length; i++)
      dumpListener->push(buffer[i]);
  }
  buffer.flush();  //flush buffer
}
void CommandListener::setCommandSet(const CommandSet& commandSet) {
  this->commandSet = &commandSet;
  this->actions = commandSet.getFuncs();
#ifdef SCOMdebug
  Serial.println(F("new commandSet"));
#endif
  buffer.initialize(commandSet.getLength());
}
void CommandListener::printComs() const {
  Serial.print(F("printing size="));
  Serial.print(commandSet->size);
  Serial.print(F(" length="));
  Serial.println(commandSet->length);
  for (uint8_t i = 0; i < commandSet->size; i++) {
    Serial.print(i);
    Serial.print(F("="));
    char* pt = commandSet->getMaskPtr(i);
    for (uint8_t j = 0; j < commandSet->length; j++)
      Serial.write(pgm_read_byte_near(pt + j));
    Serial.println();
  }
  if (dumpListener != NULL) {
    Serial.println(F("+"));
    dumpListener->printComs();
  }
}
void CommandListener::cascadeFlush() {
  buffer.flush();
  SecondaryListener* temp = dumpListener;
  while (temp != NULL) {
    temp->buffer.flush();
    temp = temp->dumpListener;
  }
}

void emptyPort(HardwareSerial& port) {
  while (port.available())
    port.read();
}

PortListener::PortListener(HardwareSerial* port, uint32_t baudRate, SecondaryListener* dumpListener = NULL)
  : CommandListener(dumpListener) {
  this->port = port;
  this->charLoadingTime = 10000 / baudRate + 1;  //bR(bits/s) bR/10000(chars/ms)
  lastUpdate.setNow();
}
void PortListener::flushPortToSubscriber() {
  if (dumpListener != NULL) {  //send all chars to subscriber
    while (port->available())
      dumpListener->push(port->read());
    return;
  }
  emptyPort(*port);
}
void PortListener::update() {
  if (!(*port)) return;  //if no connection, abort
  if (lastUpdate.timeSince() < charLoadingTime) return;
  if (commandSet == NULL) {   //if no command set
    flushPortToSubscriber();  //try flush to subscriber
    lastAvailable = 0;
    return;
  }
  if (port->available() > lastAvailable) {
#ifdef SCOMdebug
    Serial.print(F("new char ("));
    Serial.print(port->available());
    Serial.println(F(")"));
    delay(10);
#endif
    lastCharLoaded.setNow();
  }
  while (port->available() >= commandSet->getLength()) {  //while enough available for command length
#ifdef SCOMdebug
    Serial.println(F("loading line"));
    delay(10);
#endif
    for (uint8_t i = 0; i < commandSet->getLength(); ++i) {  //load into buffer
      buffer.push((char)port->read());
    }
    checkBuffer();  //check the buffer
  }
  lastAvailable = port->available();
  if (lastCharLoaded.timeSince() > charLoadingTime)
    if (lastUpdate > lastCharLoaded) {
      if (lastAvailable)
        flushPortToSubscriber();  //try flush to subscriber
      else
        cascadeFlush();
    }
  lastUpdate.setNow();
}
SecondaryListener::SecondaryListener(SecondaryListener* dumpListener = NULL)
  : CommandListener(dumpListener) {}
void SecondaryListener::push(char c) {
  buffer.push(c);
  checkBuffer();
}


//========================================================================================================================================================================
void comEnd() {  //Nextion command termination
  Serial1.print(F("\xFF\xFF\xFF"));
}

void loadSomething(const String& objName, const String& propName, const String& value, bool brackets = false) {  //{objName}.{propName}={value}
  comEnd();
  Serial1.print(objName);
  Serial1.write('.');
  Serial1.print(propName);
  Serial1.write('=');
  if (brackets) {
    Serial1.write('"');
    Serial1.print(value);
    Serial1.write('"');
  } else
    Serial1.print(value);
  comEnd();
}

void loadTxt(const String& name, const String& txt) {  //update text  {name}.txt="{txt}"
  loadSomething(name, F("txt"), txt, true);
}

void loadVal(const String& name, uint16_t val) {  //{name}.val={val}
  loadSomething(name, F("val"), String(val), false);
}
void loadPic(const String& name, uint16_t pic) {
  loadSomething(name, F("pic"), String(pic), false);
}
void callFunction(const String& name, const String& par0 = "") {  //{name} {pars}
  comEnd();
  Serial1.print(name);
  Serial1.write(' ');
  Serial1.print(par0);
}

void addParametre(const String& par) {  //use after call function to add parameter
  Serial1.write(',');
  Serial1.print(par);
}

void click(const String& name, bool press) {  //click {name},[press/release]
  callFunction(F("click"), name);
  addParametre(String(press));
  comEnd();
}

void goNextionPage(const String& pagename) {
  callFunction(F("page"), pagename);
  comEnd();
}

const String& letterIndex(const String& letter, uint8_t index) {
  return (letter + String(index));
}

void loadSlider(uint8_t number, uint8_t value) {
  const String name = letterIndex(F("h"), number);
  loadVal(name, value);
  click(name, false);
}

void setVisibility(const String& objName, bool visible) {
  callFunction(F("vis"), objName);
  addParametre(String(visible));
  comEnd();
}

//NextionScreen
NextionScreen* NextionScreen::currentScreen;

NextionScreen* NextionScreen::getActiveScreen() {
  return currentScreen;
}
void NextionScreen::setActive(PortListener& port) {
  if (currentScreen != NULL) currentScreen->isActive = false;
  currentScreen = this;
  isActive = true;
  port.setCommandSet(getCommandSet());
  loadScreen();
}
void NextionScreen::deactivateScreen(PortListener& port) {
  if (currentScreen != NULL) currentScreen->isActive = false;
  currentScreen = NULL;
  goNextionPage("0");
}
void NextionScreen::reloadActive() {
  if (currentScreen != NULL)
    currentScreen->loadScreen();
}


NextionScreen* DialogueScreen::fallBackScreen;
ValueStack<DialogueScreen*> DialogueScreen::screenStack;
char DialogueScreen::arg;

bool DialogueScreen::ready() const {
  return _ready;
}
char DialogueScreen::show(PortListener& port) {
  bool firstDialogue = screenStack.getSize() == 0;
  if (firstDialogue) fallBackScreen = getActiveScreen();
  screenStack.push(this);
  setActive(port);
#ifdef DIALOGUEdebug
  Serial.println(F("New dialogue"));
#endif
  waitReady();
  delete screenStack.pop();
#ifdef DIALOGUEdebug
  Serial.print(F("dialog popped"));
#endif
  if (firstDialogue)
    if (fallBackScreen != NULL)
      fallBackScreen->setActive(port);
    else
      deactivateScreen(port);
  else
    screenStack.peek()->setActive(port);
  return arg;
}
void DialogueScreen::endDialogue(char arg) {
  DialogueScreen::arg = arg;
  screenStack.peek()->_ready = true;
#ifdef DIALOGUEdebug
  Serial.print(F("dialog arg="));
  Serial.println(arg);
#endif
}

//PageScreen::
//PageScreen::PageControl::
void PageScreen::PageControl::load() {
  setVisibility(elementNamePrefix + F("L"), showLButton);
  setVisibility(elementNamePrefix + F("R"), showRButton);
  loadTxt(elementNamePrefix,
          showText ? pageFormatBefore + String(currentPage) + pageFormatSeparator + String(numberOfPages) + pageFormatAfter
                   : String(F("")));
}
PageScreen::PageControl PageScreen::PageControl::simplePC(const String& elementNamePrefix, uint8_t numberOfPages, uint8_t startingPage) {
  return PageControl(elementNamePrefix, numberOfPages, startingPage, F("Стр. "), F("/"), F(""));
}
PageScreen::PageControl::PageControl(const String& elementNamePrefix, uint8_t numberOfPages, uint8_t startingPage,
                                     const String& pageFormatBefore, const String& pageFormatSeparator, const String& pageFormatAfter)
  : NextionScreen::Element(elementNamePrefix) {
  currentPage = startingPage;
  this->numberOfPages = numberOfPages;
  this->pageFormatBefore = pageFormatBefore;
  this->pageFormatSeparator = pageFormatSeparator;
  this->pageFormatAfter = pageFormatAfter;
}
void PageScreen::PageControl::setButtonEnabled(bool next, bool enabled) {
  if (next)
    showRButton = enabled;
  else
    showLButton = enabled;
  String bName = elementNamePrefix + (next ? F("R") : F("L"));
  setVisibility(bName, enabled);
}
bool PageScreen::PageControl::goPage(uint8_t page) {
  if ((page < 1) || (page > numberOfPages)) return false;
  currentPage = page;
  setButtonEnabled(false, page > 1);
  setButtonEnabled(true, page < numberOfPages);
  return true;
}
bool PageScreen::PageControl::nextPage() {
  return goPage(currentPage + 1);
}
bool PageScreen::PageControl::prevPage() {
  return goPage(currentPage - 1);
}
void PageScreen::PageControl::inputEvent(const char args[]) {
  switch (args[0]) {
    case 'P':
      prevPage();
      pageScreen->prevPage();
      return;
    case 'N':
      nextPage();
      pageScreen->nextPage();
      return;
    case 'S':
      pageScreen->selectPage();
      return;
    default:;
  }
}
void CollectionScreen::CollectionControl::loadItems() const {
  uint8_t i = 0;
  uint8_t collectionIndex = startingItemIndex;
  //limitIndex = collectionIndex + min(numberOfVisibleItems, restOfItemsInCollection)
  uint8_t limitIndex = collectionIndex + minMax<uint8_t>(numberOfVisibleItems, items.getSize() - collectionIndex, false);
  if (collectionIndex < items.getSize())  //in this case items.getSize() - collectionIndex would result in overflow
    while (collectionIndex < limitIndex) {
      items[collectionIndex]->loadItem(elementNamePrefix, i);
      ++i;
      ++collectionIndex;
    }
  for (; i < numberOfVisibleItems; ++i)
    hideItem(i);
}
void CollectionScreen::CollectionControl::setItemVisible(uint8_t i, bool visible) const {
  if (i >= numberOfVisibleItems) return;
  if (visible) {
    uint8_t collectionIndex = startingItemIndex + i;
    if (collectionIndex < items.getSize()) {  //if item exist
      items[collectionIndex]->loadItem(elementNamePrefix, i);
      return;
    }
  }
  hideItem(i);
}
