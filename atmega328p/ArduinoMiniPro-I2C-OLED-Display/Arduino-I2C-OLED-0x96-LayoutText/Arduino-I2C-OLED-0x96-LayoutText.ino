// Test different text layouts

#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET    -1  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(100); // Pause for 2 seconds
}

void loop() {
  displayText1(); // Tiny
  displayText2(); // Small
  displayText3(); // Medium
  displayText4(); // Large
}

void displayText1(void) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1); // Draw 1X-scale text
  display.setCursor(0, 0);   display.println(F("Size1:789012345678901"));
  display.setCursor(0, 8);   display.println(F("S1 (0;8)  g"));
  display.setCursor(0,16);   display.println(F("S1 (0;16) g"));
  display.setCursor(0,24);   display.println(F("S1 (0;24) g"));
  display.setCursor(0,32);   display.println(F("S1 (0;32) g"));
  display.setCursor(0,40);   display.println(F("S1 (0;40) g"));
  display.setCursor(0,48);   display.println(F("S1 (0;48) g"));
  display.setCursor(0,56);   display.println(F("S1 (0;56) g"));
  display.display();
  delay(5000);
}

void displayText2(void) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2); // Draw 1X-scale text
  display.setCursor(0, 0);   display.println(F("Size2:7890"));
  display.setCursor(0,16);   display.println(F("S2 (0;16)g"));
  display.setCursor(0,32);   display.println(F("S3 (0;32)g"));
  display.setCursor(0,48);   display.println(F("S1 (0;48)g"));
  display.display();
  delay(5000);
}

void displayText3(void) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2); // Draw 1X-scale text
  display.setCursor(0, 0);   display.println(F("Size3:7890"));
  display.setTextSize(3); // Draw 1X-scale text
  display.setCursor(0,17);   display.println(F("g3;4567"));
  display.setCursor(0,41);   display.println(F("g3(;41)"));
  display.display();
  delay(5000);
}

void displayText4(void) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2); // Draw 1X-scale text
  display.setCursor(0, 0);   display.println(F("Size4:7890"));
  display.setTextSize(4); // Draw 1X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,16);   display.println(F("g4;45"));
  display.setTextSize(2); // Draw 1X-scale text
  display.setCursor(0,49);   display.println(F("g2(;49g"));
  display.display();
  delay(5000);
}

