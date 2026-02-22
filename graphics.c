#include "graphics.h"
#include "io.h"

volatile uint8_t* video_memory = (uint8_t*)0xA0000;
uint8_t* buffer = (uint8_t*)0x60000;

void putpixel(uint16_t x, uint16_t y, uint8_t color) {
    volatile uint8_t* location = buffer + y * SCRN_WIDTH + x;
    *location = color;
}

void clearbuf(void) {
    for (int i = 0; i < SCRN_HEIGHT*SCRN_WIDTH; i++) {
        buffer[i] = 0;
    }
}

void copybuf(void) {
    fastcpy(video_memory, buffer, SCRN_HEIGHT*SCRN_WIDTH/4);
}

void drawrect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t color) {
    if (x1 > x2 || y1 > y1) return;

    for (int i = 0; i + x1 <= x2; i++) {
        for (int j = 0; j + y1 <= y2; j++) {
            putpixel(x1 + i, y1 + j, color);
        }
    }
}