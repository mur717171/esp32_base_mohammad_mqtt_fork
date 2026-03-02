#include <Adafruit_ST7789.h>
#include <Adafruit_MAX1704X.h>
#include <integrated_display.hpp>
#include <integrated_sensors_wifi.hpp>
#include <Fonts/FreeMono9pt7b.h>
#ifndef fablab_display_pages_hpp
#define fablab_display_pages_hpp

/**
 * @brief Shows the Debug Page with all information on the included display, good for troubleshooting.
 * @param display Reference to the Adafruit_ST7789 display object
 * @param canvas Reference to the GFXcanvas16 canvas object
 * @param battery Reference to the Adafruit_MAX17048 battery object (required for debug output)
 * @note Do not delete, do not modify, if you break the WiFi connection, this will tell you.
 */
void showDebugPage(Adafruit_ST7789 &display, GFXcanvas16 &canvas, Adafruit_MAX17048 &battery, uint8_t orientation);

/**
 * @brief Iterates the pages on the frotend loop().
 * @param current_page uint8_t for the current page
 * @param num_pages const uint8_t for the total number of pages
 * @note Do not delete, do not modify (unless necessary, it's easier just to mess with the current_page parameter)
 *  unless you know what you're doing.
 */
void swapPages(uint8_t &current_page, const uint8_t num_pages);

/**
 * @brief Iterates the orientations on the frotend loop().
 * @param current_page uint8_t for the current orientation
 * @note Do not delete, do not modify (unless necessary, it's easier just to mess with the current_page parameter)
 *  unless you know what you're doing.
 */
void swapOrientation(uint8_t &current_orientation);

/**
 * @brief Shows an example page that demonstrates the important page methods on the included display,
 * programmed to be able to handle the data for any generic I2C device provided an Adafruit driver exists for it.
 * @param display Reference to the Adafruit_ST7789 display oject
 * @param canvas Reference to the GFXcanvas16 canvas object
 * @param generic_device Some I2C_Device provided by Adafruit
 * @note Do not edit the reference function, make a copy before modifying.
 */
void showExamplePage(Adafruit_ST7789 &display, GFXcanvas16 &canvas);


#endif