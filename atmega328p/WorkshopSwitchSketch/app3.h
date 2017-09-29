// app3.h - Flash on serial Debug

#include <avr/pgmspace.h>

void setup_APP3() {
  Serial.begin(9600);
  while (!Serial) {;}
  Serial.println(F("app3"));
}

void loop_APP3() {
  digitalWrite(APP2_LED_PIN, HIGH);
  delay(100);
  Serial.print(F("loop "));
  Serial.println(millis());
  digitalWrite(APP2_LED_PIN, LOW);
  delay(900);  
}

