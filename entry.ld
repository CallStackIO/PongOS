# Linker script for PongOS
# By Sullivan DeVries



MEMORY {
    BOOT_CODE (rx) : org = 0x7C00, len = 512
    MAIN_MEMORY (rwx) : org = 0x7E00, len = 0x781FF
}

SECTIONS {
    /DISCARD/ : {
        *(.note.*)
        *(.comment)
    }
    .text.start : {
        bootx86.o (.text)
    } > BOOT_CODE
    .text : {
        *(.text)
        *(.text.*)
    } > MAIN_MEMORY
    .bss : {
        *(.bss)
        *(.bss.*)
    } > MAIN_MEMORY
    .data : {
        *(.data)
        *(.data.*)
    } > MAIN_MEMORY
}

text_start = ADDR(.text);
text_end = text_start + SIZEOF(.text);

bss_start = ADDR(.bss);
bss_end = bss_start + SIZEOF(.bss);

data_start = ADDR(.data);
data_end = data_start + SIZEOF(.data);

STARTUP(bootx86.o)