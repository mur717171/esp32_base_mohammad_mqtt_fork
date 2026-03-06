#include <Arduino.h>
#include <Fonts/FreeSans12pt7b.h>

// Custom Library Includes:
#include <integrated_display.hpp>
#include <integrated_sensors_wifi.hpp>
#include <display_pages.hpp>
#include <mqtt_esp32.hpp>
#include <network_config.hpp>

// Define Integrated Peripherals:
// Do not remove, do not edit.
Adafruit_ST7789 display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16 canvas(240, 135);
Adafruit_NeoPixel neopixel(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
Adafruit_MAX17048 battery;

// MQTT Client Instance (uses network_config.hpp for host/port/topic)
MqttEsp32 mqtt(MQTT_CONFIG.host, MQTT_CONFIG.port, MQTT_CONFIG.clientId);

// Define the number of pages here:
const uint8_t num_pages = 2;
uint8_t current_page = 1;
uint8_t current_orientation = 1;

/**
 * @brief Callback when printer data is received via MQTT
 */
void onPrinterDataUpdate(const PrinterData& data) {
  Serial.println("\n[APP] Printer data received:");
  data.print();
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n[SETUP] Starting ESP32 printer monitor...");
  
  delay(100);
  
  // Do not remove, do not edit.
  displaySetup(display, canvas, &FreeSans12pt7b);
  neopixelSetup(neopixel, 30);
  batterySetup(battery);
  buttonSetup();
  
  // Setup WiFi using config from network_config.hpp
  Serial.println("[SETUP] Connecting to WiFi...");
  wifiSetup(WIFI_CONFIG.ssid, WIFI_CONFIG.password);
  delay(2000);  // Give WiFi time to connect
  
  // Setup MQTT
  Serial.println("[SETUP] Initializing MQTT...");
  mqtt.onDataReceived(onPrinterDataUpdate);
  
  if (mqtt.connect()) {
    mqtt.subscribe(MQTT_CONFIG.topic);
    Serial.println("[SETUP] ✅ MQTT connected and subscribed");
  } else {
    Serial.println("[SETUP] ❌ MQTT connection failed");
  }
}

void loop() {
  // Handle MQTT messages (must be called regularly)
  mqtt.update();
  
  neopixel.setPixelColor(0, neopixel.Color(0,255,0));
  neopixel.show();
  // User Input Handling (Button D0 maps to changing pages, D1 maps to changing orientation)
  swapPages(current_page, num_pages);
  swapOrientation(current_orientation);
  if (current_page == 1) showDebugPage(display, canvas, battery, current_orientation);
  if (current_page == 2) showExamplePage(display, canvas);

  // Do not delete, do not edit.
  refreshDisplay(display, canvas);
  delayMicroseconds(16);
  return;
}