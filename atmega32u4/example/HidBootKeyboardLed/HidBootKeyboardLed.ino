// HidBootKeyboardLed.ino
// Use https://github.com/NicoHood/HID/

#include "HID-Project.h"

static uint8_t gLedPrevState;

void setup() 
{
  // Sends a clean report to the host. This is important on any Arduino type.
  BootKeyboard.begin();
  delay(2000);

  // Get initial led state
  gLedPrevState = BootKeyboard.getLeds();
}


void loop() 
{
  // detect changed state
  uint8_t lLedState = BootKeyboard.getLeds();
  uint16_t lLedNumber = lLedState;
  
  // on change
  if (lLedState != gLedPrevState) {
    BootKeyboard.print("LED_LOCK changed ");
    BootKeyboard.println(lLedNumber);
    
    // NUM_LOCK
    if (lLedState & LED_NUM_LOCK) {
      if (!(gLedPrevState & LED_NUM_LOCK)) {
        BootKeyboard.println("NUM_LOCK on");
      }
    } else if (gLedPrevState & LED_NUM_LOCK) {
        BootKeyboard.println("NUM_LOCK off");
    }

    // CAPS_LOCK
    if (lLedState & LED_CAPS_LOCK) {
      if (!(gLedPrevState & LED_CAPS_LOCK)) {
        BootKeyboard.println("CAPS_LOCK on");
      }
    } else if (gLedPrevState & LED_CAPS_LOCK) {
        BootKeyboard.println("CAPS_LOCK off");
    }

    // SCROLL_LOCK
    if (lLedState & LED_SCROLL_LOCK) {
      if (!(gLedPrevState & LED_SCROLL_LOCK)) {
        BootKeyboard.write(KEY_SCROLL_LOCK);
        BootKeyboard.println("SCROLL_LOCK on");
      }
    } else if (gLedPrevState & LED_SCROLL_LOCK) {
        BootKeyboard.println("SCROLL_LOCK off");
    }
    gLedPrevState = lLedState;
  }

  // 
  delay(100);
}
