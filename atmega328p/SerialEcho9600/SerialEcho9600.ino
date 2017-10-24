// SerialEcho9600
// 
// Test driver for SoftwareSerialRepeaterLCD to display serial input on LCD.
// 
// PIN 00 = Serial.TX --- PIN 11 SoftwareSerial.RX
// PIN 01 = Serial.RX --- PIN 10 SoftwareSerial.TX

void setup() {
  Serial.begin(9600);
  while(!Serial){;}
  Serial.println();
  Serial.println("SETUP");
  delay(1000);
}

void loop() {
  delay(1);
  if (Serial.available()) {
    Serial.write(Serial.read());
  }
}
