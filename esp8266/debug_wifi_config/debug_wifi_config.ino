// https://github.com/iotool/arduino/esp8266/debug_wifi_config/debug_wifi_config.ino

#ifdef ESP8266
extern "C" {
  #include "user_interface.h"
  #include "Esp.h"
}
#include <ESP8266WiFi.h>
#endif

void clearWifiConfig() {
#ifdef ESP8266
  boolean l_resetConfig = false;
  switch(wifi_get_opmode()) {
    case STATION_MODE:
    case SOFTAP_MODE:
    case STATIONAP_MODE:
      l_resetConfig = true;
      break;
  }
  if (l_resetConfig) {
    // wifi access point
    struct softap_config l_wifiSoftapConfig;
    wifi_softap_get_config(&l_wifiSoftapConfig); 
    os_memset(l_wifiSoftapConfig.ssid, 0, 32);
    os_memset(l_wifiSoftapConfig.password, 0, 64);
    l_wifiSoftapConfig.authmode = AUTH_WPA_WPA2_PSK;
    l_wifiSoftapConfig.ssid_len = 0;
    l_wifiSoftapConfig.ssid_hidden = 0;
    l_wifiSoftapConfig.max_connection = 83;
    l_wifiSoftapConfig.beacon_interval = 500;
    // wifi station
    struct station_config l_wifiStationConfig;
    wifi_station_get_config(&l_wifiStationConfig);
    os_memset(l_wifiStationConfig.ssid, 0, 32);
    os_memset(l_wifiStationConfig.password, 0, 64);
    wifi_station_set_config(&l_wifiStationConfig);
    // wifi
    WiFi.persistent(true);
    WiFi.softAPdisconnect(false); 
    WiFi.enableAP(false); 
    WiFi.disconnect(); 
    WiFi.mode(WIFI_OFF);  
    Serial.print("Reset Wifi (");
    Serial.println(l_resetConfig);
    Serial.println(") ... Reboot");
    delay(5000);
    ESP.restart();
  }
#endif
}

void debugWifiConfig() {
#ifdef ESP8266
  boolean l_wifiSoftApEnabled = false;
  boolean l_wifiStationEnabled = false;
  switch(wifi_get_opmode()) {
    case STATION_MODE:
      l_wifiStationEnabled = true;
      Serial.println("Wifi Station = enabled");
      break;
    case SOFTAP_MODE:
      l_wifiSoftApEnabled = true;
      Serial.println("Wifi AccessPoint = enabled");
      break;
    case STATIONAP_MODE:
      l_wifiSoftApEnabled = true;
      l_wifiStationEnabled = true;
      Serial.println("Wifi Station = enabled");
      Serial.println("Wifi AccessPoit = enabled");
      break;
    default:
      Serial.println("Wifi Station = disabled");
      Serial.println("Wifi AccessPoit = disabled");
      break;
  }
  struct softap_config l_wifiSoftapConfig;
  struct station_config l_wifiStationConfig;
  // saved
  wifi_softap_get_config(&l_wifiSoftapConfig); 
  wifi_station_get_config(&l_wifiStationConfig); 
  String l_wifiSoftapConfig_ssid      = (char*)l_wifiSoftapConfig.ssid;
  String l_wifiSoftapConfig_password  = (char*)l_wifiSoftapConfig.password;
  String l_wifiStationConfig_ssid     = (char*)l_wifiStationConfig.ssid;
  String l_wifiStationConfig_password = (char*)l_wifiStationConfig.password;
  Serial.print("wifiSoftapConfig.ssid[32]      (CURRENT) : "); Serial.println(l_wifiSoftapConfig_ssid);
  Serial.print("wifiSoftapConfig.password[64]  (CURRENT) : "); Serial.println(l_wifiSoftapConfig_password);
  Serial.print("wifiStationConfig.ssid[32]     (CURRENT) : "); Serial.println(l_wifiStationConfig_ssid);
  Serial.print("wifiStationConfig.password[64] (CURRENT) : "); Serial.println(l_wifiStationConfig_password);
  // current
  wifi_softap_get_config_default(&l_wifiSoftapConfig); 
  wifi_station_get_config_default(&l_wifiStationConfig); 
  l_wifiSoftapConfig_ssid      = (char*)l_wifiSoftapConfig.ssid;
  l_wifiSoftapConfig_password  = (char*)l_wifiSoftapConfig.password;
  l_wifiStationConfig_ssid     = (char*)l_wifiStationConfig.ssid;
  l_wifiStationConfig_password = (char*)l_wifiStationConfig.password;
  Serial.print("wifiSoftapConfig.ssid[32]      (PERSIST) : "); Serial.println(l_wifiSoftapConfig_ssid);
  Serial.print("wifiSoftapConfig.password[64]  (PERSIST) : "); Serial.println(l_wifiSoftapConfig_password);
  Serial.print("wifiStationConfig.ssid[32]     (PERSIST) : "); Serial.println(l_wifiStationConfig_ssid);
  Serial.print("wifiStationConfig.password[64] (PERSIST) : "); Serial.println(l_wifiStationConfig_password);
#endif
}

void setupWifi() {
  char l_softapSsid[32] = {0};
  char l_softapPassword[64] = "12345678";
  uint32_t l_deviceId = system_get_chip_id();
  sprintf(l_softapSsid,"ESP-%04X",l_deviceId);  
  WiFi.persistent(false);
  WiFi.mode(WIFI_AP);  
  boolean l_softap = WiFi.softAP(l_softapSsid,l_softapPassword);
  if(l_softap == true) {
    Serial.print("Wifi AccessPoint ");
    Serial.print(l_softapSsid);
    Serial.print(" (password ");
    Serial.print(l_softapPassword);
    Serial.println(") started.");
  } else {
    Serial.println("Wifi AccessPoint failed!");
  }  
}

void setup() {
  Serial.begin(9600);
  delay(50);
  clearWifiConfig();
  setupWifi();
  debugWifiConfig();
}

void loop() {
}