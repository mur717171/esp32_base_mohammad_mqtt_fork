#include "display_pages.hpp"

void showDebugPage(Adafruit_ST7789 &display, GFXcanvas16 &canvas, Adafruit_MAX17048 &battery, uint8_t orientation) {
    canvas.setFont(NULL);
    canvas.setRotation(orientation);
    canvas.fillScreen(ST77XX_BLACK);
    canvas.setCursor(0, 17);
    canvas.setTextColor(ST77XX_RED);
    canvas.println("Debug Page: ");

    // Network Info:
    canvas.setTextColor(ST77XX_YELLOW);
    canvas.print("IP Address: ");
    canvas.println(WiFi.localIP().toString());
    canvas.setTextColor(ST77XX_GREEN); 

    // Battery Info:
    canvas.print("Battery Voltage (V): ");
    canvas.println(battery.cellVoltage());
    canvas.print("Battery Percent (%): ");
    canvas.println(battery.cellPercent());

    // Button Output:
    canvas.setTextColor(ST77XX_WHITE);
    canvas.println("");
    canvas.print("Buttons: ");
    Serial.println(digitalRead(0));
    Serial.println(digitalRead(1));
    Serial.println(digitalRead(2));
    if (!digitalRead(0)) {
      canvas.print("D0, ");
    }
    if (digitalRead(1)) {
      canvas.print("D1, ");
    }
    if (digitalRead(2)) {
      canvas.print("D2, ");
    }
}

void swapPages(uint8_t &current_page, const uint8_t num_pages) {
  static bool last = HIGH;
  bool now = digitalRead(0);
  
  if (now != last && (last = now) == LOW) {
    current_page = (current_page % num_pages) + 1;
  }
}

void swapOrientation(uint8_t &current_orientation) {
   static bool last = LOW;
  bool now = digitalRead(2);
  
  if (now != last && (last = now) == HIGH) {
    current_orientation = (current_orientation % 4) + 1;
  }
}

void showExamplePage(Adafruit_ST7789 &display, GFXcanvas16 &canvas){
    canvas.setFont(&FreeMono9pt7b);
    // Screen can take any ST77XX Color:
    canvas.fillScreen(ST77XX_BLACK);
    // The starting position for the typesetting matrix can be set to an arbitrary pixel position:
    canvas.setCursor(0,17);
    // The textcolor can take any ST77XX Color:
    canvas.setTextColor(ST77XX_WHITE);
    // canvas.print can take a wide variety of objects given they can be converted to a string.
    canvas.println("Example Page");
    // canvas.println prints a newline after the printed string.
    canvas.print("Example Sensor:"); 
    // Presumably the sensor has some method, you may have to swap the type:
    // TYPE data = generic_device.getdata()
    // String data = data.tostr()
    // canvas.println("data")
}
