// APPUI : AppInteraction.cpp (Realtime)

#include "AppInteraction.h"

AppInteraction::AppInteraction() {
  this->_init();
}

void AppInteraction::_init() {
  this->_uiInit = 1;
  this->_uiTimer = 1;
}

void AppInteraction::_timer() {
  this->_uiTimer++;
}

