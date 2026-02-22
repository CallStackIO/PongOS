#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "intdefs.h"

#define SCRN_WIDTH 320
#define SCRN_HEIGHT 200

// Draw a pixel to the screen
void putpixel(uint16_t x, uint16_t y, uint8_t color);

// Draw a rectangle to the screen
void drawrect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t color);

// The above functions are lies, they draw nothing to the screen
// They actually draw it to a 64K buffer, and this function copies that buffer to video memory
void copybuf(void);

// Clear the buffer
void clearbuf(void);

#endif