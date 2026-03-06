#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include "mqtt_client.hpp"

// Global client pointer for signal handling
PrinterMqttClient* g_client = nullptr;

// Signal handler for clean shutdown
void signal_handler(int signal) {
    std::cerr << "\n⛔ Received signal " << signal << ", shutting down..." << std::endl;
    std::cerr.flush();
    if (g_client) {
        g_client->disconnect();
    }
    exit(0);
}

int main(int argc, char* argv[]) {
    std::cerr << "MAIN START\n";
    std::cerr.flush();
    std::cerr << "═══════════════════════════════════════════════════════════" << std::endl;
    std::cerr << "    Bambu Lab Printer MQTT Client (C++ Standalone)" << std::endl;
    std::cerr << "═══════════════════════════════════════════════════════════" << std::endl;
    std::cerr.flush();
    
    // Configuration
    std::string broker_host = "localhost";
    int broker_port = 1883;
    std::string username = "";
    std::string password = "";
    
    // Parse command line arguments if provided
    if (argc > 1) {
        broker_host = argv[1];
        std::cerr << "📝 Using custom broker: " << broker_host << std::endl;
    } else {
        std::cerr << "📝 Using default local broker: " << broker_host << ":" << broker_port << std::endl;
    }
    std::cerr.flush();
    
    if (argc > 2) {
        broker_port = std::stoi(argv[2]);
        std::cerr << "📝 Using custom port: " << broker_port << std::endl;
        std::cerr.flush();
    }
    
    // Force IPv4 for localhost
    if (broker_host == "localhost") {
        broker_host = "127.0.0.1";
        std::cerr << "🔧 Converted localhost to 127.0.0.1 for IPv4 compatibility" << std::endl;
        std::cerr.flush();
    }
    
    // Setup signal handlers for clean shutdown
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    
    // Create MQTT client
    std::cerr << "\n🔨 Initializing MQTT client..." << std::endl;
    std::cerr.flush();
    
    PrinterMqttClient client(
        "esp32_printer_client",
        broker_host,
        broker_port,
        username,
        password,
        false
    );
    
    g_client = &client;
    
    // Set optional callback for data updates
    client.set_data_callback([](const PrinterData& data) {
        std::cerr << "📊 Callback: Received printer data update" << std::endl;
        std::cerr.flush();
    });
    
    // Connect to broker
    std::cerr << "\n🌐 Attempting to connect to MQTT broker..." << std::endl;
    std::cerr.flush();
    if (!client.connect()) {
        std::cerr << "❌ Failed to initiate connection" << std::endl;
        std::cerr.flush();
        return 1;
    }
    
    // Give connection time to establish
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // Subscribe to printer data topics
    std::cerr << "\n📨 Subscribing to MQTT topics..." << std::endl;
    std::cerr.flush();
    try {
        client.subscribe_to_topic("device/+/telemetry");  // Subscribe to all printer telemetry
        client.subscribe_to_topic("device/+/event");      // Subscribe to printer events
        client.subscribe_to_topic("device/+/report"); //subscribe to fake_printer.py (our implementation of the c++ scraper)
        std::cerr << "✅ Subscriptions complete" << std::endl;
        std::cerr.flush();
    } catch (const std::exception& e) {
        std::cerr << "⚠️  Subscription error: " << e.what() << std::endl;
        std::cerr.flush();
    }
    
    // Main event loop
    std::cerr << "\n🔄 Entering main loop (Ctrl+C to exit)..." << std::endl;
    std::cerr << "Waiting for printer data..." << std::endl;
    std::cerr.flush();
    
    try {
        while (true) {
            std::cerr << "DEBUG: Loop Tick..." << std::endl;
            std::cerr.flush();
            
            int ret = client.loop(500);
            
            if (ret != 0) {
                std::cerr << "⚠️  Loop error: " << ret << std::endl;
                std::cerr.flush();
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception in main loop: " << e.what() << std::endl;
        std::cerr.flush();
        return 1;
    }
    
    return 0;
}
