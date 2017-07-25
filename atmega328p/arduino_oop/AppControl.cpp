// APPCL : AppControl.cpp (Workflow)

#include "AppControl.h"

AppControl::AppControl() {
  this->_init();
}

void AppControl::_init() {
  this->_clInit = 1;
  this->_clTimer = 1;
}

void AppControl::_timer() {
  this->_clTimer++;
}

