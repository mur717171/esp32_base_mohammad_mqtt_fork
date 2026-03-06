#ifndef MQTT_ESP32_HPP
#define MQTT_ESP32_HPP

#include <Arduino.h>
#include <ArduinoMqttClient.h>
#include <WiFi.h>
#include "printer_data.hpp"
#include <functional>
#include <string>

/**
 * @class MqttEsp32
 * @brief Arduino-compatible MQTT wrapper using ArduinoMqttClient
 * Handles connection, subscription, message parsing (JSON), and printer data updates
 */
class MqttEsp32 {
public:
    using DataCallback = std::function<void(const PrinterData&)>;
    
    /**
     * @brief Constructor
     * @param broker_host MQTT broker hostname or IP
     * @param broker_port MQTT broker port (default 1883 for non-TLS)
     * @param client_id MQTT client identifier
     */
    MqttEsp32(
        const char* broker_host = "localhost",
        int broker_port = 1883,
        const char* client_id = "esp32-printer-client"
    );
    
    ~MqttEsp32();
    
    /**
     * @brief Connect to MQTT broker
     * @return true if connection successful, false otherwise
     */
    bool connect();
    
    /**
     * @brief Disconnect from MQTT broker
     * @return true if disconnection successful
     */
    bool disconnect();
    
    /**
     * @brief Handle MQTT message polling
     * Call this regularly in Arduino loop() to process incoming messages
     */
    void update();
    
    /**
     * @brief Subscribe to an MQTT topic
     * @param topic Topic string (supports wildcards like "device/+/report")
     */
    void subscribe(const char* topic);
    
    /**
     * @brief Get the latest printer data
     * @return const reference to PrinterData struct
     */
    const PrinterData& getPrinterData() const { return printer_data_; }
    
    /**
     * @brief Check if connected to broker
     * @return true if connected
     */
    bool isConnected() const;
    
    /**
     * @brief Register callback for when printer data is updated
     * @param callback Function to call when new data arrives
     */
    void onDataReceived(DataCallback callback) { data_callback_ = callback; }
    
private:
    // Configuration
    std::string broker_host_;
    int broker_port_;
    std::string client_id_;
    
    // MQTT client and WiFi
    WiFiClient wifi_client_;
    MqttClient mqtt_client_;
    
    // Shared printer data
    PrinterData printer_data_;
    
    // User callback
    DataCallback data_callback_;
    
    // Message callback handlers
    void onMessageReceived(int messageSize);
    void parseJsonMessage(const String& json_str);
};

#endif // MQTT_ESP32_HPP
