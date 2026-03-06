#pragma once

/**
 * @file network_config.hpp
 * @brief Configuration structs for WiFi and MQTT
 * Define config in network_config.cpp and select which to use
 */

struct WifiConfig {
    const char* ssid;
    const char* password;
};

struct MqttConfig {
    const char* host;
    int port;
    const char* clientId;
    const char* topic;
};

// Configuration declarations (defined in network_config.cpp)
extern const WifiConfig WIFI_DEV;
extern const WifiConfig WIFI_PROD;
extern const MqttConfig MQTT_DEV;
extern const MqttConfig MQTT_PROD;

// Active configs (change these to swap between dev and prod)
extern const WifiConfig& WIFI_CONFIG;
extern const MqttConfig& MQTT_CONFIG;
