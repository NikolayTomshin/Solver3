const NextionScreen::TextField::Font& NextionScreen::TextField::Font::screenFonts(uint8_t fontID) {
  switch (fontID) {
    case 0: return Font(24, 12.18, 1.2);
    case 1: return Font(32, 16.25, 1.2);
    default: return Font(16, 8.12, 1.2);
  }
}
const uint8_t NextionScreen::TextField::Font::numberOfFonts() {
  return 3;
}
const NextionScreen::TextField::Font& NextionScreen::TextField::Font::screenFontsSizeDesc(uint8_t index) {
  switch (index) {
    case 0: return screenFonts(1);
    case 1: return screenFonts(0);
    default: return screenFonts(2);
  }
}

//FestControl
void FestControl::activate() {
  co.updateTopMessage(Q("Можно брать и\r\n класть кубик"));
  setScreen(co);
  rm.setHistorySave(&ops);
}
void FestControl::disable() {
  rm.setHistorySave();
  delete this;
}

//Fest Control screens
void CO::loadScreen() {
  goNextionPage(Q("CO"));
  loadMessage();
  loadControl();
}

void CO::loadControl() {
  loadPic(Q("p0"), controlEnabled ? 17 : 16);
}
void CO::loadMessage() {
  loadTxt(Q("t1"), topMessage);
}
void CO::updateTopMessage(const StrRep& message) {
  topMessage = message;
  if (isActive) loadMessage();
}
void CO::updateControl(bool enabled) {
  controlEnabled = enabled;
  if (isActive) loadControl();
}
const CommandSet CO::getCommandSet() const {
  return demoSet();
};

void BE::loadScreen() {
  goNextionPage(Q("BE"));
}
const CommandSet BE::getCommandSet() const {
  return begSet();
};

void EE::loadScreen() {
  goNextionPage(Q("EE"));
}
const CommandSet EE::getCommandSet() const {
  return retSet();
};

void SES::loadScreen() {
  goNextionPage(Q("SS"));
  for (uint8_t i = 0; i < 4; ++i) {
    StrVal s(F("h "));
    s[1] = digitOf(i);
    loadVal(s, rm.scanner.servoAngles[i]);
    s[0] = 'n';
    loadVal(s, rm.scanner.servoAngles[i]);
  }
}
const CommandSet SES::getCommandSet() const {
  return scannerServoSetD();
};

