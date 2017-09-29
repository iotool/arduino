/*

  WorkshopSwitchSketch.ino

  Switch between sketches during a workshop without upload.

  Enter select mode by push button during power on.
  Each button push will iterate to the next sketch. 
  The builtin led will blink one time for sketch 1,
  two times for skect 2 and so one.

  To reduce memory usage to only 1 byte I removed serial debuging.
 
 */

#define APP_BUTTON_PIN            16  /* pin A2  mfs button */
#define APP_LED_PIN               13  /* pin D13 builtin led */
#define APP_COUNT                 3   /* sketches */
#define APP_STATIC_UINT32_BUFFER  4   /* static variables */

#include "app.h"
#include "app1.h"
#include "app2.h"
#include "app3.h"

void setup() {
  switch(appSetup(APP_COUNT,APP_BUTTON_PIN,APP_LED_PIN)) {
    case 1: setup_APP1(); break;
    case 2: setup_APP2(); break;
    case 3: setup_APP3(); break;
  }
}

void loop() {
  switch(appLoop()) {
    case 1: loop_APP1(); break;
    case 2: loop_APP2(); break;
    case 3: loop_APP3(); break;
  }
}
