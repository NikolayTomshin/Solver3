class

void repeat() {
  if (Serial1.available()) {
    while (Serial1.available()) Serial.println(Serial1.readString());
  }

  if (Serial.available()) {
    while (Serial.available()) Serial.println(Serial.readString());
    Serial.println("end");
  }
}

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial1.setTimeout(3);
  Serial.setTimeout(3);
}

void loop() {
  // put your main code here, to run repeatedly:
  repeat();
}
