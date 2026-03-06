#include "mqtt_esp32.hpp"
#include <ArduinoJson.h>

MqttEsp32::MqttEsp32(const char* broker_host, int broker_port, const char* client_id)
    : broker_host_(broker_host),
      broker_port_(broker_port),
      client_id_(client_id),
      wifi_client_(),
      mqtt_client_(wifi_client_) {
    Serial.println("[MQTT] MqttEsp32 instance created");
}

MqttEsp32::~MqttEsp32() {
    disconnect();
}

bool MqttEsp32::connect() {
    if (isConnected()) {
        Serial.println("[MQTT] Already connected");
        return true;
    }
    
    Serial.print("[MQTT] Connecting to ");
    Serial.print(broker_host_.c_str());
    Serial.print(":");
    Serial.println(broker_port_);
    
    mqtt_client_.setId(client_id_.c_str());
    
    // Attempt connection
    if (!mqtt_client_.connect(broker_host_.c_str(), broker_port_)) {
        int error = mqtt_client_.connectError();
        Serial.print("[MQTT] ❌ Connection failed with error code: ");
        Serial.println(error);
        return false;
    }
    
    Serial.println("[MQTT] ✅ Connected to broker");
    printer_data_.is_connected = true;
    return true;
}

bool MqttEsp32::disconnect() {
    if (isConnected()) {
        mqtt_client_.stop();
        printer_data_.is_connected = false;
        Serial.println("[MQTT] Disconnected from broker");
        return true;
    }
    return false;
}

bool MqttEsp32::isConnected() const {
    return mqtt_client_.connected();
}

void MqttEsp32::subscribe(const char* topic) {
    if (!isConnected()) {
        Serial.println("[MQTT] ⚠️  Not connected, cannot subscribe");
        return;
    }
    
    Serial.print("[MQTT] Subscribing to: ");
    Serial.println(topic);
    mqtt_client_.subscribe(topic);
}

void MqttEsp32::update() {
    if (!isConnected()) {
        return;
    }
    
    // Poll for incoming messages
    int messageSize = mqtt_client_.parseMessage();
    if (messageSize > 0) {
        onMessageReceived(messageSize);
    }
}

void MqttEsp32::onMessageReceived(int messageSize) {
    String topic = mqtt_client_.messageTopic();
    
    // Read message payload into string
    String payload = "";
    while (mqtt_client_.available()) {
        payload += (char)mqtt_client_.read();
    }
    
    Serial.print("[MQTT] Message arrived on topic: ");
    Serial.println(topic);
    Serial.print("[MQTT] Payload length: ");
    Serial.println(payload.length());
    
    // Parse JSON and populate printer data
    parseJsonMessage(payload);
    
    // Invoke user callback if registered
    if (data_callback_) {
        data_callback_(printer_data_);
    }
}

void MqttEsp32::parseJsonMessage(const String& json_str) {
    // Create JSON document (512 bytes should be enough for printer data)
    StaticJsonDocument<512> doc;
    
    // Deserialize JSON
    DeserializationError error = deserializeJson(doc, json_str);
    
    if (error) {
        Serial.print("[JSON] ❌ Parse error: ");
        Serial.println(error.c_str());
        return;
    }
    
    Serial.println("[JSON] ✅ Parsed successfully");
    
    // Extract temperature data
    if (doc.containsKey("nozzle_temper")) {
        printer_data_.nozzle_temp = doc["nozzle_temper"].as<float>();
    }
    if (doc.containsKey("nozzle_target_temper")) {
        printer_data_.nozzle_target_temp = doc["nozzle_target_temper"].as<float>();
    }
    if (doc.containsKey("bed_temper")) {
        printer_data_.bed_temp = doc["bed_temper"].as<float>();
    }
    if (doc.containsKey("bed_target_temper")) {
        printer_data_.bed_target_temp = doc["bed_target_temper"].as<float>();
    }
    if (doc.containsKey("chamber_temper")) {
        printer_data_.chamber_temp = doc["chamber_temper"].as<float>();
    }
    
    // Extract print progress
    if (doc.containsKey("print")) {
        JsonObject print_obj = doc["print"].as<JsonObject>();
        if (print_obj.containsKey("progress")) {
            printer_data_.print_progress = print_obj["progress"].as<uint16_t>();
        }
        if (print_obj.containsKey("layer_num")) {
            printer_data_.current_layer = 
                String(print_obj["layer_num"].as<int>()).c_str();
        }
    }
    
    // Extract speed
    if (doc.containsKey("spd_mag")) {
        printer_data_.print_speed = doc["spd_mag"].as<float>();
    }
    
    // Extract Z height
    if (doc.containsKey("z_axis")) {
        printer_data_.z_height = doc["z_axis"].as<float>();
    }
    
    // Extract filament info
    if (doc.containsKey("filament_type")) {
        printer_data_.filament_type = doc["filament_type"].as<String>().c_str();
    }
    
    // Extract print stage and convert to state string
    if (doc.containsKey("mc_print_stage")) {
        int stage = doc["mc_print_stage"].as<int>();
        switch (stage) {
            case 0:
                printer_data_.gcode_state = "IDLE";
                break;
            case 1:
                printer_data_.gcode_state = "PRINTING";
                break;
            case 2:
                printer_data_.gcode_state = "PAUSED";
                break;
            default:
                printer_data_.gcode_state = "UNKNOWN";
                break;
        }
    }
    
    // Extract light status
    if (doc.containsKey("lights_report")) {
        JsonArray lights = doc["lights_report"].as<JsonArray>();
        if (lights.size() > 0 && lights[0].containsKey("mode")) {
            printer_data_.light_on = (lights[0]["mode"].as<int>() != 0);
        }
    }
    
    // Update timestamp
    printer_data_.last_update = millis();
    
    Serial.println("[MQTT] ✅ Printer data updated");
}
