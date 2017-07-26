// APPHW : AppShield.h (Driver)
// 
// v003: timer

#include "AppUtil.h"
#include <avr/pgmspace.h>

class AppShield {
  private:
    /* --- class (internal) --- */
    void _boot();
    void _init();
    void _timer();
    unsigned int _hwTimer;
  protected:
    /* --- package (internal) --- */
    void hwInit() { _init(); }
    void hwTimer() { _timer(); }
  public: 
    /* --- application (external) --- */
    AppShield();
};

