1. changed to mqtt-backend-... filename mixup

push 1 done----------------------------------------------------------------------

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

push 2 done

doing pio build

got this error
Compiling .pio\build\adafruit_feather_esp32s3_reversetft\FrameworkArduino\Stream.cpp.o
Archiving .pio\build\adafruit_feather_esp32s3_reversetft\lib163\libWiFi.a
Compiling .pio\build\adafruit_feather_esp32s3_reversetft\FrameworkArduino\StreamString.cpp.o
Compiling .pio\build\adafruit_feather_esp32s3_reversetft\FrameworkArduino\Tone.cpp.o
lib/mqtt_esp32/mqtt_esp32.cpp: In member function 'bool MqttEsp32::isConnected() const':
lib/mqtt_esp32/mqtt_esp32.cpp:54:35: error: passing 'const MqttClient' as 'this' argument discards qualifiers [-fpermissive]
     return mqtt_client_.connected();
                                   ^
In file included from .pio/libdeps/adafruit_feather_esp32s3_reversetft/ArduinoMqttClient/src/ArduinoMqttClient.h:23,
                 from lib/mqtt_esp32/mqtt_esp32.hpp:5,
                 from lib/mqtt_esp32/mqtt_esp32.cpp:1:
.pio/libdeps/adafruit_feather_esp32s3_reversetft/ArduinoMqttClient/src/MqttClient.h:95:19: note:   in call to 'virtual uint8_t MqttClient::connected()'
   virtual uint8_t connected();
                   ^~~~~~~~~
Compiling .pio\build\adafruit_feather_esp32s3_reversetft\FrameworkArduino\USB.cpp.o
*** [.pio\build\adafruit_feather_esp32s3_reversetft\lib395\mqtt_esp32\mqtt_esp32.cpp.o] Error 1
================================================ [FAILED] Took 37.36 seconds ================================================
PS C:\Users\Mohammed Ibrahim\e\qttp\featurebranch\esp32_basefeature\esp32_base> 


i also setup diagram.json and wokwi.toml

erorr from const mismatch in wrapper




there is a known issue

build_flags =
    -DARDUINO_USB_CDC_ON_BOOT=1
    -DARDUINO_USB_MODE=1

when using hardware




when wokwi simulation,
build_flags =
    -DARDUINO_USB_CDC_ON_BOOT=0
    -DARDUINO_USB_MODE=0
GitHub Issue # 846 (Wokwi), ESP32-S3 USB initialization in Wokwi causes the USB interrupt to hang forever and never clear https://github.com/wokwi/wokwi-features/issues/846



suggestion:

two separate envs in pio , one with build flags on 1 (for mcu plugged in) one with
build flags on 0 for virtual testing



env switch fixed, but wokwi uses wokwi-guest as the wifi inside its env

theres a build flag you can add to env that fixes this issue
-DWOKWI_SIMULATION=1
added that flag to env-wokwi
-DWOKWI_SIMULATION=1 and -D WOKWI_BUILD apparently do the same thing so both can be used




1. since switching to wokwi-wifi, it is important to use a public mqtt broker because local setups break since wokwi runs in hte cloud
wokwi is cloud based, so this approach needs to be implemented
therefore switch .hosts to broker.hivemq.com, consider changing topics too in case you don't want to 
listen on other topics that are titled device/+/report



fake_printer.py (publisher) - publisher must publish to the same broker
printer_scraper.py (subscriber) - subsriber must subscribe to the same broker
esp32 in wokwi - must connect to the broker, and then interact with publisher and subscriber

wokwi is in channel 6 - WiFi.begin("Wokwi-GUEST", "", 6); (snippet from wokwi docs)
we can add a sepcific in integrated sensors only for when wokwi testing happens



