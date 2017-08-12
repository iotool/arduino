# Fix 72 minute overflow

* millis() produce a overflow every 72 minutes for ESP8266
* timeout <= millis don't work for hours or days

## EspUptime Library

* call uptime.update() at loop
* read uptime.seconds instead of millis()
