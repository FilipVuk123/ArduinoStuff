

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("In setup");
  pinMode(LEDWS_BUILTIN, OUTPUT);
}

void loop() {
  Serial.println("In loop");
  digitalWrite(LEDWS_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LEDWS_BUILTIN, LOW);
  delay(500);
}
