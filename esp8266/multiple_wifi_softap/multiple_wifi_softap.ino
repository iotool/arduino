// https://github.com/iotool/arduino/esp8266/multiple_wifi_softap

#ifdef ESP8266
extern "C" {
#include "c_types.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "Esp.h"
}
#endif

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer webserver ( 80 );
struct softap_config softapConfig;
boolean softapStart;

void setup() {
  Serial.begin(9600);
  // --- clear
  WiFi.persistent(false);         // dont erase flash
  WiFi.softAPdisconnect(true);    // disconnect client
  WiFi.enableAP(false);           // close accesspoint
  WiFi.mode(WIFI_OFF);            // clear opmode
  WiFi.onEvent(WiFiEvent);
  // --- softap
  WiFi.mode(WIFI_AP);
  boolean softapStart = WiFi.softAP("ESP-Hotspot");
  wifi_softap_get_config(&softapConfig);
  softapConfig.max_connection = 4;
  softapConfig.beacon_interval = 100;  
  ETS_UART_INTR_DISABLE();
  wifi_softap_set_config_current(&softapConfig);
  ETS_UART_INTR_ENABLE();
  // --- webserver
  webserver.on ( "/", handleRoot);
  webserver.onNotFound ([]() {
    webserver.send ( 200, "text/plain", "NotFound!" );
  });
  webserver.begin();
}

byte softAp = 0;
unsigned int loops = 0;

void loop ( void ) {
  // --- webserver
  webserver.handleClient();
  delay(1);
  // --- flip/flop softAp
  loops++;
  if (loops % 1000 == 0) {
    // --- softAp[0]: "ESP-Config"
    wifi_softap_get_config(&softapConfig);
    os_memset(softapConfig.ssid, 0, 32);
    softAp = 1;
    char ssid[] = "ESP-Config";
    char password[] = "12345678";
    strcpy(reinterpret_cast<char*>(softapConfig.ssid), ssid);
    strcpy(reinterpret_cast<char*>(softapConfig.password), password);
    softapConfig.ssid_len = strlen(ssid);
    softapConfig.authmode = AUTH_WPA_WPA2_PSK;
    ETS_UART_INTR_DISABLE();
    wifi_softap_set_config_current(&softapConfig);
    ETS_UART_INTR_ENABLE();
  }
  if (loops % 1000 == 500) {
    // --- softAp[1]: "ESP-Hotspot"
    wifi_softap_get_config(&softapConfig);
    os_memset(softapConfig.ssid, 0, 32);
    softAp = 0;
    char ssid[] = "ESP-Hotspot";
    strcpy(reinterpret_cast<char*>(softapConfig.ssid), ssid);
    os_memset(softapConfig.password, 0, 64);
    softapConfig.ssid_len = strlen(ssid);
    softapConfig.authmode = AUTH_OPEN;
    ETS_UART_INTR_DISABLE();
    wifi_softap_set_config_current(&softapConfig);
    ETS_UART_INTR_ENABLE();
  }
}

void handleRoot() { 
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  char html[800];
  switch(softAp) {
    case 1:
      snprintf ( html, 799,
"<html>\
 <head>\
  <title>ESP-AP-0</title>\
 </head>\
 <body>\
  <h1>ESP-AP-1</h1>\
  <p>Uptime: %02d:%02d:%02d</p>\
 </body>\
</html>",hr, min % 60, sec % 60
    );
    break;
    default:
      snprintf ( html, 799,
"<html>\
 <head>\
  <title>ESP-AP-0</title>\
 </head>\
 <body>\
  <h1>ESP-AP-0</h1>\
  <p>Uptime: %02d:%02d:%02d</p>\
 </body>\
</html>",
    hr, min % 60, sec % 60
      );
      break;
  }
  webserver.send ( 200, "text/html", html );
}

void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d / ap: %d\n", event,softAp);
  switch(event) {
    case WIFI_EVENT_SOFTAPMODE_STACONNECTED:
        Serial.println("WIFI_EVENT_SOFTAPMODE_STACONNECTED");
        break;
    case WIFI_EVENT_SOFTAPMODE_STADISCONNECTED:
        Serial.println("WIFI_EVENT_SOFTAPMODE_STADISCONNECTED");
        break;
    case WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED:
        Serial.println("WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED");
        break;
  }
}

