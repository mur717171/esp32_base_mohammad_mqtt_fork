#include "mqtt_client.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <sstream>
#include <cstring>
#include <thread>
#include <chrono>
#include <ctime>

using json = nlohmann::json;

// Static wrapper callbacks
int PrinterMqttClient::on_message_arrived_wrapper(void* context, char* topicName, 
                                                   int topicLen, MQTTClient_message* message) {
    // Cast context back to the C++ instance
    PrinterMqttClient* pThis = static_cast<PrinterMqttClient*>(context);
    if (pThis && message) {
        pThis->on_message_arrived(topicName, topicLen, message);
        MQTTClient_freeMessage(&message);
    }
    return 1;
}

void PrinterMqttClient::on_connection_lost_wrapper(void* context, char* cause) {
    PrinterMqttClient* pThis = static_cast<PrinterMqttClient*>(context);
    if (pThis) {
        pThis->on_connection_lost(cause);
    }
}

void PrinterMqttClient::on_delivery_complete_wrapper(void* context, MQTTClient_deliveryToken token) {
    PrinterMqttClient* pThis = static_cast<PrinterMqttClient*>(context);
    if (pThis) {
        pThis->on_delivery_complete(token);
    }
}

PrinterMqttClient::PrinterMqttClient(
    const std::string& id,
    const std::string& host,
    int port,
    const std::string& username,
    const std::string& password,
    bool use_tls)
    : host_(host),
      port_(port),
      username_(username),
      password_(password),
      use_tls_(use_tls),
      is_connected_(false),
      client_(nullptr) {
    
    std::cerr << "DEBUG: Constructor entry\n";
    std::cerr.flush();
    
    // Build broker URL
    std::string broker_url = "tcp://" + host_ + ":" + std::to_string(port_);
    if (use_tls_) {
        broker_url = "ssl://" + host_ + ":" + std::to_string(port_);
    }
    
    std::cerr << "DEBUG: Broker URL: " << broker_url << "\n";
    std::cerr.flush();
    
    std::cerr << "DEBUG: About to create MQTT C client\n";
    std::cerr.flush();
    
    try {
        // Create MQTT client using C API
        int rc = MQTTClient_create(&client_, broker_url.c_str(), id.c_str(),
                                    MQTTCLIENT_PERSISTENCE_NONE, nullptr);
        
        if (rc != MQTTCLIENT_SUCCESS) {
            throw std::runtime_error("Failed to create MQTT client: error code " + std::to_string(rc));
        }
        
        // Register callbacks with 'this' pointer as context
        rc = MQTTClient_setCallbacks(
            client_, 
            this,  // Pass 'this' pointer as context
            on_connection_lost_wrapper,
            on_message_arrived_wrapper,
            on_delivery_complete_wrapper
        );
        
        if (rc != MQTTCLIENT_SUCCESS) {
            throw std::runtime_error("Failed to set MQTT callbacks: error code " + std::to_string(rc));
        }
        
        std::cerr << "✓ MQTT C client created successfully\n";
        std::cerr << "📡 MQTT Client initialized: " << id << " on " << broker_url << std::endl;
        std::cerr.flush();
    } catch (const std::exception& e) {     
        std::cerr << "❌ Exception during client creation: " << e.what() << std::endl;
        std::cerr.flush();
        throw;
    } catch (...) {
        std::cerr << "❌ Unknown exception during client creation\n";
        std::cerr.flush();
        throw;
    }
}

PrinterMqttClient::~PrinterMqttClient() {
    disconnect();
    if (client_ != nullptr) {
        MQTTClient_destroy(&client_);
    }
}

bool PrinterMqttClient::connect() {
    std::cerr << "🔗 Connecting to " << host_ << ":" << port_ << "..." << std::endl;
    std::cerr.flush();

    try {
        if (client_ == nullptr) {
            std::cerr << "❌ Client not initialized\n";
            return false;
        }
        
        MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
        conn_opts.keepAliveInterval = 20;
        conn_opts.cleansession = 1;
        
        // Set credentials if provided
        if (!username_.empty()) {
            conn_opts.username = username_.c_str();
            if (!password_.empty()) {
                conn_opts.password = password_.c_str();
            }
        }
        
        // Connect to broker
        int rc = MQTTClient_connect(client_, &conn_opts);
        
        if (rc != MQTTCLIENT_SUCCESS) {
            std::cerr << "❌ Connection failed: error code " << rc << std::endl;
            std::cerr.flush();
            return false;
        }
        
        is_connected_ = true;
        std::cerr << "✓ Connected to MQTT broker\n";
        std::cerr.flush();
        
        // Wait for connection to fully establish
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "❌ Connection failed: " << e.what() << std::endl;
        std::cerr.flush();
        return false;
    }
}

bool PrinterMqttClient::disconnect() {
    try {
        if (client_ != nullptr) {
            int rc = MQTTClient_disconnect(client_, 10000);
            if (rc != MQTTCLIENT_SUCCESS) {
                std::cerr << "⚠️  Disconnect error code: " << rc << std::endl;
            }
            is_connected_ = false;
        }
    } catch (const std::exception& e) {
        std::cerr << "❌ Disconnect error: " << e.what() << std::endl;
        std::cerr.flush();
    }
    return true;
}

