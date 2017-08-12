#include "espuptime.h"

EspUptime uptime;

void setup() {
  // put your setup code here, to run once:

}

int g_uptime = 0;

void loop() {
  uptime.update();
  g_uptime = uptime.seconds();
  delay(100);
}
