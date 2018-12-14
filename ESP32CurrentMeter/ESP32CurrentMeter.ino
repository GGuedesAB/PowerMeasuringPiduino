void setup() {
  Serial.begin(9600);
}

void loop() {
  int value1 = analogRead(A0);
  int value2 = analogRead(A1);
  String result = String (value1)+"|"+String(value2);
  Serial.println(result);
}
