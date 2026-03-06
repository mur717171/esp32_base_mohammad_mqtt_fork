#ifndef MQTT_CLIENT_HPP
#define MQTT_CLIENT_HPP

#include "printer_data.hpp"
#include <string>
#include <functional>
#include <MQTTClient.h>
#include <nlohmann/json.hpp>

class PrinterMqttClient {
public:
    PrinterMqttClient(
        const std::string& id,
        const std::string& host,
        int port,
        const std::string& username = "",
        const std::string& password = "",
        bool use_tls = false
    );
    
    ~PrinterMqttClient();
    
    bool connect();
    bool disconnect();
    int loop(int timeout_ms = 500);
    void subscribe_to_topic(const std::string& topic);
    
    const PrinterData& get_printer_data() const { return printer_data_; }
    bool is_connected() const { return is_connected_; }
    
    using DataCallback = std::function<void(const PrinterData&)>;
    void set_data_callback(DataCallback cb) { data_callback_ = cb; }

private:
    // Static wrapper callbacks that receive 'this' pointer in context
    static int on_message_arrived_wrapper(void* context, char* topicName, 
                                          int topicLen, MQTTClient_message* message);
    static void on_connection_lost_wrapper(void* context, char* cause);
    static void on_delivery_complete_wrapper(void* context, MQTTClient_deliveryToken token);
    
    // Instance methods called by the wrappers
    void on_message_arrived(char* topicName, int topicLen, MQTTClient_message* message);
    void on_connection_lost(const char* cause);
    void on_delivery_complete(MQTTClient_deliveryToken token);
    void handle_message(const char* topic, const char* payload, int payload_len);
    void parse_printer_data(const std::string& json_payload);
    
    // Paho C library handle
    MQTTClient client_ = nullptr;
    
    std::string host_;
    int port_;
    std::string username_;
    std::string password_;
    bool use_tls_;
    bool is_connected_;
    
    PrinterData printer_data_;
    std::function<void(const PrinterData&)> data_callback_;
};

#endif // MQTT_CLIENT_HPP
