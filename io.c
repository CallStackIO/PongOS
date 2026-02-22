#include "io.h"

uint8_t inb(uint16_t src) {
    uint8_t ret;
    __asm__ volatile ("mov %1, %%DX\n"
        "inb %%DX\n"
        "mov %%AL, %0"
        : "=r" (ret)
        : "r" (src)
        : "dx");
    return ret;
}

void outb(uint16_t dst, uint8_t val) {
    __asm__ volatile ("mov %0, %%DX\n"
        "mov %1, %%AL\n"
        "outb %%AL, %%DX"
        :
        : "r" (dst), "r" (val)
        : "dx", "al");
}

void io_wait(void) {
    uint16_t t = 10000;
    while(t--);
}

// Shamelessly copied from a wiki page
#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

#define PIC_EOI		0x20		/* End-of-interrupt command code */

#define ICW1_ICW4	0x01		/* Indicates that ICW4 will be present */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */

#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

#define CASCADE_IRQ 2

__attribute__((optimize("O0")))
void PIC_sendEOI(void)
{
	outb(PIC1_COMMAND,PIC_EOI);
}

// Something weird is going on here when setting optimization above 0
__attribute__((optimize("O0")))
void remap_PIC(void) {
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, 0x20);                 // ICW2: Master PIC vector offset
	io_wait();
	outb(PIC2_DATA, 0x28);                 // ICW2: Slave PIC vector offset
	io_wait();
	outb(PIC1_DATA, 1 << CASCADE_IRQ);        // ICW3: tell Master PIC that there is a slave PIC at IRQ2
	io_wait();
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();
	
	outb(PIC1_DATA, ICW4_8086);               // ICW4: have the PICs use 8086 mode (and not 8080 mode)
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();

	// Unmask both PICs.
	outb(PIC1_DATA, 0xFD);
	outb(PIC2_DATA, 0xFF);
}

// Done copying

KeyState key_w = READ;
KeyState key_s = READ;
KeyState key_up_arrow = READ;
KeyState key_down_arrow = READ;

void set_keys(uint8_t keycode) {
    switch (keycode) {
        case 0x11: // W down
            key_w = KEY_DOWN;
            break;
        case 0x1F: // S down
            key_s = KEY_DOWN;
            break;
        case 0x91: // W up
            key_w = KEY_UP;
            break;
        case 0x9F: // S up
            key_s = KEY_UP;
            break;
        case 0x48: // Up arrow down
            key_up_arrow = KEY_DOWN;
            break;
        case 0x50: // Down arrow down
            key_down_arrow = KEY_DOWN;
            break;
        case 0xC8: // Up arrow up
            key_up_arrow = KEY_UP;
            break;
        case 0xD0: // Down arrow up
            key_down_arrow = KEY_UP;
            break;
        default:
    }
}

void handle_keyboard_isr(void) {
    // Read keyboard status and check low bit
    // If bit 0 is set, there is some data to read.
    // I dont know why we check this, if we hit an interrupt then obviusly there is some data to read no?
    uint8_t status;
    uint8_t keycode;

    status = inb(0x64);
    if (status & 0x1) {
        // Read keycode
        keycode = inb(0x60);
        set_keys(keycode);
    }




    PIC_sendEOI();
}

void wait_vblank(void) {
    while (true) {
        uint8_t status1 = inb(0x3DA);
        if (status1 & (1 << 3)) {
            break;
        }
    }
}