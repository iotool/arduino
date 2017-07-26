// APPFW : App.ino (Main)
// 
// Application Layer
// FW ~ Main
// CL ~ Workflow
// UI ~ Realtime
// HW ~ Driver
// 
// v003: software timer

//--- #include <TimerOne.h>

#include "AppControl.h"
AppControl app;

void setup() {
  app.init();
  //--- Timer1.initialize(APPHW_TIMER_MICROS);
  //--- Timer1.attachInterrupt(interrupt);
}

unsigned int g_fwUptime = 0;
unsigned int g_fwTimer = 0;
unsigned int g_fwLoop = 0;
unsigned int g_fwTimerOverflow = (unsigned int)(0-APPHW_TIMER_MICROS);
unsigned int g_fwLoopOverflow = (unsigned int)(0-APPFW_LOOP_MICROS);

void loop() {
  g_fwUptime = micros() & 0xFFFF;
  if ( (g_fwTimer <= g_fwUptime) && (g_fwTimer <= 0xFFFF) && (g_fwUptime <= 0xFFFF) 
    || (g_fwTimer > APPHW_TIMER_MICROS) && (g_fwUptime < g_fwTimerOverflow) && (g_fwTimer-g_fwUptime > APPHW_TIMER_MICROS) ) {
    g_fwTimer = g_fwUptime + APPHW_TIMER_MICROS;
    app.timer();
  } else 
  if ( (g_fwLoop <= g_fwUptime) && (g_fwLoop <= 0xFFFF) && (g_fwUptime <= 0xFFFF) 
    || (g_fwLoop > APPFW_LOOP_MICROS) && (g_fwUptime < g_fwLoopOverflow) && (g_fwLoop-g_fwUptime > APPFW_LOOP_MICROS) ) {
    g_fwLoop = g_fwUptime + APPFW_LOOP_MICROS;
    app.loop();
  }
}

//--- void interrupt() {
//---   app.timer();
//--- }
