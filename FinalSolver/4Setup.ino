//setup and loop funcs

void setup() {
  reg.addObject(&rm.left, F("left"));
  reg.addObject(&rm.right, F("right"));
  reg.addObject(&rm, F("rm"));
  reg.getSetting(F("reg/autoLd")).load();
  if (reg.loadAll) reg.loadAllConfigs();

  Serial.begin(PCBAUD);
  Serial1.begin(NXBAUD);
  Serial.println(F("Start"));
  // while (!Serial)
  //   ;
  // while (!Serial1)
  //   ;
  //Serial conections initialization
  // Orientation test = 0;
  // OrtAng angle(1);
  // test.print();
  // Serial.print("\t");
  // Axis ax= Axis::Direction::nY;
  // ax.printLetter();
  // Serial.print(int8_t(angle));
  // Serial.print("->");
  // test *= Rotation(ax, angle);  //rotate cube
  // test.print();
  // Serial.println();

  cm2.setCommandSet(startSet);

  pcm.setCommandSet(pcSet);
  pcm.printComs();

  RobotState::setActive(festControl);

  rm.initialize();
  rm.allignBoth();
  rm.open();
  Serial.println(F("end"));
}

void loop() {
  // delay(100);
  // setLed(f(led));
  fullSystemUpdate();
}