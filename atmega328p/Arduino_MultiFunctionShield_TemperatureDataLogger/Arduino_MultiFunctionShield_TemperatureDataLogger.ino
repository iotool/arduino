// 
//  Temperature Data Logger 24 hours (3 minutes)
// 
//  Arduino Uno R3
//  Multi-function Shield
//  2x DHT11 Temperature Sensor
// 
//  https://github.com/rocketscream/Low-Power
// 

#include "DHT.h"
#include <TimerOne.h>
#include <Wire.h>
#include <MultiFuncShield.h>
#include <EEPROM.h>

#define DHTTYPE   DHT11
#define DHT_1_PIN  5
#define DHT_2_PIN  6

DHT dht_1(DHT_1_PIN, DHTTYPE);
DHT dht_2(DHT_2_PIN, DHTTYPE);

byte  temp1b = 0; // temperature sensor 1 (byte)
byte  temp2b = 0; // temperature sensor 2 (byte)
float temp2f = 0; // temperature sensor 1 (float)
float temp1f = 0; // temperature sensor 2 (float)
int   adress = 0; // eeprom

void setup() {

  Serial.begin(9600);
  while(!Serial && millis()<100){;}
  Serial.print("Arduino MultiFunctionShield TemperatureDataLogger"); Serial.println(adress);
  Timer1.initialize();
  MFS.initialize(&Timer1);

  // show boot message
  MFS.write("boot"); delay(500);
  MFS.write("tENP"); delay(500);
  MFS.write("dAtA"); delay(500);
  MFS.write("LOGr"); delay(500);

  // clear eeprom (middle button at power on)
  if (digitalRead(16)==LOW) {
    MFS.write("cler");
    Serial.println("EEPROM-Format");
    for (int i = 0 ; i < EEPROM.length() ; i++) {
      EEPROM.write(i, 0);
    }
    while(true) {
      MFS.write("End"); delay(500);
      MFS.write("cler"); delay(500);
    }
  }

  // send data (left/right button at power on)
  if ((digitalRead(15)==LOW) || (digitalRead(17)==LOW)) {
    byte delay_ms = 1; if (digitalRead(15)==LOW) delay_ms = 50;
    byte display_flag = 0; if (digitalRead(17)==LOW) {display_flag = 1;}
    MFS.write("dAtA");
    Serial.print((float)0);
    Serial.print(",");
    Serial.print((float)0);
    Serial.println();
    for (int i = 0 ; i < EEPROM.length() ; i+=2) {
      temp1b = EEPROM.read(i);
      temp2b = EEPROM.read(i+1);
      temp1f = -9.0 + (float)temp1b/4;
      temp2f = -9.0 + (float)temp2b/4;
      if ((temp1b>0) && (temp2b>0)) {
        Serial.print((float)temp1f);
        Serial.print(",");
        Serial.print((float)temp2f);
        Serial.println();
        delay(delay_ms);
        if (display_flag) {
          MFS.write("Adr"); delay(500);
          MFS.write(i); delay(500);
          MFS.write(temp1f); delay(500);
          MFS.write(temp2f); delay(500);
          MFS.write("   "); delay(500);
          i-=2;
          if ((digitalRead(15)==HIGH)) i+=2; 
          if ((digitalRead(17)==HIGH) && (i >= 0)) i-=2; 
        }
      }
    }
    while(true) {
      MFS.write("End"); delay(500);
      MFS.write("dAtA"); delay(500);
    }
  }

  // record data
  MFS.write("run"); delay(500);
  for (int i = 0 ; i < EEPROM.length() ; i+=2) {
    temp1b = EEPROM.read(i);
    temp2b = EEPROM.read(i+1);
    if ((temp1b>0) && (temp2b>0)) {
      adress += 2;
    } else {
      i = EEPROM.length();
    }
  }
  Serial.print("EEPROM.adress "); Serial.println(adress);
  dht_1.begin();
  dht_2.begin();
  MFS.write("adr "); delay(500); MFS.write(adress); delay(500);
  MFS.write("    "); delay(100);

}

byte  t1_index = 0;
byte  t2_index = 0;
float t1_value[10] = {0};
float t2_value[10] = {0};
byte  t1_count = 0;
byte  t2_count = 0;
byte  loop_count = 0;

