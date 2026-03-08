#include "integrated_sensors_wifi.hpp"

void batterySetup(Adafruit_MAX17048 &battery) {
    battery.begin();
}

void buttonSetup() {
    pinMode(0, INPUT_PULLUP);
    pinMode(1, INPUT_PULLDOWN);
    pinMode(2, INPUT_PULLDOWN);
}

bool wifiSetup(const char* ssid, const char* password) {
    WiFi.mode(WIFI_STA);
    
#ifdef WOKWI_BUILD
    // Wokwi-GUEST broadcasts on channel 6 - specify to skip WiFi scan and connect faster
    WiFi.begin(ssid, password, 6);
#else
    // Real hardware - auto-scan for available WiFi channels
    WiFi.begin(ssid, password);
#endif

    // Attempt 20 times to connect to the wireless network:
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        attempts++;
    }

    // Report Status into the Bool Objecet:
    if (WiFi.status() == WL_CONNECTED) {
         return true;
    } else {
        return false;
    }
}