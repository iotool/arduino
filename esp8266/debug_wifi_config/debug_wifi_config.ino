// https://github.com/iotool/arduino/esp8266/debug_wifi_config/debug_wifi_config.ino

#ifdef ESP8266
extern "C" {
  #include "user_interface.h"
}
#endif

void setup() {
  Serial.begin(9600);
#ifdef ESP8266
  struct softap_config wifiSoftapConfig;
  struct station_config wifiStationConfig;
  wifi_softap_get_config(&wifiSoftapConfig); 
  wifi_station_get_config(&wifiStationConfig); 
  String wifiSoftapConfig_ssid = (char*)wifiSoftapConfig.ssid;
  String wifiSoftapConfig_password = (char*)wifiSoftapConfig.password;
  String wifiStationConfig_ssid = (char*)wifiStationConfig.ssid;
  String wifiStationConfig_password = (char*)wifiStationConfig.password;
  if (wifiSoftapConfig_ssid.length() > 0) {
    Serial.println("Wifi AccessPoint = enabled");
  } else {
    Serial.println("Wifi AccessPoint = disabled");
  }
  Serial.print("wifiSoftapConfig.ssid[32]: "); Serial.println(wifiSoftapConfig_ssid);
  Serial.print("wifiSoftapConfig.password[64]: "); Serial.println(wifiSoftapConfig_password);
  if (wifiStationConfig_ssid.length() > 0) {
    Serial.println("Wifi Station = enabled");
  } else {
    Serial.println("Wifi Station = disabled");
  }
  Serial.print("wifiStationConfig.ssid[32]: "); Serial.println(wifiStationConfig_ssid);
  Serial.print("wifiStationConfig.password[64]: "); Serial.println(wifiStationConfig_password);
#endif
}

void loop() {
}