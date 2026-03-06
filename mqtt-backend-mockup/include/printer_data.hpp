#ifndef PRINTER_DATA_HPP
#define PRINTER_DATA_HPP

#include <iostream>
#include <string>
#include <cstdint>
#include <ctime>

/**
 * @struct PrinterData
 * @brief Parsed printer status from Bambu Lab printer MQTT messages
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
     * @brief Print formatted printer data to console
     */
    void print() const {
        std::cout << "=== Printer Status ===" << std::endl;
        std::cout << "Status: " << printer_status << " (" << gcode_state << ")" << std::endl;
        std::cout << "Progress: " << print_progress << "%" << std::endl;
        std::cout << "Nozzle: " << nozzle_temp << "°C -> " << nozzle_target_temp << "°C" << std::endl;
        std::cout << "Bed: " << bed_temp << "°C -> " << bed_target_temp << "°C" << std::endl;
        std::cout << "Chamber: " << chamber_temp << "°C" << std::endl;
        std::cout << "Z Height: " << z_height << "mm" << std::endl;
        std::cout << "Speed: " << print_speed << "%" << std::endl;
        std::cout << "Print Time: " << print_time << "s | Remaining: " << remaining_time << "s" << std::endl;
        std::cout << "Filament: " << filament_type << " (" << filament_color << ")" << std::endl;
        std::cout << "Light: " << (light_on ? "ON" : "OFF") << std::endl;
        std::cout << "======================" << std::endl;
    }
};

#endif // PRINTER_DATA_HPP
