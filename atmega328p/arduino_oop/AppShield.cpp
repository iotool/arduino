// APPHW : AppShield.cpp (Driver)
// 
// v003: timer

#include "AppShield.h"

AppShield::AppShield() {
  this->_boot();
}

void AppShield::_boot() {
}

void AppShield::_init() {
  this->_hwTimer = 1;
}

void AppShield::_timer() {
  this->_hwTimer++;
}

