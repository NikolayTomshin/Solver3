void setScreen(NextionScreen& ns) {
  ns.setActive(cm);
}
char showDialogue(DialogueScreen* ds) {
  return ds->show(cm);
}
char Config::mysteryDialogueCall() {
  return (showDialogue(new ShortDialogue(ShortDialogue::Type::Warning,
                                         ShortDialogue::Options::YesNo,
                                         F("Вы хотели ввести \"0\"?")))
          == 'Y');
}
template<class T> T* activeState() {
  return (T*)RobotState::getActiveState();
}

//Implementation of command voids


void startStart() {
  NextionScreen::reloadActive();
  Serial.print("reloaded");
}

//scannerServo
void scannerServoGoD() {
  Scanner& sc = rm.scanner;
  uint8_t i = CommandSet::args[0];
  if (i == 0xFF) {
    DialogueScreen::endDialogue(0);
    return;
  }
  sc.goAngle(sc.servoAngles[i] = CommandSet::args[1]);
}
//demo
void fym() {
  Serial.println(F("fym"));
  rm.RAction(true, 1);
}
void fyp() {
  Serial.println(F("fyp"));
  rm.RAction(true, -1);
}
void fzm() {
  Serial.println(F("fzm"));
  rm.LAction(true, -1);
}
void fzp() {
  Serial.println(F("fzp"));
  rm.LAction(true, 1);
}
void frc() {
  Serial.println(F("frc"));
  rm.RAction(false, -1);
}
void fr() {
  Serial.println(F("fr"));
  rm.RAction(false, 1);
}
void fdc() {
  Serial.println(F("fdc"));
  rm.LAction(false, -1);
}
void fd() {
  Serial.println(F("fd"));
  rm.LAction(false, 1);
}
void fg() {
  Serial.println(F("fg"));
  if (rm.isHolding()) {
    if (showDialogue(new EE))
      proceedRet();
  } else {
    scrambleCube(showDialogue(new BE));
  }
}

void fes() {
  showDialogue(new SES);
}
//beg
void beginCubeControlD() {
#ifdef DIALOGUEdebug
  Serial.println(F("DIALOGUE BEGIN"));
#endif
  DialogueScreen::endDialogue(CommandSet::args[0]);
}
void scrambleCube(uint8_t moves) {
  activeState<FestControl>()->scrambleCube(moves);
}
void FestControl::scrambleCube(uint8_t moves) {  //после начала
  co.updateTopMessage(F("Захватываю..."));
  ops.clear();
  rm.grab();    /// Что делать?
  if (moves) {  //если нужно перемешать
    co.updateTopMessage(F("Перемешиваю..."));
    rm.setSmooth(false);
    rm.scramble(moves, millis());
  }
  rm.setSmooth(true);
  co.updateTopMessage(F(" Куб захвачен\r\n  Что делать?"));
  co.updateControl(true);
}
//ret
void dialogueCancel() {
#ifdef DIALOGUEdebug
  Serial.println(F("DIALOGUE CANCEL"));
#endif
  DialogueScreen::endDialogue(0);
}
void dialogueYes() {
#ifdef DIALOGUEdebug
  Serial.println(F("DIALOGUE YES"));
#endif
  DialogueScreen::endDialogue('Y');
}

void proceedRet() {
  activeState<FestControl>()->proceedRet();
}
void FestControl::proceedRet() {  //собрать и отпустить
  ops.print();                    //debug log
  rm.setSmooth(false);            //legacy control feature (not gone yet)
  co.updateTopMessage(F("Делаю обратно..."));
  rm.playBack();
  co.updateTopMessage(F("Отпускаю..."));
  rm.letGo();  //   кубик
  co.updateTopMessage(F("Можно брать и\r\n класть кубик"));
  co.updateControl(false);
}

//pc
bool led = false;

void pcTest() {
  Serial.print(led);
  Serial.print(F(" Toggled!"));
  // setLed(f(led));
  f(led);

  rm.syncGrab(led ? 1 : 2);
  Serial.println();
  delay(200);
}