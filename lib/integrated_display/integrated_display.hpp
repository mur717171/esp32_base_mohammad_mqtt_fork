#include <Adafruit_ST7789.h>
#include <Adafruit_NeoPixel.h>

#ifndef fablab_integrated_display_hpp
#define fablab_integrated_display_hpp 

/**
 * @brief Abstraction to set up the Adafruit Integrated Display.
 * @param display Adafruit_ST7789 Reference
 * @param canvas GFXcanvas16 Reference
 * @param font The desired display font
 */
void displaySetup(Adafruit_ST7789 &display, GFXcanvas16 &canvas, const GFXfont* font);

/**
 * @brief Abstraction to refrsh the Adafruit Integrated Display.
 * @param display Adafruit_ST7789 Reference
 * @param canvas GFXcanvas16 Reference
 */
void refreshDisplay(Adafruit_ST7789 &display, GFXcanvas16 &canvas);

/**
 * @brief Abstraction to set up the Adafruit Integrated Neopixel LED.
 * @param neopixel Adafruit_Neopixel Reference
 * @param brightness Neopixel brightness (0-100)
 */
void neopixelSetup(Adafruit_NeoPixel &neopixel, const uint8_t brightness);


#endif