const CommandSet SettingsScreen::getCommandSet() const {
  return settingsSet();
}
EditorScreen::~EditorScreen() {
}
bool EditorScreen::getBinary() const {
  return BitCoding::readBit(flags, 0);
}
bool EditorScreen::getInt() const {
  return BitCoding::readBit(flags, 1);
}
bool EditorScreen::getUint() const {
  return BitCoding::readBit(flags, 2);
}
bool EditorScreen::getInsert() const {
  return BitCoding::readBit(flags, 3);
}
bool EditorScreen::getBool() const {
  return BitCoding::readBit(flags, 4);
}
bool EditorScreen::getLocked() const {
  return BitCoding::readBit(flags, 5);
}
void EditorScreen::setBinary(bool value) {
  BitCoding::writeBit(flags, value, 0);
}
void EditorScreen::setInt(bool value) {
  BitCoding::writeBit(flags, value, 1);
}
void EditorScreen::setUint(bool value) {
  BitCoding::writeBit(flags, value, 2);
}
void EditorScreen::setInsert(bool value) {
  BitCoding::writeBit(flags, value, 3);
}
void EditorScreen::setBool(bool value) {
  BitCoding::writeBit(flags, value, 4);
}
void EditorScreen::setLocked(bool value) {
  BitCoding::writeBit(flags, value, 5);
}
void EditorScreen::setFlags(bool Binary, bool Int, bool Uint, bool Insert, bool Bool, bool Locked) {
  setBinary(Binary);
  setInt(Int);
  setUint(Uint);
  setInsert(Insert);
  setBool(Bool);
  setLocked(Locked);
}
void EditorScreen::updateString() const {
  comEnd();
  Q("va.txt=\"").print(Serial1);
  if (getBool() || getLocked())
    repString.print(Serial1);
  else {
    uint8_t i = 0;
    const uint8_t len = repString.strLen();
    for (; i < cursor; ++i)
      Serial1.write(repString[i]);
    if (getInsert()) {
      Serial1.write('[');
      if (cursor < len)
        Serial1.write(repString[i]);
      Serial1.write(']');
      ++i;
    } else Serial1.write('|');
    for (; i < len; ++i)
      Serial1.write(repString[i]);
  }

  Serial1.write('"');
  comEnd();  //end command
}
void EditorScreen::moveCursor(bool right) {
  if (right) {
    if ((cursor < repString.strLen()) && (cursor < cursorLimit))
      ++cursor;
    return;
  }
  if (cursor)
    --cursor;
}
void EditorScreen::enterChar(char c) {
  //insert doesn't add length exept when on last {[c]=c; if ++c} insert=overtype
  //!insert always adds length {string+=end, if ++c}            !insert=insert
  //cursor can't move on cursorlimit=max length of string
  if (repString.strLen() < cursorLimit)  //may add chars
  {
    if (getInsert()) {                     //insert at cursor
      if (cursor == repString.strLen()) {  //cursor at the end
        repString.append(C(c));
        goto moveCursor;
      }                                   //else  rewrite char at cursor
    } else {                              //add at cursor
      uint8_t n = repString.strLen();     //index of not existing character
      uint8_t p = n - 1;                  //index of last character
      repString.append(C(repString[p]));  //add last char to the end
      while (n > cursor) {                //push forward chars from the cursor
        repString[n] = repString[p];
        --n;
        --p;
      }  //after that rewrite char at cursor
    }
  }
  repString[cursor] = c;  //rewrite char at cursor
moveCursor:
  dirty = true;
  moveCursor(true);
}
void EditorScreen::eraseChar() {
  //insert erases this character
  //!insert erases previous character
  if (getBinary()) return;
  if (!repString.strLen()) return;
  if (!(cursor || getInsert())) return;
  uint8_t n = cursor + getInsert();  //index after erased char
  uint8_t p = n - 1;                 //index of erased char
  const uint8_t len = repString.strLen();
  while (n < len) {  //collapse all characters to the erased
    repString[p] = repString[n];
    ++n;
    ++p;
  }
  repString.truncate(repString.strLen() - 1);  //resize string
  moveCursor(false);
}
void EditorScreen::negate() {
  if (getBool()) {
    repString.fillWith(boolStr(!boolFromString(repString)));
    return;
  }
  if (getBinary()) {
    repString[cursor] = (repString[cursor] == '1') ? '0' : '1';
    return;
  }
  if (!getUint()) {  //float, int
    if (repString.indexOf(C('-')) >= 0)
      repString.replace(C('-'), StrEmpty());
    else repString.insertAt(C('-'), 0);
  }
}
void EditorScreen::putPoint() {
  if (flags & 0b00110111) return;  //not float
  uint16_t pointAt = repString.indexOf(C('.'));
  if (pointAt != -1) {
    repString.replace(C('.'), StrEmpty());
    if (pointAt < cursor) --cursor;
  }
  enterChar('.');
}
const CommandSet EditorScreen::getCommandSet() const {
  return editorSet();
}
EditorScreen::EditorScreen(Config* config, const String& title)
  : TitledScreen(title) {
  this->config = config;
  switch (config->getType()) {  //set flags for appropriate editing
    //                             (Binary, Int, Uint, Insert, Bool, Locked)
    case Config::Type::String: setFlags(false, false, true, false, false, false); break;
    case Config::Type::Bool: setFlags(true, false, false, true, true, false); break;
    case Config::Type::Float: setFlags(false, false, false, false, false, false); break;
    case Config::Type::Binary: setFlags(true, false, false, true, false, false); break;
    case Config::Type::Complicated: setFlags(false, false, false, false, false, true); break;
    case Config::Type::Enum: setFlags(false, true, false, false, false, true); break;
    case Config::Type::Uint: setFlags(false, true, true, false, false, false); break;
    case Config::Type::Int: setFlags(false, true, false, false, false, false); break;
    //                             (Binary, Int, Uint, Insert, Bool, Locked)
    default: setFlags(false, false, false, false, false, true);
  }
  repString.fillWith(config->toString());
  setInsert(getBinary());  //can't insert if fixed length in case of binary
  cursorLimit = getBinary() ? repString.strLen() : 25;
}
StrVal EditorScreen::elName(uint8_t i) {
  if (i < 10) return letterIndex(Q("b"), i).toVal();
  switch (i) {
    case 10: return F("err");
    case 11: return F("flo");
    case 12: return F("ins");
    case 13: return F("bL");
    case 14: return F("bR");
    case 15: return F("neg");
    default: return F("");
  }
}
void EditorScreen::loadScreen() {
  auto hide = [](uint8_t i) -> void {  //helper
    setVisibility(elName(i), false);
  };
  auto hideHide = [&hide](uint8_t i, uint8_t j) -> void {  //helper helper
    ++j;
    for (; i < j; ++i) hide(i);
  };
  goNextionPage(Q("NUM"));  //load page
  if (getLocked()) {
    hideHide(0, 15);  //hide all input buttons
    goto skipChecks;
  }
  if (getBinary()) {
    hideHide(2, 12);  //hide numpad except 0 1 and insert toggle
    if (getBool()) {
      hideHide(13, 14);  //hide cursor control
    }
    goto skipChecks;
  }
  if (getInt()) {
    hide(11);  //hide point
  }
  if (getUint())
    hide(15);  //hide negate
skipChecks:
  loadTitle();
  if (!dirty) setVisibility(Q("can"), false);
  updateString();
}
void EditorScreen::inputEvent() {
  char c = CommandSet::args[0];
  switch (c) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return enterChar(c);
    case '.':
      putPoint();
      return updateString();
    case 'I':
      setInsert(!getInsert());
      return updateString();
    case 'N':
      negate();
      return updateString();
    case 'L':
      moveCursor(false);
      return updateString();
    case 'R':
      moveCursor(true);
      return updateString();
    case 'E':
      eraseChar();
      return updateString();
    case 'O': return apply();
    case 'C': return cancel();
    default: return updateString();
  }
}
void EditorScreen::apply() {
  if (!dirty) return endDialogue('C');                                     //not dirty nothing to do
  if (showDialogue(ShortDialogue::yn(Q("Изменить параметр?"))) == 'Y')     //ask
    if (config->fromString(repString))                                     //try
      return endDialogue('A');                                             //applied
    else showDialogue(ShortDialogue::notice(Q("Неподходящее значение")));  //didn't work
}
void EditorScreen::cancel() {
  if (!dirty) return endDialogue('C');  //not dirty nothing to do
  repString.fillWith(config->toString());
  dirty = false;
}
SettingsScreen::SettingItem::SettingItem(Config* config, bool withPtr, const String& name) {
  this->config = config;
  this->withPtr = withPtr;
  this->name = name;
}
void SettingsScreen::SettingItem::loadItem(const String& collectionNamePrefix, uint8_t index) {
  loadTxt(letterIndex(collectionNamePrefix, index), name + F(" = ") + config->toString());
}
void SettingsScreen::SettingItem::inputEvent(const char args[]) {
  switch (args[0]) {
    case 'E': return edit();
    case 'S':
      if (withPtr) return ((ConfigWithPtr*)config)->save();
    case 'L':
      if (withPtr) return ((ConfigWithPtr*)config)->load();
    default:;
  }
}
void SettingsScreen::SettingItem::edit() {
  showDialogue(new EditorScreen(config, String(config->name)));
}

