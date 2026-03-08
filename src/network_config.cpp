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
    .host = "192.168.0.20",              // ← CHANGE to your PC's IP address
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
// WOKWI SIMULATION CONFIGURATION (Wokwi simulated WiFi + local Mosquitto)
// ============================================================================

const WifiConfig WIFI_WOKWI = {
    .ssid = "Wokwi-GUEST",
    .password = ""                       // Wokwi-GUEST is open network
};

const MqttConfig MQTT_WOKWI = {
    .host = "broker.hivemq.com",              // Same as MQTT_DEV (host machine)
    .port = 1883,
    .clientId = "esp32-printer-client",
    .topic = "device/+/report"           // Matches fake_printer.py output
};

// ============================================================================
// SELECT ACTIVE CONFIGURATION (based on build environment)
// ============================================================================
#ifdef WOKWI_BUILD
// Wokwi Simulation Environment
const WifiConfig& WIFI_CONFIG = WIFI_WOKWI;
const MqttConfig& MQTT_CONFIG = MQTT_WOKWI;
#else
// Real Hardware - Use Development by default
// To use Production, comment out the two lines below and uncomment WIFI_PROD/MQTT_PROD
const WifiConfig& WIFI_CONFIG = WIFI_DEV;
const MqttConfig& MQTT_CONFIG = MQTT_DEV;
// const WifiConfig& WIFI_CONFIG = WIFI_PROD;
// const MqttConfig& MQTT_CONFIG = MQTT_PROD;
#endif
// ============================================================================
