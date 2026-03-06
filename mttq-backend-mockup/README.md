

i generated this for my personal use i'll rewrite it later cuz information overload



# Bambu Lab Printer MQTT Client (C++ Standalone)

Standalone C++ implementation of printer data scraping from Bambu Lab printers via MQTT, ported from the working Python script.

## Overview

This client connects to a Bambu Lab X1C printer (or a local mosquitto broker that relays printer data) and:
- Subscribes to printer MQTT topics (`device/#`)
- Parses incoming JSON payloads
- Extracts key printer metrics (temperature, progress, status, speed, etc.)
- Provides data via C++ structures for easy integration

## Architecture

```
TCP/SSL Connection
        ↓
   MQTT Broker (mosquitto)  ← Python script publishes here
        ↓
  MQTT Client (C++)
        ↓
  JSON Parsing (nlohmann/json)
        ↓
  PrinterData struct
        ↓
  Display/Integration
```

## Prerequisites

### Windows Setup

1. **Visual Studio 2019+** or **MinGW** with C++17 support
2. **CMake 3.10+**
3. **vcpkg** (for dependency management):
   ```powershell
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   .\vcpkg\integrate install
   ```

### Install Dependencies via vcpkg

```powershell
vcpkg install mosquittopp:x64-windows
vcpkg install nlohmann-json:x64-windows
vcpkg install openssl:x64-windows
```

### Alternative: Manual Installation

- **libmosquitto-cpp**: http://mosquitto.org/download/
- **nlohmann/json**: Header-only library (included in some vcpkg installs)
- **OpenSSL**: Usually pre-installed on Windows or available via vcpkg

## Building

### Using CMake with vcpkg

```powershell
# Create build directory
mkdir build
cd build

# Configure with vcpkg toolchain
cmake .. -DCMAKE_TOOLCHAIN_FILE=<path-to-vcpkg>/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . --config Release

# Run
.\Release\mqtt_client.exe
# Or: .\Release\mqtt_client.exe 10.10.2.20 8883  (for direct printer connection)
```

## Configuration

Edit `src/main.cpp` to configure:

```cpp
// Line ~19-21 in main.cpp
std::string broker_host = "localhost";      // Change to mosquitto broker IP
int broker_port = 1883;                     // Use 1883 for plain MQTT, 8883 for TLS
std::string access_code = "bc245185";       // Your printer's 8-digit LAN code
```

Or pass via command line:
```powershell
.\mqtt_client.exe 10.10.2.20 8883
```

## Connection Modes

### 1. Local Mosquitto Broker (Recommended for Testing)
```
Python script → Mosquitto (localhost:1883) ← C++ client reads from here
```
Local broker has no auth, plain MQTT. Simple for development.

### 2. Direct to Printer
```
C++ client → Bambu X1C (10.10.2.20:8883)
```
Requires username: `bblp`, password: 8-digit LAN code, TLS/SSL enabled.

## Protocol Details

### MQTT Connection
- **Host**: `10.10.2.20` (Bambu printer IP) or `localhost` (mosquitto broker)
- **Port**: `8883` (printer, TLS) or `1883` (broker, plain)
- **Username**: `bblp`
- **Password**: 8-digit access code (e.g., `bc245185`)
- **Topic Filter**: `device/#` (subscribes to all device status messages)
- **TLS**: Self-signed cert allowed (like Python implementation)

### Payload Example
```json
{
  "nozzle_temper": 220.5,
  "nozzle_target_temper": 220,
  "bed_temper": 60.2,
  "bed_target_temper": 60,
  "chamber_temper": 45.1,
  "print": {
    "progress": 35,
    "layer_num": 42
  },
  "spd_mag": 100,
  "z_axis": 12.34,
  "filament_type": "PLA",
  "mc_print_stage": 1
}
```

## Output

When connected, the client displays:
```
✅ CONNECTED to Bambu Lab printer MQTT broker
📥 Subscribed to device/#

📨 Message from topic: device/abc123def456/report
✓ Parsed printer data:
=== Printer Status ===
Status: printing (PRINTING)
Progress: 35%
Nozzle: 220.5°C -> 220°C
Bed: 60.2°C -> 60°C
Chamber: 45.1°C
Z Height: 12.34mm
Speed: 100%
Print Time: 3600s | Remaining: 2400s
Filament: PLA (Black)
Light: ON
=======================
```

## Integration into ESP32

Once tested and verified with a real printer:

1. Copy `include/printer_data.hpp` and core parsing logic to ESP32 project
2. Replace libmosquitto-cpp with Arduino MQTT library (e.g., `PubSubClient`)
3. Replace nlohmann/json with Arduino JSON library (e.g., `ArduinoJson`)
4. Port into `platform_interface.cpp` in the main ESP32 firmware

## Testing Checklist

- [ ] Start local mosquitto broker: `mosquitto` (or `mosquitto.exe` on Windows)
- [ ] Start Python printer_scraper.py connecting to local broker
- [ ] Verify Python script prints data every 1-2 seconds
- [ ] Start C++ client: `mqtt_client.exe localhost 1883`
- [ ] Verify C++ client receives and parses same data
- [ ] Compare output between Python and C++ side-by-side
- [ ] Test reconnection handling (kill broker, restart)
- [ ] Test direct connection to real Bambu printer (if available)

## Troubleshooting

| Issue | Solution |
|-------|----------|
| "Cannot find mosquittopp" | Install via vcpkg or set CMake find path |
| "nlohmann/json not found" | Install via vcpkg or use `-DCMAKE_PREFIX_PATH` |
| Connection fails with "Authentication error" | Verify username/password and access code |
| "Self-signed certificate error" | TLS insecure mode is enabled; if not working, check OpenSSL |
| "Connection refused" | Verify broker IP/port, check if mosquitto is running |
| "No messages received" | Check topic subscription, verify Python script is publishing |

## References

- Bambu Lab X1C Specs: https://en.bambulab.com/products/x1-carbon
- Mosquitto Documentation: https://mosquitto.org/man/mosquitto-8.html
- libmosquitto C++ Bindings: https://github.com/eclipse/mosquitto
- nlohmann/json: https://json.nlohmann.me/

---

**Version**: 1.0  
**Status**: Standalone component - ready for ESP32 integration
