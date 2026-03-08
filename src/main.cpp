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
#ifdef WOKWI_BUILD
  mqtt.publish("device/esp32/debug", "[ACK] Printer data received and parsed");
#endif
}

void setup() {
  Serial.begin(115200);
  Serial.println("[DEBUG] A - Serial started");
  delay(1000);
  Serial.println("[SETUP] Starting ESP32 printer monitor...");
  
  delay(100);

#ifndef WOKWI_BUILD
  Serial.println("[DEBUG] B - About to call displaySetup");
  displaySetup(display, canvas, &FreeSans12pt7b);
  Serial.println("[DEBUG] C - displaySetup done");
  
  neopixelSetup(neopixel, 30);
  Serial.println("[DEBUG] D - neopixelSetup done");
  
  batterySetup(battery);
  Serial.println("[DEBUG] E - batterySetup done");
  
  buttonSetup();
  Serial.println("[DEBUG] F - buttonSetup done");
#endif
  
  Serial.println("[SETUP] Connecting to WiFi...");
  Serial.println("[DEBUG] G - About to call wifiSetup");
  wifiSetup(WIFI_CONFIG.ssid, WIFI_CONFIG.password);
  Serial.println("[DEBUG] H - wifiSetup done");
  delay(2000);
  
  Serial.println("[SETUP] Initializing MQTT...");
  Serial.println("[DEBUG] I - About to set MQTT callback");
  mqtt.onDataReceived(onPrinterDataUpdate);
  Serial.println("[DEBUG] J - About to connect MQTT");
  
  if (mqtt.connect()) {
    Serial.println("[DEBUG] K - MQTT connected");
    mqtt.subscribe(MQTT_CONFIG.topic);
    Serial.println("[SETUP] ✅ MQTT connected and subscribed");
  } else {
    Serial.println("[SETUP] ❌ MQTT connection failed");
  }
  Serial.println("[DEBUG] L - Setup complete");
}

void loop() {
  // Handle MQTT messages (must be called regularly)
  mqtt.update();

#ifndef WOKWI_BUILD
  neopixel.setPixelColor(0, neopixel.Color(0,255,0));
  neopixel.show();
  // User Input Handling (Button D0 maps to changing pages, D1 maps to changing orientation)
  swapPages(current_page, num_pages);
  swapOrientation(current_orientation);
  if (current_page == 1) showDebugPage(display, canvas, battery, current_orientation);
  if (current_page == 2) showExamplePage(display, canvas);

  // Do not delete, do not edit.
  refreshDisplay(display, canvas);
#endif

  delayMicroseconds(16);
  return;
}