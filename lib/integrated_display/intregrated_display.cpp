#include "integrated_display.hpp"


void displaySetup(Adafruit_ST7789 &display, GFXcanvas16 &canvas, const GFXfont *font) {
    display.init(135,240);
    display.setRotation(3);
    canvas.setFont(font);
    canvas.setTextColor(ST77XX_WHITE);
}

void refreshDisplay(Adafruit_ST7789 &display, GFXcanvas16 &canvas) {
    display.drawRGBBitmap(0, 0, canvas.getBuffer(), 240, 135);
    pinMode(TFT_BACKLITE, OUTPUT);
    digitalWrite(TFT_BACKLITE, HIGH);
}

void neopixelSetup(Adafruit_NeoPixel &neopixel, const uint8_t brightness) {
    neopixel.begin();
    neopixel.setBrightness(brightness);
    neopixel.show();
}
