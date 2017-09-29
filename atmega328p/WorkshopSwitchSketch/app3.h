// app3.h

#include <avr/pgmspace.h>

void setup_APP3() {
  Serial.begin(9600);
  while (!Serial) {;}
  Serial.println(F("app3"));
}

void loop_APP3() {
  Serial.print(F("loop "));
  Serial.println(millis());
  delay(1000);
}