void SettingsScreen::loadScreen() {
#ifdef SETDebug
  Serial.println(F("Loading Settings"));
  delay(20);
#endif
  goNextionPage(Q("LTR"));
#ifdef SETDebug
  Serial.println(F("Starting loading pageControl"));
  delay(10);
  if (pageControl == NULL)
    Serial.print(F("НЕТ пэшконтрола"));
  else {
    Serial.print(F("Page control !=NUll"));
    pageControl->print();
  }
  delay(10);
#endif
  pageControl->load();
#ifdef SETDebug
  Serial.println(F("PC loaded"));
  delay(10);
#endif
  collectionControl->load();
#ifdef SETDebug
  Serial.print(F("CC loaded"));
  delay(10);
#endif
}
SettingsScreen::SettingsScreen(const Array<SettingItem*>& settingItems, const String& title, uint8_t numberOfVisibleItems)
  : TitledScreen(title) {
  const uint8_t& size = settingItems.getSize();
#ifdef SETDebug
  Serial.print(F("New setting screen "));
  Serial.print(size);
  Serial.print(F(" settings\n\rVisible items "));
  Serial.println(numberOfVisibleItems);
#endif  //SETDebug
  pageControl = new PageControl(PageControl::simplePC(F("pc"), size / numberOfVisibleItems + bool(size % numberOfVisibleItems), 1));
#ifdef SETDebug
  Serial.println(F("Page control+"));
  delay(10);
#endif  //SETDebug
  collectionControl = new CollectionControl(F("co"), size, 0, numberOfVisibleItems, &hideElement);
#ifdef SETDebug
  Serial.println(F("CollectionControl control+"));
  delay(10);
#endif  //SETDebug
  {
    uint8_t i = 0;
    for (auto settingItem = settingItems.iterator(); settingItem.notEnd(); ++settingItem) {
      (*collectionControl)[i] = *settingItem;
      ++i;
    }
  }
#ifdef SETDebug
  Serial.println(F("CollectionControl control set"));
  delay(10);
#endif  //SETDebug
#ifdef SETDebug
  Serial.println(F("Not crashed yet "));
  delay(20);
#endif  //SETDebug
}
void SettingsScreen::inputEvent() {
  switch (CommandSet::args[0]) {
    case 'P': return pageControl->inputEvent(&CommandSet::args[1]);
    case 'C': return collectionControl->inputEvent(&CommandSet::args[1]);
    case 'E': return onExit();
    default:;
  }
}
void SettingsScreen::onExit() {
  DialogueScreen::endDialogue(0);
}

