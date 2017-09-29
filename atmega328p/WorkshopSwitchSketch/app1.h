// app1.h - Blink 1000 ms

#define APP1_LED_PIN  13

void setup_APP1() {
  pinMode(APP1_LED_PIN, OUTPUT);
}

void loop_APP1() {
  digitalWrite(APP1_LED_PIN, HIGH);
  delay(1000);
  digitalWrite(APP1_LED_PIN, LOW);
  delay(1000);
}

