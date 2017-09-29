// app2.h

#define APP2_LED_PIN  13
#define APP2_MILLIS_BLINK_INTERVAL 0

void setup_APP2() {
  pinMode(APP2_LED_PIN, OUTPUT);
  g_appStaticUint32[APP2_MILLIS_BLINK_INTERVAL] = 0;
}

void loop_APP2() {
  unsigned long uptime = millis();
  if (uptime - g_appStaticUint32[APP2_MILLIS_BLINK_INTERVAL] > 1000) {
    g_appStaticUint32[APP2_MILLIS_BLINK_INTERVAL] = uptime;
    if (digitalRead(APP2_LED_PIN) == HIGH) {
      digitalWrite(APP2_LED_PIN, LOW);
    } else {
      digitalWrite(APP2_LED_PIN, HIGH);
    }
  }
}

