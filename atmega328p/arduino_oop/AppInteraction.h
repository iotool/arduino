// APPUI : AppInteraction.h (Realtime)

#include "AppShield.h"

class AppInteraction: private AppShield {
  private:
    void _init();
    void _timer();
    unsigned char _uiInit;
    unsigned int _uiTimer;
  protected:
    void uiInit() { hwInit(); _init(); }
    void uiTimer() { hwTimer(); _timer(); }
  public:
    AppInteraction();
};

