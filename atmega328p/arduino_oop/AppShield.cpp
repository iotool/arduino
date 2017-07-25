// APPHW : AppShield.cpp (Driver)

#include "AppShield.h"

AppShield::AppShield() {
  this->_init();
}

void AppShield::_init() {
  this->_hwInit = 1;
  this->_hwTimer = 1;
}

void AppShield::_timer() {
  this->_hwTimer++;
}

