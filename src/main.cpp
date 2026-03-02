#include <Arduino.h>
#include <Fonts/FreeSans12pt7b.h>

// Custom Library Includes:
#include <integrated_display.hpp>
#include <integrated_sensors_wifi.hpp>
#include <display_pages.hpp>

// Config Includes:
// CONFIG INCLUDED IN GITIGNORE, COPY AND PASTE INTO networkconfig.h!!
#include <networkconfig.h> 

/*
// networkconfig.h:
constexpr const char* WIFI_SSID = "PRAXIS_STUDENT";
constexpr const char* WIFI_PASS = "password";
*/

// Define Integrated Peripherals:
// Do not remove, do not edit.
Adafruit_ST7789 display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16 canvas(240, 135);
Adafruit_NeoPixel neopixel(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
Adafruit_MAX17048 battery;

// Define Sensor Objects Here:
//
//
//

// Define the number of pages here:
const uint8_t num_pages = 2;
uint8_t current_page = 1;

void setup() {
  delay(100);
  
  // Do not remove, do not edit.
  displaySetup(display, canvas, &FreeSans12pt7b);
  neopixelSetup(neopixel, 30);
  batterySetup(battery);
  buttonSetup();
  wifiSetup(WIFI_SSID, WIFI_PASS);

  // Initiate Sensor Setup Logic Here:
  // if(!mysensor.begin()) {
  // };
  // 
  //

}

void loop() {
  neopixel.setPixelColor(0, neopixel.Color(0,255,0));
  neopixel.show();
  swapPages(current_page, num_pages);
  if (current_page == 1) showDebugPage(display, canvas, battery);
  if (current_page == 2) showExamplePage(display, canvas);

  // Do not delete, do not edit.
  refreshDisplay(display, canvas);
  delayMicroseconds(16);
  return;
}
