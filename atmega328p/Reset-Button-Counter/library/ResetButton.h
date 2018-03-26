#define RESET_BUTTON_MAGIC   0xFACE800C /* detect poweron */
#define RESET_BUTTON_LIMIT   10000      /* ms end of refresh */
#define RESET_BUTTON_REFRESH 1000       /* ms refresh interval */
#define RESET_BUTTON_DISPLAY 3000       /* ms show current mode */
#define RESET_BUTTON_READY   2000       /* ms led on at start */
#define RESET_BUTTON_TOGGLE  0          /* ms toggle timeout */
#define RESET_BUTTON_MODES   3          /* number of modes */
#define RESET_BUTTON_LED     13         /* pin builtin led */

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

void SetupResetButton();
void InitResetSafeMemory();
void StartResetSafeMemory();
void RefreshResetSafeMemory();

void SetupResetButton() 
{
  // initialize memory
  InitResetSafeMemory();
  StartResetSafeMemory();
  pinMode(RESET_BUTTON_LED,OUTPUT);

  // toggle mode
  if (gResetSafeMemory.toggleMode == 1)
  {
    // toggle mode
    while (millis()<RESET_BUTTON_DISPLAY) 
    {
      // selected mode blink code
      for (byte i=0; i<=gResetSafeMemory.sketchMode; i++) 
      {
        digitalWrite(RESET_BUTTON_LED,HIGH); delay(150);
        digitalWrite(RESET_BUTTON_LED,LOW); delay(250);
      }
      delay(500);
      RefreshResetSafeMemory();
    }
    digitalWrite(RESET_BUTTON_LED,HIGH); delay(RESET_BUTTON_READY);
  }
  digitalWrite(RESET_BUTTON_LED,LOW);

  // start sketch
  RefreshResetSafeMemory();

  // push button
  if (gResetSafeMemory.pushButton == 1) 
  {
    // push button blink code (fast flash)
    for (byte i=0; i<=5; i++) 
    {
      digitalWrite(RESET_BUTTON_LED,HIGH); delay(150);
      digitalWrite(RESET_BUTTON_LED,LOW); delay(50);
    }
  }
}

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
