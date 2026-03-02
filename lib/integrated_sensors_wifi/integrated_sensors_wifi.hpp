#include <Arduino.h>
#include <Adafruit_MAX1704X.h>
#include <WiFi.h>

#ifndef fablab_integrated_sensors_wifi_hpp
#define fablab_integrated_sensors_wifi_hpp 

/**
 * @brief Abstraction to set up the Adafruit Max17048 Integrated Battery Management IC.
 * @param battery Adafruit_MAX17048 Reference
 */
void batterySetup(Adafruit_MAX17048 &battery);

/**
 * @brief Abstraction to set up the mode for the built-in buttons.
 */
void buttonSetup();

/**
 * @brief Abstraction to set up a WiFi connection.
 * @param ssid The Wireless Network ssid (or name)
 * @param password The Wireless Network Password
 */
bool wifiSetup(const char* ssid, const char* password);

#endif