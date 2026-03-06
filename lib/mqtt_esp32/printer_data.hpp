#ifndef PRINTER_DATA_HPP
#define PRINTER_DATA_HPP

#include <string>
#include <cstdint>

/**
 * @struct PrinterData
 * @brief Parsed printer status from Bambu Lab printer MQTT messages
 * Arduino-compatible version (uses Serial instead of iostream)
 */
struct PrinterData {
    // Connection Status
    bool is_connected = false;
    
    // Printer State
    std::string printer_status;  // "idle", "printing", "paused", "error", etc.
    std::string gcode_state;     // "PRINTING", "IDLE", etc.
    
    // Temperature (°C)
    float nozzle_temp = 0.0f;
    float nozzle_target_temp = 0.0f;
    float bed_temp = 0.0f;
    float bed_target_temp = 0.0f;
    float chamber_temp = 0.0f;
    
    // Print Progress
    uint16_t print_progress = 0;  // Percentage 0-100
    uint32_t print_time = 0;      // Elapsed time in seconds
    uint32_t remaining_time = 0;  // Estimated remaining time in seconds
    
    // Print Details
    std::string current_layer;    // Current layer info
    float print_speed = 0.0f;     // Print speed percentage
    std::string filament_color;   // Filament color if available
    std::string filament_type;    // e.g., "PLA"
    
    // Mechanical
    float z_height = 0.0f;        // Current Z height in mm
    
    // Light status
    bool light_on = false;
    
    // Timestamp
    uint32_t last_update = 0;     // System time when data was last updated
    
    /**
     * @brief Reset all fields to default values
     */
    void reset() {
        printer_status = "";
        gcode_state = "";
        nozzle_temp = 0.0f;
        nozzle_target_temp = 0.0f;
        bed_temp = 0.0f;
        bed_target_temp = 0.0f;
        chamber_temp = 0.0f;
        print_progress = 0;
        print_time = 0;
        remaining_time = 0;
        current_layer = "";
        print_speed = 0.0f;
        filament_color = "";
        filament_type = "";
        z_height = 0.0f;
        light_on = false;
    }
    
    /**
     * @brief Print formatted printer data to Serial (Arduino-compatible)
     */
    void print() const {
        Serial.println("=== Printer Status ===");
        Serial.print("Status: ");
        Serial.print(printer_status.c_str());
        Serial.print(" (");
        Serial.print(gcode_state.c_str());
        Serial.println(")");
        Serial.print("Progress: ");
        Serial.print(print_progress);
        Serial.println("%");
        Serial.print("Nozzle: ");
        Serial.print(nozzle_temp);
        Serial.print("°C -> ");
        Serial.print(nozzle_target_temp);
        Serial.println("°C");
        Serial.print("Bed: ");
        Serial.print(bed_temp);
        Serial.print("°C -> ");
        Serial.print(bed_target_temp);
        Serial.println("°C");
        Serial.print("Chamber: ");
        Serial.print(chamber_temp);
        Serial.println("°C");
        Serial.print("Z Height: ");
        Serial.print(z_height);
        Serial.println("mm");
        Serial.print("Speed: ");
        Serial.print(print_speed);
        Serial.println("%");
        Serial.print("Filament: ");
        Serial.print(filament_type.c_str());
        Serial.print(" (");
        Serial.print(filament_color.c_str());
        Serial.println(")");
        Serial.print("Light: ");
        Serial.println(light_on ? "ON" : "OFF");
        Serial.println("=======================");
    }
};

#endif // PRINTER_DATA_HPP