const CommandSet ShortDialogue::getCommandSet() const {
  return shortDSet();
}

void Bios::loadScreen() {
  goNextionPage(Q("Bios"));
}
const CommandSet Bios::getCommandSet() const {
  return biosSet();
}
void Bios::showSettings() {
  showDialogue(new BiosSettings);
#ifdef SETDebug
  Serial.print(F("End of bios settings"));
  delay(10);
#endif
}
void Bios::inputEvent() {
  switch (CommandSet::args[0]) {
    case 'S': return showSettings();
    case 'C': return endDialogue(0);
    default:;
  }
}
void BiosInvite::loadScreen() {
  goNextionPage(Q("BiosIntro"));
}
const CommandSet BiosInvite::getCommandSet() const {
  return biosSet();
}
void BiosInvite::inputEvent() {
  switch (CommandSet::args[0]) {
    case 'E': return endDialogue(1);
    default: return endDialogue(0);
  }
}
BiosSettings::BiosSettings()
  : SettingsScreen() {
  const ConfigurableObject& startObj = reg.getConfObject(F("start"));  //get ConfObject of startup settings
#ifdef SETDebug
  Serial.print(F("Bios found startup object"));
#endif                                                  //SETDebug
  const uint8_t size = startObj.getNumberOfSettings();  //number of settings
#ifdef SETDebug
  Serial.print(F(" there is "));
  Serial.print(size);
  Serial.print(F(" settings\n\r"));
  delay(30);
#endif                                     //SETDebug
  Array<SettingItem*> settingItems(size);  //initialize array
  for (uint8_t i = 0; i < size; ++i) {
    const ConfigWithPtr& confW = startObj.getSetting(i);                //foreach setting
    settingItems[i] = new SettingItem(&confW, true, confW.toString());  //add setting item with EEPROM setting same name
  }
  *this = BiosSettings(settingItems, F("Параметры запуска"), 4);
}