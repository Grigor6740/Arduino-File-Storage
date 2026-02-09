#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_WIDTH 128 
#define OLED_HEIGHT 32

extern Adafruit_SSD1306 oled;

void initOled();

#endif