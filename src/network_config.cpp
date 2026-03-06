#include "network_config.hpp"

/**
 * @file network_config.cpp
 * @brief Implementation of WiFi and MQTT configuration constants
 * 
 * To swap between dev and prod: Change WIFI_CONFIG and MQTT_CONFIG references at bottom
 */

// ============================================================================
// DEVELOPMENT CONFIGURATION (local Mosquitto + test WiFi)
// ============================================================================

const WifiConfig WIFI_DEV = {
    .ssid = "PRAXIS_STUDENT",
    .password = "password"
};

const MqttConfig MQTT_DEV = {
    .host = "192.168.x.y",              // ← CHANGE to your PC's IP address
    .port = 1883,
    .clientId = "esp32-printer-client",
    .topic = "device/+/report"           // ← Matches fake_printer.py output
};

// ============================================================================
// PRODUCTION CONFIGURATION (real Bambu Lab printer)
// ============================================================================

const WifiConfig WIFI_PROD = {
    .ssid = "YOUR_LAB_WIFI_SSID",
    .password = "YOUR_LAB_WIFI_PASSWORD"
};

const MqttConfig MQTT_PROD = {
    .host = "10.10.2.20",                // Real Bambu Lab printer
    .port = 8883,                        // TLS port
    .clientId = "esp32-printer-client",
    .topic = "device/+/report"           // Real printer topic
};

// ============================================================================
// SELECT ACTIVE CONFIGURATION
// ============================================================================
// Change these two lines to swap between development and production
// 
// For development (mock Mosquitto):
const WifiConfig& WIFI_CONFIG = WIFI_DEV;
const MqttConfig& MQTT_CONFIG = MQTT_DEV;
//
// For production (real Bambu Lab printer):
// const WifiConfig& WIFI_CONFIG = WIFI_PROD;
// const MqttConfig& MQTT_CONFIG = MQTT_PROD;
// ============================================================================
