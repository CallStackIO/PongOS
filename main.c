#include "graphics.h"
#include "io.h"
#include "pong.h"

extern void load_interrupts32(void);

// We are finally in C-land
void main(void) {
    // Before we start the main game loop we need to get the keyboard working
    // In order to do that we need to enable the PIC 8259 interrupts
    remap_PIC();

    // Before we enable any interrupts, we need to do set up the IDT
    load_interrupts32();

    // Now we can enable interrupts
    __asm__("sti");

    // Finally, start the game loop
    game();
}