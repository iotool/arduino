// APPUI : AppInteraction.cpp (Realtime)
// 
// v003: timer, loop

#include "AppInteraction.h"

AppInteraction::AppInteraction() {
  this->_boot();
}

void AppInteraction::_boot() {
}

void AppInteraction::_init() {
  this->_uiTimer = 1;
  this->_uiLoop = 1;
  Serial.begin(9600);
  while(!Serial && micros()<100){;}
  Serial.println(APPUT_VERSION_STR);
}

void AppInteraction::_timer() {
  this->_uiTimer++;
}

void AppInteraction::_loop() {
  this->_uiLoop++;
  switch(this->_uiLoop % 50) {
    case 0: Serial.print("timer="); break;
    case 1: Serial.print(this->_uiTimer); break;
    case 2: Serial.print(" loop="); break;
    case 3: Serial.println(this->_uiLoop); break;
  }
}

