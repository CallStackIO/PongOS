# Bootstrapping code for the rest of the program
# By Sullivan DeVries

.code16

.text
.global init
init:
    # Enter into protected mode
    cli # Disable interrupts

    # Check if A20 is enabled
    call checka20line
    test %AX, %AX
    jz .Ltryenablea20 # If A20 is not enabled, we need to try and enable it
    lea str_a20_is_enabled, %SI
    call printstr
    jmp .La20enabled

    .Ltryenablea20:
    # First try with int 15
    call enablea20int15
    call checka20line
    lea str_a20_enabled, %SI
    call printstr
    jmp .La20enabled

    .La20enabled:

    # Load GDT
    lgdt GDTR

    # Now we set the graphics mode to 13
    call setvgamode13

    # Enable protected mode
    mov %CR0, %EAX
    or 1, %AL
    mov %EAX, %CR0

    # Enter into protected mode code
    ljmp $0x8, $pmentry

# Checks if the A20 line is enabled
# Returns: AL = enabled/disabled
checka20line:
    # According to the intel I recieved, we need to test if the memory wraps around at 1Mib
    # If it does, A20 is clear if not A20 is set
    push %DS
    push %SI
    push %ES
    push %DI

    xor %AX, %AX
    mov %AX, %DS
    not %AX
    mov %AX, %ES
    mov $0x500, %SI # DS:SI = 0x0000:0500
    mov $0x510, %DI # ES:DI = 0xFFFF:0510
                     # Exactly 1 MiB apart

    # Save the values at these locations to be restored later
    mov %DS:(%SI), %AL
    push %AX
    mov %ES:(%DI), %AL
    push %AX

    # Write to the memory addresses
    movb $0x67, %DS:(%SI)
    movb $0x21, %ES:(%DI)

    # Read back the values
    mov %DS:(%SI), %AL
    
    # If the values at the memory addresses are the same, then A20 is disabled
    # A20 is enabled if we can actually write abouve 1MiB
    mov $1, %AH
    cmp %AL, %ES:(%DI)
    jne .La20exit
    dec %AH

.La20exit:
    pop %AX
    mov %AL, %ES:(%DI)
    pop %AX
    mov %AL, %ES:(%DI)
    pop %DI
    pop %ES
    pop %SI
    pop %DS
    ret

# Enable A20 line with bios interrupt 15
enablea20int15:
    push %AX
    mov $0x2401, %AX
    sti
    int $0x15
    cli
    pop %AX
    ret

# AL = Character to write
printc:
	push %AX
	push %BX
	mov $0xE, %AH
	mov $0, %BX
	int $0x10
	pop %BX
	pop %AX
	ret

# DS:SI Address of string
printstr:
    push %DS
    push %SI
    push %AX
	.Lloop:
		lodsb
		cmp $0, %AL
		je .Ldone
		call printc
		jmp .Lloop

	.Ldone:
        pop %AX
		pop %SI
		pop %DS
		ret

.global setvgamode13
setvgamode13:
    push %AX
    mov $0, %AH
    mov $0x13, %AL
    int $0x10
    pop %AX
    ret

.extern main

.code32
pmentry:
    # Set correct segment selectors
    mov $0x10, %AX
    mov %AX, %DS
    mov %AX, %ES
    mov %AX, %FS
    mov %AX, %GS
    mov %AX, %SS

    # GCC expects direction flag to be clear upon entering main (I think)
    cld
    
    # All that is left to do is call main
    call main

.data
str_a20_is_enabled: .string "\xF A20 is enabled\r\n"
str_a20_enabled: .string "\xF Enabled A20\r\n"
str_looping: .string "Looping...\r\n"
str_pm_enter: .string "\xF Entered protected mode\r\n"


GDT_START:
# Entry 1
.fill 8, 1, 0
# Entry 2
.word 0xFFFF # Limit (part 1)
.word 0 # Base (part 1)
.byte 0 # Base (part 2)
.byte 0x9A # Access byte
.byte 0b11001111 # Flags and limit (part 2)
.byte 0 # Base (part 3)
# Entry 3
.word 0xFFFF # Limit (part 1)
.word 0 # Base (part 1)
.byte 0 # Base (part 2)
.byte 0x92 # Access byte
.byte 0b11001111 # Flags and limit (part 2)
.byte 0 # Base (part 3)
GDT_END:

GDTR:
.word GDTR - GDT_START - 1
.long GDT_START