update - created a wrapper if block so wokwi virtual board uses its own config, this is because
esp32 s3 reverse has a display and other pinnings, but the wokwi board has nothing so its better to isolate
it for testing, and to use a different env for the usb itself
^ doing this requires me to change the build flags for the wokwi env back to =1 because it routes serial back to the usb cdc interface, which is what the wokwi terminal monitors
^ changing it to 1 broke it so doesnt work
use 0 instead, related to issue on github mentioned earlier

  "connections": [
    ["esp32:TX", "serial-monitor:RX", "", []],
    ["esp32:RX", "serial-monitor:TX", "", []]
  ]
}

is the same as


  "connections": [
    ["esp32:43", "serial-monitor:RX", "", []],
    ["esp32:44", "serial-monitor:TX", "", []]
  ]
} just uses gpio numbers instead



fake_printer.py (publisher) - publisher must publish to the same broker
printer_scraper.py (subscriber) - subsriber must subscribe to the same broker (i made this earlier to 
test if publisher in py worked)
esp32 in wokwi - SUBSCRIBER ONLY parses data from bambu (or fakeprinter)

> ESP32 in Wokwi subscribes to a topic → receives message from `fake_printer.py` → confirm it worked

- having serial output issues right now

instead im gonna > ESP32 in Wokwi subscribes to a topic → receives message from `fake_printer.py` → confirm it worked, but then write a method inside esp32 in the if block for ifdef wokwi build, have it 
publish to create a closed circular feedback look between fakeprinter and esp32


workflow:

fake_printer.py publishes to hivemq

esp32 subs to that and publishes (but publishes in esp32/debug event)

mosquitto_sub -h broker.hivemq.com -t "device/esp32/debug" - mosquito subscribes to the debug message from esp32

circular feedback loop

PS C:\Users\Mohammed Ibrahim> mosquitto_sub -h broker.hivemq.com -t "device/esp32/debug"
[ACK] Printer data received and parsed
[ACK] Printer data received and parsed
[ACK] Printer data received and parsed
[ACK] Printer data received and parsed
[ACK] Printer data received and parsed
[ACK] Printer data received and parsed
[ACK] Printer data received and parsed
[ACK] Printer data received and parsed
[ACK] Printer data received and parsed
[ACK] Printer data received and parsed
[ACK] Printer data received and parsed
[ACK] Printer data received and parsed
[ACK] Printer data received and parsed
[ACK] Printer data received and parsed
[ACK] Printer data received and parsed
[ACK] Printer data received and parsed
[ACK] Printer data received and parsed
[ACK] Printer data received and parsed
[ACK] Printer data received and parsed
[ACK] Printer data received and parsed
[ACK] Printer data received and parsed
[ACK] Printer data received and parsed


it works - serial output bug from github was the issue.


going forward, when we deploy in prod with a real usb, we will likely just have to publish
esp32/debug events, and then listen for those with mosquitto.

'was wokwi redundant'
no

WiFi config selection works — #ifdef WOKWI_BUILD correctly picks Wokwi-GUEST + HiveMQ
wifiSetup() with channel 6 works — connected successfully
MQTT connect + subscribe works — ArduinoMqttClient connects to a remote broker over the internet
JSON parsing works — fake_printer.py payload was deserialized and the callback fired
The #ifndef WOKWI_BUILD guards work — hardware code was skipped, no hangs

*its impossible to test this without the wokwi you need either Wokwi or real hardware to actually run the ESP32 firmware *

when mcu connected - use the real wifi info in config settings. for now, we got the listening events to
work with wokwi


summary:

open terminal
1. run fake_printer.py with py fake_printer.py --broker local/hivemq (hivemq if testing cpp esp32)
2. then start running wokwi sim in vscode with f1 -> wokwi: start simulation
3. mosquitto_sub -h broker.hivemq.com -t "device/esp32/debug" - mosquito subscribes to the debug message from esp32 wokwi debug publisher output
you should see it publishing

i saw it publishing, so we should be good. more debug might be necessary when we're using the mcu


