//setup and loop funcs

//define this MACROS in  4preSetup.ino :
// #define SaveAllSettings //run once and set all configs with program values
// #define PCDEBUG      //wait for pc serial connection before start

void setup() {
  systemInitDemo();
}

void loop() {
#ifdef SaveAllSettings
  pcm.update();  //pc debug
#else
  fullSystemUpdate();  //normal
#endif  //SaveAllSettings
}