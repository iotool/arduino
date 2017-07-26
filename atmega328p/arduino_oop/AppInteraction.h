// APPUI : AppInteraction.h (Realtime)
// 
// v003: timer, loop

#include "AppShield.h"

class AppInteraction: public AppShield {
  private:
    /* --- class (internal) --- */
    void _boot();
    void _init();
    void _timer();
    void _loop();
    unsigned long _uiTimer;
    unsigned long _uiLoop;
  protected:
    /* --- package (internal) --- */
    void uiInit() { hwInit(); _init(); }
    void uiLoop() { _loop(); }
  public:
    /* --- application (external) --- */
    AppInteraction();
    void timer() { hwTimer(); _timer(); }
};

