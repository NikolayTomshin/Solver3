

//FestControl
void FestControl::activate() {
  co.updateTopMessage(F("Можно брать и\r\n класть кубик"));
  setScreen(co);
  rm.setHistorySave(&ops);
}
void FestControl::disable() {
  rm.setHistorySave();
  delete this;
}

//Fest Control screens
void CO::loadScreen() {
  goNextionPage(F("CO"));
  loadMessage();
  loadControl();
}

void CO::loadControl() {
  loadPic(F("p0"), controlEnabled ? 17 : 16);
}
void CO::loadMessage() {
  loadTxt(F("t1"), topMessage);
}
void CO::updateTopMessage(const String& message) {
  topMessage = message;
  if (isActive) loadMessage();
}
void CO::updateControl(bool enabled) {
  controlEnabled = enabled;
  if (isActive) loadControl();
}
const CommandSet& CO::getCommandSet() const {
  return demoSet();
};

void BE::loadScreen() {
  goNextionPage(F("BE"));
}
const CommandSet& BE::getCommandSet() const {
  return begSet();
};

void EE::loadScreen() {
  goNextionPage(F("EE"));
}
const CommandSet& EE::getCommandSet() const {
  return retSet();
};

void SES::loadScreen() {
  goNextionPage(F("SS"));
  for (uint8_t i = 0; i < 4; ++i) {
    String s = F("h ");
    s[1] = digitOf(i);
    loadVal(s, rm.scanner.servoAngles[i]);
    s[0] = 'n';
    loadVal(s, rm.scanner.servoAngles[i]);
  }
}
const CommandSet& SES::getCommandSet() const {
  return scannerServoSetD();
};
