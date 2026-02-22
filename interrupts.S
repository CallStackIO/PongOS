# Set up interrupts in x86-32 protected mode
# By Sullivan DeVries

.data
IDT_Table:
# IDT
.org 0
# Entry 0x8
.org 0x40
.word 0
.word (1 << 3)
.byte 0
.byte 0x8E
.word 0
# Entry 0xD
.org 0x68
.word 0
.word (1 << 3)
.byte 0
.byte 0x8E
.word 0
# Entry 0x20
.org 0x100
.word 0
.word (1 << 3)
.byte 0
.byte 0x8E
.word 0
# Entry 0x21
.org 0x108
.word 0
.word (1 << 3)
.byte 0
.byte 0x8E
.word 0

.org 2048

IDTR:
.word 2047
.long IDT_Table

# Writes the interrupt handler function addresses to the IDT and loads the IDTR
.text
.global load_interrupts32
load_interrupts32:
    push %EAX
    push %EBX
    push %ECX

    lea GPF_isr, %EAX
    lea IDT_Table, %EBX
    add $0x68, %EBX
    movw %AX, (%EBX)
    sar $16, %EAX
    add $6, %EBX
    movw %AX, (%EBX)

    lea DF_isr, %EAX
    lea IDT_Table, %EBX
    add $0x40, %EBX
    movw %AX, (%EBX)
    sar $16, %EAX
    add $6, %EBX
    movw %AX, (%EBX)

    lea irq0_isr, %EAX
    lea IDT_Table, %EBX
    add $0x100, %EBX
    movw %AX, (%EBX)
    sar $16, %EAX
    add $6, %EBX
    movw %AX, (%EBX)

    lea keyboard_isr, %EAX
    lea IDT_Table, %EBX
    add $0x108, %EBX
    movw %AX, (%EBX)
    sar $16, %EAX
    add $6, %EBX
    movw %AX, (%EBX)


    # Load the IDT
    lidt IDTR

    pop %ECX
    pop %EBX
    pop %EAX
    ret

.extern PIC_sendEOI
.extern handle_keyboard_isr

keyboard_isr:
    pusha
    push %EBP
    mov %ESP, %EBP
    call handle_keyboard_isr
    pop %EBP
    popa
    iretl

GPF_isr:
    jmp .
    iretl
DF_isr:
    jmp .
    iretl

irq0_isr:
    nop
    nop
    push %EBP
    mov %ESP, %EBP
    call PIC_sendEOI
    pop %EBP
    iretl


# My implemention of memcpy
.global fastcpy
fastcpy:
    push %EBP
    mov %ESP, %EBP
    push %EDI
    push %ESI
    mov 8(%EBP), %EDI
    mov 12(%EBP), %ESI
    mov 16(%EBP), %ECX
    rep movsl
    pop %ESI
    pop %EDI
    leave
    ret

