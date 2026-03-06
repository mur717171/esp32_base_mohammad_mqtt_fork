1. changed to mqtt-backend-... filename mixup

push 1 done

currently: paho c lib + c++ wrapper parsesbambu mqtt - backend to merge

    this is not compatible with the esp32
    how to make compatible
    - std::thread - not needed
    - srd::cerr +iostream, replace with arduino serial
    - paho c library - replace with arduino 
    - desktop specific build stuff is not compatible w arduino build system
    need:
        convert to arduino-compatible mqtt module (arduino wrapper)
        potentially uses pubsubclient or asyncmqtt client (arduino libs)
        integrates w esp32's wifi
        just arduino serial for debug (no threading or iostream)
        arduino libs are lighter than paho (good)
        im thinking we switch to just arcuinomqttclient because it has tls built in, these other ones dont seem to use tls

    after research
        pubsubclient supports tls through wificlientsecure
        asyncmqttclient support tls thorugh asyncclientsecure
        arduinomqttclient should be obvious why

        im gonna use arduinomqttclient because official modern and native



    testing flow:

    dev: mock mosquitto + py scripts nontls until it works
    prod: go in lab and use it, dont need microcontroller bc its a c scraper that connects
    
    tldr: just swap broker host + load certificate because the backend works (in pre-repo testing worked fine)

    adding tls is a trivial step provided everything else works fine so i dont really care about that for now

   sketch
   Desktop/PC:
  └── mqtt-backend-mockup/
      ├── Paho C lib + C++ wrapper (desktop only)
      └── Python scripts (test publishers)
      
ESP32 Microcontroller:
  └── src/main.cpp + lib/mqtt_esp32/
      ├── ArduinoMqttClient (ESP32 only)
      ├── Adafruit libs (display, battery, WiFi)
      └── PrinterData struct (shared)


      bit of a problem with simulating it


      1. use wokwi to simulate the hardware




*configure wokwi.toml

esp32: adafruit libs




Desktop:
  mqtt-backend-mockup/     (Paho, C++, Python scripts, TLS, etc.)

ESP32 (Arduino):
  esp32_base/src/main.cpp
  esp32_base/lib/mqtt_esp32/          (wrapper around Arduino MQTT)
                mqtt_esp32.h/.cpp


pretty much

mqtt-backend-mockup/
  ├── Paho C lib + C++ wrapper (PrinterMqttClient)
  ├── Python scripts (fake_printer.py, printer_scraper.py)
  └── Real broker testing later

esp32_base/
  ├── src/main.cpp (Arduino sketch, integrates MQTT + display)
  ├── lib/mqtt_esp32/ (YOUR NEW MODULE)
  │   ├── mqtt_esp32.hpp (ArduinoMqttClient wrapper)
  │   └── mqtt_esp32.cpp
  ├── lib/integrated_display/ (existing)
  ├── lib/integrated_sensors_wifi/ (existing)
  └── platformio.ini (add ArduinoMqttClient dependency)

  better more expanded



tldr it goes in lib and src of adafruit




also add networkconfig.h, it makes it easier to punch in your network info

i was confused about config.json because of my work on frontend emulator - config.json in this project is redundant because everything is plug and 
play for pio already



integrated lib and src into adafruit dependencies, about to start testing. local first and then hardware sims on wokwi