#define LOGINTERVAL 3*60000 /* 3x min => 24x hours 20x 2x byte => 960 byte */
unsigned long nextLogTime = LOGINTERVAL;

void loop() {

  // sleep 8 seconds, keep powerbank alive
  delay(8000);
  MFS.write(" -- "); delay(10); 

  // messure temperature
  float h_1 = 0;
  float t_1 = 0;
  float h_2 = 0;
  float t_2 = 0;
  for (byte i=0;i<3;i++) {
    h_1 += dht_1.readHumidity();     delay(50);
    t_1 += dht_1.readTemperature();  delay(50);
    h_2 += dht_2.readHumidity();     delay(50);
    t_2 += dht_2.readTemperature();  delay(50);
  }
  h_1 /= 3;
  t_1 /= 3;
  h_2 /= 3;
  t_2 /= 3;

  // calibration
  t_1 += 0.4;
  t_2 -= 0.9;

  // average
  t1_count += (t1_count < 10 ? 1 : 0);
  t2_count += (t2_count < 10 ? 1 : 0);
  t1_value[t1_index % 10] = t_1;
  t2_value[t2_index % 10] = t_2;
  t1_index++;
  t2_index++;
  float t1_avg = 0;  for (byte i1=0;i1<t1_count;i1++) {t1_avg += t1_value[i1];} t1_avg /= t1_count;
  float t2_avg = 0;  for (byte i2=0;i2<t1_count;i2++) {t2_avg += t2_value[i2];} t2_avg /= t2_count;

  // convert for eeprom
  temp1b = (t1_avg+9)*4;
  temp2b = (t2_avg+9)*4;
  temp1f = -9.0 + (float)temp1b/4;
  temp2f = -9.0 + (float)temp2b/4;

  // save at eeprom
  unsigned long thisTime = millis();
  if (nextLogTime <= thisTime) {
    nextLogTime = thisTime + LOGINTERVAL; // Min
    MFS.write("log ");  delay(500);
    if (adress < EEPROM.length()) {
       EEPROM.write(adress, temp1b);
       EEPROM.write(adress+1, temp2b);
       MFS.write("adr=");  delay(500);
       MFS.write(adress);  delay(500);
       adress += 2;
    }
  }
  Serial.print("LOG adr="); Serial.print(adress);
  Serial.print(" next="); Serial.print(nextLogTime);
  Serial.print(" millis="); Serial.println(thisTime);

  // debug
  Serial.print("Sensor 1: hum="); Serial.print(h_1); Serial.print(" %\t"); 
  Serial.print(" temp=");Serial.print(t_1); Serial.print(" *C ");
  Serial.print(" avg="); Serial.print(t1_avg); Serial.print(" *C ");
  Serial.print(" cnt=");Serial.print(t1_count);
  Serial.print(" eeprom: byte=");Serial.print(temp1b);
  Serial.print(" float="); Serial.print(temp1f); Serial.println("");
  Serial.print("Sensor 2: hum="); Serial.print(h_2); Serial.print(" %\t"); 
  Serial.print(" temp=");Serial.print(t_2); Serial.print(" *C ");
  Serial.print(" avg="); Serial.print(t2_avg); Serial.print(" *C ");
  Serial.print(" cnt=");Serial.print(t2_count);
  Serial.print(" eeprom: byte=");Serial.print(temp2b);
  Serial.print(" float="); Serial.print(temp2f); Serial.println("");

  // display
  loop_count++;
  if (loop_count % 7 == 0) {
    MFS.write("T1_5");  delay(500);  MFS.write((float)t1_avg,2);  delay(750);
    MFS.write("T2_6");  delay(500);  MFS.write((float)t2_avg,2);  delay(750);
    MFS.write("tine");  delay(500);  MFS.write((float)thisTime/60000,1); delay(750);
    MFS.write("nect");  delay(500);  MFS.write((float)(nextLogTime-thisTime)/60000,1); delay(750);
    MFS.write("adr");   delay(500);  MFS.write(adress); delay(750);
  } else {
    MFS.write((float)t1_avg,2);  delay(750);
    MFS.write((float)t2_avg,2);  delay(750);
  }
  MFS.write("    "); delay(100);

}
