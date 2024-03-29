//setup and loop funcs



//startup configs===================================================================================================================
class StartUpSettings : public IConfigurable {
protected:
  uint8_t flags;
public:
  enum class FirstActiveScreen : uint8_t {
    DemoControl,
    MainMenu,
    Solving,
    Bios,
    Settings,
    Diagnostics
  } firstActiveScreen;  //Which screen to start from

  virtual const uint8_t numberOfConfigs() const override {
    return 2;
  }  //how many
  virtual Config getConfig(uint8_t index) const override {
    switch (index) {
      case 0: return Config(&flags, sizeof(uint8_t), F("flags"), Config::Type::Bool);
      default: return Config(&firstActiveScreen, sizeof(StartUpSettings::FirstActiveScreen), F("scrn"), Config::Type::Enum);
    }
  }  //get by index

  StartUpSettings(bool showBiosAtBeggining, bool openBiosAutomatically, bool loadAllSettings, StartUpSettings::FirstActiveScreen firstActiveScreen) {
    setShowBiosAtBeggining(showBiosAtBeggining);      //let user enter bios before everything
    setOpenBiosAutomatically(openBiosAutomatically);  //load all settings from EEPROM after bios
    setLoadAllSettings(loadAllSettings);              //load all settings from EEPROM after bios
    this->firstActiveScreen = firstActiveScreen;
  }
  bool showBiosAtBeggining() const {  //get get get get get get get get get
    return BitCoding::readBit(flags, 0);
  }
  bool openBiosAutomatically() const {
    return BitCoding::readBit(flags, 1);
  }
  bool loadAllSettings() const {
    return BitCoding::readBit(flags, 2);
  }
  void setShowBiosAtBeggining(bool value) {  //set set set set set set set set set
    BitCoding::writeBit(flags, value, 0);
  }
  void setOpenBiosAutomatically(bool value) {
    BitCoding::writeBit(flags, value, 1);
  }
  void setLoadAllSettings(bool value) {
    BitCoding::writeBit(flags, value, 2);
  }
  //showBiosAtBeggining, openBiosAutomatically,  loadAllSettings, firstActiveScreen
} startup(true, true, false, StartUpSettings::FirstActiveScreen::MainMenu);  //startup settings

//SETUP//SETUP//SETUP//SETUP//SETUP//SETUP//SETUP//SETUP//SETUP//SETUP//SETUP//SETUP//SETUP//SETUP//SETUP//SETUP//SETUP//SETUP//SETUP
//#define SaveAllSettings //run once and set all configs with program values
#define PCDEBUG
void setup() {
  Serial.begin(PCBAUD);  //begin pc port
#ifdef PCDEBUG
  while (!Serial)
    ;
#endif
  Serial.println(F("Start"));
  pcm.setCommandSet(pcSet());  //set pc commandSet

  reg.addObject(&startup, F("start"));  //add configurable objects to register
  reg.addObject(&rm.right, F("right"));
  reg.addObject(&rm.left, F("left"));
  reg.addObject(&rm, F("rm"));


#ifndef SaveAllSettings           //in SAS build don't initialize NX port and interface
  Serial1.begin(NXBAUD);          //begin nx port
  cm2.setCommandSet(startSet());  //set screen awakening detection

  //reg.getConfObject(F("start")).loadAll();  //load startup settings
  //Bios stage
  if (!startup.showBiosAtBeggining()) goto skipBios;
  if (!startup.openBiosAutomatically())                //If open bios not automatically, ask
    if (!showDialogue(new BiosInvite)) goto skipBios;  //if timeout skip
  showDialogue(new Bios);
skipBios:
  Serial.print(F("Bios skipped"));
  //end of bios stage, next loading configs
  if (startup.loadAllSettings()) reg.loadAllConfigs();

  rm.initializeSettings();
#else
  reg.saveAllConfigs();
  Serial.print(F("saving complete"));
#endif  //!SaveAllSettings
#ifndef SaveAllSettings
  rm.initializeHardware();
  rm.allignBoth();
  rm.open();
  RobotState::setActive(*new FestControl);
#endif  //!SaveAllSettings
  Serial.println(F("end"));
}

void loop() {
#ifdef SaveAllSettings
  pcm.update();  //pc debug
#else
  fullSystemUpdate();  //normal
#endif  //SaveAllSettings
}