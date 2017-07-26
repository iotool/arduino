// APPCL : AppControl.cpp (Workflow)
// 
// v003: loop

#include "AppControl.h"

AppControl::AppControl() {
  this->_boot();
}

void AppControl::_boot() {
}

void AppControl::_init() {
  this->_clLoop = 1;
}

void AppControl::_loop() {
  this->_clLoop++;
}

