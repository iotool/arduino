// Arduino-LED-Sensor (V02)
// 
// Tested with green LED an 1 kOhm resistor at pin A5.
// LED produce up to 1.4 voltage and 3 micro amps current at light.
// The current is very small and messurements don not provide stable results.
// This sketch generates voltage floating at adc pin between LOW and HIGH.
// At darkness there is large floating (high variation) and 
// light produce reduce floating (low variation).
//
// Example to switch led on by blink code.
//
// A5 -- [1kOhm] -- [LED>| -- GND

#define APP_ADC5_PIN 19 /* pin A5 adc */
#define APP_SERIAL_INTERVAL 32 /* ms */
#define APP_ADC_BUFFER 8 /* values */
#define APP_GRAPH_BUFFER 8 /* ms */
#define APP_GRAPH_INTERVAL 32 /* ms */

static uint32_t appSerialTimeout = 0;
static byte     appPinAdcToogleFlag = 0;
static uint8_t  appPinAdcBuffer[APP_ADC_BUFFER] = {0};
static uint16_t appPinAdcVariation = 0;
static uint32_t appGraphTimeout = 0;
static uint16_t appGraphPinAdcVariationAvg = 0;
static uint8_t  appGraphBuffer[APP_GRAPH_BUFFER] = {0};
static uint16_t appGraphVariation = 0;
static uint32_t appBlinkTimeout = 0;

void setup() {
  pinMode(APP_ADC5_PIN, INPUT);
  Serial.begin(9600);
  while(!Serial) {;}
}

void loop() {

  unsigned long now = millis();

  /* --- read adc --- */

  // toggle floating
  if (appPinAdcToogleFlag == 0) {
    pinMode(APP_ADC5_PIN, INPUT_PULLUP);
    appPinAdcToogleFlag = 1;
  } else {
    pinMode(APP_ADC5_PIN, OUTPUT);
    appPinAdcToogleFlag = 0;
  }

  // read adc
  pinMode(APP_ADC5_PIN, INPUT);
  delay(1);
  uint16_t readAdc = analogRead(APP_ADC5_PIN);

  /* --- calculate light indicator --- */

  // reduce adc to 8 bit (small values)
  if (readAdc > 255) {
    readAdc = 255;
  }

  // add to buffer
  for (uint8_t i=(APP_ADC_BUFFER-1); i>0; i--) {
    appPinAdcBuffer[i] = appPinAdcBuffer[i-1];
  }
  appPinAdcBuffer[0] = readAdc;

  // calculate light indicator
  appPinAdcVariation = 0;
  for (uint8_t i=1; i<APP_ADC_BUFFER; i++) {
    if (appPinAdcBuffer[i] > appPinAdcBuffer[i-1]) {
      appPinAdcVariation += appPinAdcBuffer[i]-appPinAdcBuffer[i-1];
    } else {
      appPinAdcVariation += appPinAdcBuffer[i-1]-appPinAdcBuffer[i];
    }
  }

  /* --- calculate blink indicator --- */

  // calculate average light indicator
  appGraphPinAdcVariationAvg *= 15;
  appGraphPinAdcVariationAvg += appPinAdcVariation;
  appGraphPinAdcVariationAvg /= 16;

  // graph interval
  if (now-appGraphTimeout >= APP_GRAPH_INTERVAL) {
    appGraphTimeout = now;

    // add to buffer
    for (uint8_t i=(APP_GRAPH_BUFFER-1); i>0; i--) {
      appGraphBuffer[i] = appGraphBuffer[i-1];
    }
    uint16_t appGraphPinAdcVariationAvg8bit = appGraphPinAdcVariationAvg/8;
    if (appGraphPinAdcVariationAvg8bit > 255) {
      appGraphPinAdcVariationAvg8bit = 255;
    }
    appGraphBuffer[0] = appGraphPinAdcVariationAvg8bit;

    // calculate blink indicator
    appGraphVariation = 0;
    for (uint8_t i=1; i<APP_GRAPH_BUFFER; i++) {
      if (appGraphBuffer[i] > appGraphBuffer[i-1]) {
        appGraphVariation += appGraphBuffer[i]-appGraphBuffer[i-1];
      } else {
        appGraphVariation += appGraphBuffer[i-1]-appGraphBuffer[i];
      }
    }
  } 

  /* --- onBlink = led on --- */

  // set start timestamp
  if (appGraphVariation > 100) {
    if (appBlinkTimeout == 0) {
      appBlinkTimeout = now;
    }
  } else {
    appBlinkTimeout = 0;
  }

  // onBlink > 250ms
  if ((appBlinkTimeout+250<now) && (appBlinkTimeout > 0) && (now > 3000)) {
    pinMode(APP_ADC5_PIN, OUTPUT);
    digitalWrite(APP_ADC5_PIN, HIGH);
    delay(1);
    digitalWrite(APP_ADC5_PIN, LOW);
    pinMode(APP_ADC5_PIN, INPUT);
  }
  
  /* --- debug adc --- */
  if (now-appSerialTimeout >= APP_SERIAL_INTERVAL) {
    appSerialTimeout = now;
    Serial.print(1024); // max value
    Serial.print(" "); // next plotter value
    Serial.print(readAdc); // current value
    Serial.print(" "); // next plotter value
    Serial.print(appGraphVariation); // abs gap sum
    Serial.print(" "); // next plotter value
    Serial.print(appPinAdcVariation); // abs gap sum
    if ((appBlinkTimeout+250<now) && (appBlinkTimeout > 0) && (now > 3000)) {
      Serial.print(" "); // next plotter value
      Serial.print(500); // led on
    }else {
      Serial.print(" "); // next plotter value
      Serial.print(300); // led off
    }
    Serial.println();  
  }

}