int PrinterMqttClient::loop(int timeout_ms) {
    if (client_ == nullptr) {
        return -1;
    }
    
    // Use MQTTClient_yield to process network I/O and trigger callbacks
    try {
        MQTTClient_yield();
    } catch (...) {
        // Ignore errors
    }
    
    return 0;
}

void PrinterMqttClient::subscribe_to_topic(const std::string& topic) {
    if (!is_connected_) {
        throw std::runtime_error("Not connected to MQTT broker");
    }
    
    if (client_ == nullptr) {
        throw std::runtime_error("MQTT client not initialized");
    }
    
    int rc = MQTTClient_subscribe(client_, topic.c_str(), 1);
    
    if (rc != MQTTCLIENT_SUCCESS) {
        throw std::runtime_error("Failed to subscribe to topic '" + topic + "': error " + std::to_string(rc));
    }
    
    std::cerr << "✓ Subscribed to: " << topic << std::endl;
    std::cerr.flush();
}

// Instance method implementations
void PrinterMqttClient::on_message_arrived(char* topicName, int topicLen, MQTTClient_message* message) {
    // Copy payload immediately to ensure it remains valid after message is freed by wrapper
    std::string payload_copy(static_cast<char*>(message->payload), message->payloadlen);
    std::string topic_copy(topicName, topicLen);
    handle_message(topic_copy.c_str(), payload_copy.c_str(), payload_copy.length());
}

void PrinterMqttClient::on_connection_lost(const char* cause) {
    std::cerr << "⚠️  MQTT connection lost: " << (cause ? cause : "unknown reason") << std::endl;
}

void PrinterMqttClient::on_delivery_complete(MQTTClient_deliveryToken token) {
    // Used for publish callbacks, not subscription
}

void PrinterMqttClient::handle_message(const char* topic, const char* payload, int payload_len) {
    try {
        std::string json_str(payload, payload_len);
        
        std::cerr << "\n📨 Message from topic: " << topic << std::endl;
        std::cerr << "📦 Payload: " << json_str << std::endl;  // Debug: verify payload integrity
        std::cerr.flush();
        
        parse_printer_data(json_str);
    } catch (const std::exception& e) {
        std::cerr << "❌ Error processing message: " << e.what() << std::endl;
        std::cerr.flush();
    }
}

void PrinterMqttClient::parse_printer_data(const std::string& json_payload) {
    try {
        json data = json::parse(json_payload);
        
        printer_data_.last_update = static_cast<uint32_t>(time(nullptr));
        
        // Extract printer state information
        if (data.contains("print")) {
            auto& print_obj = data["print"];
            if (print_obj.contains("progress")) {
                printer_data_.print_progress = print_obj["progress"];
            }
            if (print_obj.contains("layer_num")) {
                printer_data_.current_layer = std::to_string(print_obj["layer_num"].get<int>());
            }
        }
        
        // Extract temperatures
        if (data.contains("nozzle_temper")) {
            printer_data_.nozzle_temp = data["nozzle_temper"];
        }
        if (data.contains("nozzle_target_temper")) {
            printer_data_.nozzle_target_temp = data["nozzle_target_temper"];
        }
        if (data.contains("bed_temper")) {
            printer_data_.bed_temp = data["bed_temper"];
        }
        if (data.contains("bed_target_temper")) {
            printer_data_.bed_target_temp = data["bed_target_temper"];
        }
        if (data.contains("chamber_temper")) {
            printer_data_.chamber_temp = data["chamber_temper"];
        }
        
        // Extract printer status
        if (data.contains("print_type")) {
            printer_data_.gcode_state = data["print_type"];
        }
        if (data.contains("mc_print_stage")) {
            int stage = data["mc_print_stage"];
            if (stage == 0) {
                printer_data_.printer_status = "idle";
            } else if (stage == 1) {
                printer_data_.printer_status = "printing";
            } else if (stage == 2) {
                printer_data_.printer_status = "paused";
            } else {
                printer_data_.printer_status = "unknown";
            }
        }
        
        // Extract other data
        if (data.contains("spd_mag")) {
            printer_data_.print_speed = data["spd_mag"];
        }
        if (data.contains("z_axis")) {
            printer_data_.z_height = data["z_axis"];
        }
        if (data.contains("filament_type")) {
            printer_data_.filament_type = data["filament_type"];
        }
        if (data.contains("lights_report")) {
            auto& lights = data["lights_report"];
            if (lights.is_array() && lights.size() > 0) {
                if (lights[0].contains("mode")) {
                    printer_data_.light_on = (lights[0]["mode"] != 0);
                }
            }
        }
        
        printer_data_.is_connected = true;
        
        std::cerr << "✓ Parsed printer data:\n";
        std::cerr.flush();
        printer_data_.print();
        
        if (data_callback_) {
            data_callback_(printer_data_);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ JSON parsing error: " << e.what() << std::endl;
        std::cerr.flush();
    }
}
