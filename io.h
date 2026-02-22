#ifndef IO_H
#define IO_H

#include "intdefs.h"

typedef enum {
    READ,
    KEY_DOWN,
    KEY_UP
} KeyState;

extern KeyState key_w;
extern KeyState key_s;
extern KeyState key_up_arrow;
extern KeyState key_down_arrow;

// Wrapper for the IN instruction
uint8_t inb(uint16_t src);

// Wrapper for the OUT instruction
void outb(uint16_t dst, uint8_t val);

// Wait some time for IO to complete
void io_wait(void);

// Remap PIC interrupts to 0x20 and 0x28 and set the enable mask only for the kayboard interrupts
void remap_PIC(void);

// Wait for vblank
void wait_vblank(void);

// My implentation of memcpy 
extern void fastcpy(volatile void* dst, volatile void* src, uint16_t cnt); // cnt is in dwords

#endif