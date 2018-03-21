//////////////////////////////////////////////////////////////////////
//                                                                  //
//               Reset-Button-Counter by IoTool                     //
//                                                                  //
//    https://github.com/iotool/arduino/tree/master/atmega328p/     //
//////////////////////////////////////////////////////////////////////
// 
//  This sketch use the Arduino reset button like a push button.
//  During power on you can choose between three different modes.
//  The selected mode is indicated by a blink code (default 0).
//  After start, the click can be interpreted as a push or reset.
// 
//  RESET_BUTTON_TOGGLE  0          enable toggle all the time
//  RESET_BUTTON_TOGGLE  5000       disable toggle after 5 seconds       
//

#define RESET_BUTTON_MAGIC   0xFACE800C /* detect poweron */
#define RESET_BUTTON_LIMIT   10000      /* ms end of refresh */
#define RESET_BUTTON_REFRESH 1000       /* ms refresh interval */
#define RESET_BUTTON_DISPLAY 3000       /* ms show current mode */
#define RESET_BUTTON_READY   2000       /* ms led on at start */
#define RESET_BUTTON_TOGGLE  0          /* ms toggle timeout */
#define RESET_BUTTON_MODES   3          /* number of modes */

typedef struct {
  uint32_t initMemory;                  // initial checksum
  uint32_t uptimePrev;                  // previouse uptime
  uint8_t  resetCount;                  // reset counter
  uint8_t  sketchMode;                  // current sketch mode
  unsigned pushButton:1;                // flag push button
  unsigned toggleMode:1;                // flag toggle enabled
  unsigned :6;                          // reserved (8 bit aligned)
} tResetSafeMemory;

tResetSafeMemory gResetSafeMemory \
  __attribute__ ((__section__ (".noinit")))
;

void InitResetSafeMemory();
void StartResetSafeMemory();
void RefreshResetSafeMemory();

////////////// Main

void setup() 
{
  // read mode
  InitResetSafeMemory();
  Serial.begin(9600);
  delay(10);
  Serial.println();  
  Serial.print("resetCount = "); Serial.println(gResetSafeMemory.resetCount);  
  Serial.print("UptimePrev = "); Serial.println(gResetSafeMemory.uptimePrev);  
  Serial.print("sketchMode = "); Serial.println(gResetSafeMemory.sketchMode);  
  Serial.print("pushButton = "); Serial.println(gResetSafeMemory.pushButton);
  Serial.print("toggleMode = "); Serial.println(gResetSafeMemory.toggleMode);
  StartResetSafeMemory();

  // toggle mode
  pinMode(13,OUTPUT);
  if (gResetSafeMemory.toggleMode == 1)
  {
    // toggle mode
    while (millis()<RESET_BUTTON_DISPLAY) 
    {
      // display blink code
      for (byte i=0; i<=gResetSafeMemory.sketchMode; i++) 
      {
        digitalWrite(13,HIGH); delay(150);
        digitalWrite(13,LOW); delay(250);
      }
      delay(500);
      RefreshResetSafeMemory();
    }
    digitalWrite(13,HIGH); delay(RESET_BUTTON_READY);
  }
  digitalWrite(13,LOW);
  // start sketch
  Serial.println("Ready");
  delay(10);
  RefreshResetSafeMemory();
    
  // ... your code here ...
  if (gResetSafeMemory.pushButton == 1) 
  {
    Serial.println("Button");
    // display blink code
    for (byte i=0; i<=5; i++) 
    {
      digitalWrite(13,HIGH); delay(150);
      digitalWrite(13,LOW); delay(50);
    }
  }
}

void loop() 
{
  delay(1);
  // ... your code here ...
}

////////////// ResetHandler

void InitResetSafeMemory() 
{
  if (gResetSafeMemory.initMemory != RESET_BUTTON_MAGIC) 
  {
    // poweron
    gResetSafeMemory.initMemory = RESET_BUTTON_MAGIC;
    gResetSafeMemory.uptimePrev = 0;
    gResetSafeMemory.sketchMode = 0;
    gResetSafeMemory.resetCount = 0;
    gResetSafeMemory.pushButton = 0;
    gResetSafeMemory.toggleMode = 1;
  } 
  else 
  {
    // reset
    gResetSafeMemory.resetCount++;
    if (gResetSafeMemory.toggleMode == 0) 
    {
      // toggle disabled after timeout
      gResetSafeMemory.pushButton = 1;
    }
    else if ((RESET_BUTTON_TOGGLE > 0) && (gResetSafeMemory.uptimePrev > RESET_BUTTON_TOGGLE)) 
    {
      // disable toggle by timeout
      gResetSafeMemory.toggleMode = 0;
      gResetSafeMemory.pushButton = 1;
    }
    else if (gResetSafeMemory.uptimePrev > (RESET_BUTTON_DISPLAY+RESET_BUTTON_READY)) 
    {
      // after select-mode timeout
      gResetSafeMemory.pushButton = 1;
    }
    else 
    {
      // during start
      gResetSafeMemory.pushButton = 0;
      gResetSafeMemory.sketchMode++;
      if (gResetSafeMemory.sketchMode >= RESET_BUTTON_MODES) 
      {
        gResetSafeMemory.sketchMode = 0;
      }      
    }
  }
}

void StartResetSafeMemory() 
{
  // after start
  gResetSafeMemory.uptimePrev = 0;
}

void RefreshResetSafeMemory() 
{
  // increment uptime
  if (gResetSafeMemory.uptimePrev < RESET_BUTTON_LIMIT) 
  {
    if ((millis()-gResetSafeMemory.uptimePrev) > RESET_BUTTON_REFRESH) 
    {
      gResetSafeMemory.uptimePrev = millis();
    }
  }
}
