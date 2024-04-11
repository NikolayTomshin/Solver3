#include "Arduino.h"
#include <stdint.h>
#include <math.h>
#include "HardwareSerial.h"
#include <avr/pgmspace.h>
#include "SCom.h"


uint8_t CommandSet::args[3] = { 0 };
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
void CommandSet::printComs() const {
  Serial.write('{');
  for (uint8_t i = 0; i < size; ++i) {      //foreach command in set
                                            //check if letters in buffer are valid as command
    const char* cPtr = getMaskPtr(i);       //get pointer to first char of mask
    for (uint8_t j = 0; j < length; j++) {  //foreach character of buffer
      //compare with mask char
      char theChar = pgm_read_byte_near(cPtr + j);
      if (theChar == '\0') Serial.print(F("\\0"));
      else
        Serial.write(theChar);  //get mask char
    }
    Serial.write(' ');
  }
  Serial.write('}');
}
void ComBuffer::initialize(uint8_t comSize) {
#ifdef SCOMdebug
  Serial.print(F("new command length "));
  Serial.println(comSize);
  delay(10);
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
        Serial.write(buffer[k]);
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
  if (this->commandSet != NULL) delete this->commandSet;
  this->commandSet = new CommandSet(commandSet);
  this->actions = commandSet.getFuncs();
#ifdef SCOMdebug
  Serial.println(F("new commandSet"));
  commandSet.printComs();
#endif
  buffer.initialize(commandSet.getLength());
}
void CommandListener::printComs() const {
  Serial.print(F("ComSet size="));
  Serial.print(commandSet->size);
  Serial.print(F(" length="));
  Serial.println(commandSet->length);
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
CommandListener::~CommandListener() {
  if (commandSet != NULL) delete commandSet;
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

String letterIndex(const String& letter, uint8_t index) {
  return (letter + String(index));
}

void loadSlider(uint8_t number, uint8_t value) {
  const String name = letterIndex(F("h"), number);
  loadVal(name, value);
  click(name, false);
}
//Show or hide element on Nextion screen
void setVisibility(const String& objName, bool visible) {
#ifdef SCOMdebug
  Serial.print(objName);
  Serial.print(F(" VIS "));
  Serial.println(visible);
  delay(10);
#endif
  callFunction(F("vis"), objName);
  addParametre(String(visible));
  comEnd();
}

//Nextion

NextionScreen* NextionScreen::currentScreen;

NextionScreen* NextionScreen::getActiveScreen() {
  return currentScreen;
}
void NextionScreen::setActive(PortListener& port) {
  if (currentScreen != NULL) currentScreen->isActive = false;
  currentScreen = this;
  isActive = true;
  port.setCommandSet(getCommandSet());
#ifdef SCOMdebug
  Serial.println(F("SUCC"));
  delay(10);
#endif;
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
Stack<DialogueScreen*> DialogueScreen::screenStack;
char DialogueScreen::arg;

bool DialogueScreen::ready() const {
  return _ready;
}
char DialogueScreen::show(PortListener& port) {
#ifdef DIALOGUEdebug
  Serial.println(F("New dialogue"));
  delay(10);
#endif
  bool firstDialogue = screenStack.getSize() == 0;
  if (firstDialogue) fallBackScreen = getActiveScreen();
  screenStack.push(this);
  setActive(port);
#ifdef DIALOGUEdebug
  Serial.println(F("nd active"));
  delay(10);
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
PageScreen::~PageScreen() {
  delete pageControl;
}
void PageScreen::PageControl::print() const {
  Serial.print(currentPage);
  Serial.print(F("/"));
  Serial.print(numberOfPages);
  Serial.println(F(" страница"));
}
void PageScreen::PageControl::load() {
#ifdef SCOMdebug
  Serial.println("PageControl::load() ex");
  delay(50);
#endif
  setVisibility(elementNamePrefix + "L", showLButton);
  setVisibility(elementNamePrefix + "R", showRButton);
#ifdef SCOMdebug
  Serial.println("PC set button vis");
  delay(10);
#endif
  loadTxt(elementNamePrefix + "S",
          showText ? pageFormatBefore + String(currentPage) + pageFormatSeparator + String(numberOfPages) + pageFormatAfter
                   : String(""));
#ifdef SCOMdebug
  Serial.println("PC loaded TXT");
  delay(10);
#endif
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
  setVisibility(elementNamePrefix + (next ? F("R") : F("L")), enabled);
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
      pageScreen->onPrevPage();
      return;
    case 'N':
      nextPage();
      pageScreen->onNextPage();
      return;
    case 'S':
      pageScreen->onSelectPage();
      return;
    default:;
  }
}
uint8_t PageScreen::PageControl::getCurrentPage() const {
  return currentPage;
}
uint8_t PageScreen::PageControl::getNumberOfPages() const {
  return numberOfPages;
}

TitledScreen::TitledScreen(const String& title) {
  this->title = title;
}
void TitledScreen::loadTitle() const {
  loadTxt(F("ttl"), title);
}
void TitledScreen::loadTitle(const String& title) {
  this->title = title;
  loadTitle();
}

void ExitableScreen::updateExitComponents() const {
  setVisibility(F("ext"), exitAllowed);
}
void ExitableScreen::setExitAllowed(bool allowed) {
  exitAllowed = allowed;
  updateExitComponents();
}

CollectionScreen::~CollectionScreen() {
  delete collectionControl;
}
CollectionScreen::CollectionControl::
  CollectionControl(const String& collectionPrefix, uint8_t sizeOfCollection,
                    uint8_t startingItemIndex, uint8_t numberOfVisibleItems, itemHider hideItem)
  : NextionScreen::Element::Element(collectionPrefix) {
  this->hideItem = hideItem;
  this->startingItemIndex = startingItemIndex;
  this->numberOfVisibleItems = numberOfVisibleItems;
  items = Array<Item*>(sizeOfCollection);
}
CollectionScreen::CollectionControl::Item*& CollectionScreen::CollectionControl::operator[](uint8_t i) {  //access items
  return items[i];
}
uint8_t CollectionScreen::CollectionControl::getNumberOfVisibleItems() const {
  return numberOfVisibleItems;
}
void CollectionScreen::CollectionControl::loadFromIndex(uint8_t startingItemIndex) {
  this->startingItemIndex = startingItemIndex;
  load();
}
void CollectionScreen::CollectionControl::shiftLoadIndex(int8_t startingItemIndexShift) {
  startingItemIndex += startingItemIndexShift;
  load();
}
void CollectionScreen::CollectionControl::load() {
  uint8_t collectionIndex = startingItemIndex;
  //limitIndex = collectionIndex + min(numberOfVisibleItems, restOfItemsInCollection)
  uint8_t limitIndex = collectionIndex + minMax<uint8_t>(numberOfVisibleItems, items.getSize() - collectionIndex, false);
  uint8_t i = 0;
  if (collectionIndex < items.getSize())  //protection from too large startingItemIndex
    while (collectionIndex < limitIndex) {
      items[collectionIndex]->loadItem(elementNamePrefix, i);
      ++collectionIndex;
      ++i;
    }
  for (; i < numberOfVisibleItems; ++i)
    (*hideItem)(elementNamePrefix, i);
}
uint8_t CollectionScreen::CollectionControl::upperLimitOfAccess() const {
  const uint8_t& s = items.getSize();
  return (startingItemIndex < s) ? limits<uint8_t>(numberOfVisibleItems, s - startingItemIndex, true) : 0;
}
void CollectionScreen::CollectionControl::inputEvent(const char args[]) {
  const uint8_t& i = *args;
  if (i < upperLimitOfAccess())
    items[i + startingItemIndex]->inputEvent(&args[1]);
}
void CollectionScreen::hideElement(const String& collectionPrefix, uint8_t i) {
  setVisibility(letterIndex(collectionPrefix, i), false);
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
  (*hideItem)(elementNamePrefix, i);
}

uint8_t NextionScreen::TextField::Font::lengthOfLine(const uint16_t& pixels) {
  return uint8_t(pixels / (charWidthAverageDpx * 10.0));
}
void NextionScreen::TextField::setString(const String& string) {
  currentText = string;
}
NextionScreen::TextField::TextField(const String& textFieldNamePrefix, uint8_t fontID, uint16_t widthInPixels,
                                    NextionScreen::TextField::LoadMode loadMode)
  : Element(textFieldNamePrefix) {
  this->fontID = fontID;
  this->widthInPixels = widthInPixels;
  this->loadMode = loadMode;
}
void NextionScreen::TextField::load() {
  switch (loadMode) {
    case LoadMode::Line: return loadTxt(elementNamePrefix, currentText);
    case LoadMode::Multiline: return loadMultiline();
    case LoadMode::MultilineCentered: return loadMultilineCentered();
    default: return;
  }
}
void NextionScreen::TextField::loadString(const String& string) {
  currentText = string;
  load();
}
void NextionScreen::TextField::loadFit(const String& string) {
  const uint16_t& length = string.length();
  for (uint8_t i = 0; i < Font::numberOfFonts(); i++)
    if (Font::screenFontsSizeDesc(i).lengthOfLine(widthInPixels) >= length) {
      fontID = i;
      break;
    }
  loadString(string);
}
const NextionScreen::TextField::Font& NextionScreen::TextField::getFont() const {
  return Font::screenFonts(fontID);
}
void NextionScreen::TextField::loadMultiline() {
  comEnd();
  Serial1.print(elementNamePrefix);
  Serial1.print(F(".txt=\""));
  const uint8_t lineSize = getFont().lengthOfLine(widthInPixels);

  for (uint16_t lineCursor = 0; lineCursor < currentText.length();) {  //while line cursor not reached the end
    lineCursor = indexOfSkipping(currentText, lineCursor, F(" "));     //jump over empty spaces
    uint16_t lineLimit = endOfTheLine(currentText, lineSize, lineCursor, F(" "));

    for (; lineCursor < lineLimit; ++lineCursor)  //print chars of line
      Serial1.write(currentText[lineCursor]);
    Serial1.print(F("\r\n"));  //print new line
  }                            //all chars of line are sent
  Serial1.write('"');
  comEnd();  //end command
}
void NextionScreen::TextField::loadMultilineCentered() {
  comEnd();
  Serial1.print(elementNamePrefix);
  Serial1.print(F(".txt=\""));
  const uint8_t lineSize = getFont().lengthOfLine(widthInPixels);

  uint8_t maxLineLength = 0;
  for (uint16_t lineCursor = 0; lineCursor < currentText.length();) {
    lineCursor = indexOfSkipping(currentText, lineCursor, F(" "));  //jump over empty spaces
    uint16_t lineLimit = endOfTheLine(currentText, lineSize, lineCursor, F(" "));
    uint8_t lineLength = uint8_t(lineLimit - lineCursor + 1);

    if (lineLength > maxLineLength) maxLineLength = lineLength;
    lineCursor = lineLimit;
  }                                                   //find longest line
  const uint8_t& centeredLineLength = maxLineLength;  //maxLineLength=width of centered paragraph

  for (uint16_t lineCursor = 0; lineCursor < currentText.length();) {
    lineCursor = indexOfSkipping(currentText, lineCursor, F(" "));  //jump over empty spaces
    uint16_t lineLimit = endOfTheLine(currentText, lineSize, lineCursor, F(" "));
    uint8_t lineLength = uint8_t(lineLimit - lineCursor + 1);

    uint8_t indent = (centeredLineLength - lineLength) / 2;
    for (uint8_t i = 0; i < indent; ++i)  //print indent
      Serial1.write(' ');
    for (; lineCursor < lineLimit; ++lineCursor)  //print chars of line
      Serial1.write(currentText[lineCursor]);
    Serial1.print(F("\r\n"));  //print new line
  }

  Serial1.write('"');
  comEnd();  //end command
}
void ListScreen::onNextPage() {
  collectionControl->shiftLoadIndex(collectionControl->getNumberOfVisibleItems());
}
void ListScreen::onPrevPage() {
  collectionControl->shiftLoadIndex(-collectionControl->getNumberOfVisibleItems());
}
void ListScreen::onSelectPage() {
  collectionControl->loadFromIndex(collectionControl->getNumberOfVisibleItems() * (pageControl->getCurrentPage() - 1));
}

ShortDialogue::~ShortDialogue() {
  delete message;
}

ShortDialogue::ShortDialogue(Type type, Options options, const String& message)
  : TitledScreen(title) {
  String title;
  switch (type) {
    case Type::Notice: title = F("Оповещение"); break;
    default: title = F("Внимание"); break;
    case Type::Error: title = F("Ошибка"); break;
  }
  char l = '\0';
  char m = '\0';
  char r = '\0';
  String messageL = F("");
  String messageM = F("");
  String messageR = F("");
  switch (options) {
    default:
      m = 'O';
      messageM = F("Ок");
      break;
    case Options::YesNoCancel:
      m = 'C';
      messageM = F("Отмена");
    case Options::YesNo:
      l = 'Y';
      messageM = F("Да");
      r = 'N';
      messageM = F("Нет");
      break;
  }
  initialize(message, l, messageL, m, messageM, r, messageR);
}

ShortDialogue::ShortDialogue(const String& title, const String& messageString,
                             char l, const String& messageL,
                             char m, const String& messageM,
                             char r, const String& messageR)
  : TitledScreen(title) {
  initialize(messageString, l, messageL, m, messageM, r, messageR);
}
void ShortDialogue::initialize(const String& messageString,
                               char l, const String& messageL,
                               char m, const String& messageM,
                               char r, const String& messageR) {
  this->message = new TextField(F("t0"), 0, 320, TextField::LoadMode::Multiline);
  this->message->setString(messageString);

  this->l = messageL;
  this->m = messageM;
  this->r = messageR;
  answers[0] = l;
  answers[1] = m;
  answers[2] = r;
}
void ShortDialogue::loadScreen() {
  goNextionPage(F("Short"));
  loadTitle();
  message->load();
  for (uint8_t i = 0; i < 3; ++i) {
    if (answers[i] == '\0') setVisibility(String(letterOf(i)), false);
    else loadTxt(String(letterOf(i)), stringOf(i));
  }
}
const String& ShortDialogue::stringOf(uint8_t i) const {
  switch (i) {
    case 0: return l;
    case 1: return m;
    default: return r;
  }
}
char ShortDialogue::letterOf(uint8_t i) {
  switch (i) {
    case 0: return 'L';
    case 1: return 'M';
    default: return 'R';
  }
}
void ShortDialogue::inputEvent() {
  char answer = CommandSet::args[0];
  switch (answer) {
    case 'Y':
    case 'C':
    case 'N':
      endDialogue(answer);
    default:;
  }
}
ShortDialogue* ShortDialogue::notice(const String& message) {
  return new ShortDialogue(Type::Notice, Options::Proceed, message);
}
ShortDialogue* ShortDialogue::yn(const String& message) {
  return new ShortDialogue(Type::Warning, Options::YesNo, message);
}
ShortDialogue* ShortDialogue::ync(const String& message) {
  return new ShortDialogue(Type::Warning, Options::YesNoCancel, message);
}
ShortDialogue* ShortDialogue::err(const String& message) {
  return new ShortDialogue(Type::Error, Options::Proceed, message);
}