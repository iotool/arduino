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

#define RESET_BUTTON_MAGIC   0xFACE800C
#define RESET_BUTTON_LIMIT   60000
#define RESET_BUTTON_REFRESH 1000
#define RESET_BUTTON_DISPLAY 3000
#define RESET_BUTTON_READY   2000
#define RESET_BUTTON_MODES   3

typedef struct {
  uint32_t initMemory;
  uint32_t uptimePrev;
  uint8_t  sketchMode;
  uint8_t  pushButton;
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
  Serial.print("UptimePrev = "); Serial.println(gResetSafeMemory.uptimePrev);  
  Serial.print("sketchMode = "); Serial.println(gResetSafeMemory.sketchMode);  
  Serial.print("pushButton = "); Serial.println(gResetSafeMemory.pushButton);
  StartResetSafeMemory();

  // toggle mode
  pinMode(13,OUTPUT);
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
  digitalWrite(13,LOW);
  // start sketch
  Serial.println("Ready");
  delay(10);
  RefreshResetSafeMemory();
    
  // ... your code here ...
  if (gResetSafeMemory.pushButton == 1) 
  {
    Serial.println("Button");
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
    gResetSafeMemory.pushButton = 0;
  } 
  else 
  {
    // reset
    gResetSafeMemory.pushButton = 0;
    if (gResetSafeMemory.uptimePrev < (RESET_BUTTON_DISPLAY+RESET_BUTTON_READY)) 
    {
      // during start
      gResetSafeMemory.sketchMode++;
      if (gResetSafeMemory.sketchMode >= RESET_BUTTON_MODES) 
      {
        gResetSafeMemory.sketchMode = 0;
      }
    }
    else 
    {
      // after start
      gResetSafeMemory.pushButton = 1;
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
