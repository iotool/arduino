// APPFW : App.ino (Main)// // Application Layer// FW ~ Main// CL ~ Workflow// UI ~ Realtime// HW ~ Driver
#include "AppControl.h"
AppControl app;
void setup() {  app.init();}
void loop() {  app.timer();}
