// Arduino-LED-Sensor
// 
// Tested with green LED an 1 kOhm resistor at pin A5.
// LED produce up to 1.4 voltage and 3 micro amps current at light.
// The current is very small and messurements don not provide stable results.
// This sketch generates voltage floating at adc pin between LOW and HIGH.
// At darkness there is large floating (high variation) and 
// light produce reduce floating (low variation).
// 
// A5 -- [1kOhm] -- [LED>| -- GND

#define APP_ADC5_PIN 19 /* pin A5 adc */
#define APP_SERIAL_INTERVAL 32 /* ms */
#define APP_ADC_BUFFER 8 /* values */

static uint32_t appStaticUint32 = 0;
static byte appPinToogle = 0;
uint16_t appPinAdcBuffer[APP_ADC_BUFFER] = {0};
uint8_t  appPinAdcPosition = 0;
uint16_t appPinAdcVariation = 0;

void setup() {
  pinMode(APP_ADC5_PIN, INPUT);
  Serial.begin(9600);
  while(!Serial) {;}
}

void loop() {

  // toggle floating
  if (appPinToogle == 0) {
    pinMode(APP_ADC5_PIN, INPUT_PULLUP);
    appPinToogle = 1;
  } else {
    pinMode(APP_ADC5_PIN, OUTPUT);
    appPinToogle = 0;
  }
  pinMode(APP_ADC5_PIN, INPUT);
  delay(1);

  // read adc
  uint16_t readAdc = analogRead(APP_ADC5_PIN);
  appPinAdcPosition++;
  if (appPinAdcPosition > APP_ADC_BUFFER) {
    appPinAdcPosition = 0;
  }
  appPinAdcBuffer[appPinAdcPosition] = readAdc;
  appPinAdcVariation = 0;
  for (byte i=1; i<APP_ADC_BUFFER; i++) {
    if (appPinAdcBuffer[i] > appPinAdcBuffer[i-1]) {
      appPinAdcVariation += appPinAdcBuffer[i]-appPinAdcBuffer[i-1];
    } else {
      appPinAdcVariation += appPinAdcBuffer[i-1]-appPinAdcBuffer[i];
    }
  }
  
  // debug adc
  unsigned long now = millis();
  if (now-appStaticUint32 >= APP_SERIAL_INTERVAL) {
    appStaticUint32 = now;
    Serial.print(1024); // max value
    Serial.print(" "); // next plotter value
    Serial.print(readAdc); // current value
    Serial.print(" "); // next plotter value
    Serial.print(appPinAdcVariation); // abs gap sum
    Serial.println();  
  }

}
