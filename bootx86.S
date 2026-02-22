# MBR boot code
# By Sullivan DeVries

# NOTE: We have about 480 KB usable memory from 0x7E00 to 0x7FFFF
# The rest of usable memory is at 0x500 to 0x7BFF


.code16

.org 0
.text
.global _start
_start:
	# Set up segment registers
	# Zero these out
	xor %AX, %AX
	mov %AX, %DS
	mov %AX, %ES
	mov %AX, %SS

	# Set stack to just below where the code is loaded (0x7c00)
	mov $0x7c00, %SP

	# Set video mode to 3
	mov $0, %AH
	mov $3, %AL
	int $0x10
	
	// Print the starting string
	lea starting_string, %SI
	call printstr


	# This to me is so funny
	# I was too lazy to implement something that reads the size of the program and loops through until all sectors are loaded
	# So I have just been manually adding calls to readsector when I need to
	# Every time my code stopped working the first thing I checked is if I need to add another readsector call
	# Im such a lazy bastard
	mov $1, %AL
	mov $0x7e00, %BX
	call readsector
	mov $2, %AL
	add $512, %BX
	call readsector
	mov $3, %AL
	add $512, %BX
	call readsector
	mov $4, %AL
	add $512, %BX
	call readsector
	mov $5, %AL
	add $512, %BX
	call readsector
	mov $6, %AL
	add $512, %BX
	call readsector
	mov $7, %AL
	add $512, %BX
	call readsector
	mov $8, %AL
	add $512, %BX
	call readsector
	mov $9, %AL
	add $512, %BX
	call readsector
	mov $10, %AL
	add $512, %BX
	call readsector
	mov $11, %AL
	add $512, %BX
	call readsector
	mov $12, %AL
	add $512, %BX
	call readsector
	mov $13, %AL
	add $512, %BX
	call readsector
	mov $14, %AL
	add $512, %BX
	call readsector

	# Jump to init
	ljmp $0, $0x7E00

	# Loop
	jmp .

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

# SI = Offset of string
printstr:
	push %SI
	push %AX
	.loop:
		lodsb
		cmp $0, %AL
		je .done
		call printc
		jmp .loop

	.done:
		pop %AX
		pop %SI
		ret


# DL = Drive number
# AL = LBA
# ES:BX = Buffer
readsector:
	push %AX
	push %CX
	push %DX
	# BX will be clobbered so put it at the end because we will need it later
	push %BX
	# We need sectors per track and number of heads in order to convert LBA to CHS
	# Use int 0x13, we need DL=drive number, AH=8
	push %AX # Preserve AL
	push %DX
	mov $8, %AH
	int $0x13
	pop %DX
	pop %AX

	# magic, don't worry
	inc %DH
	and $0x3f, %CL
	mov %DH, %CH
	# now CH = # of heads, CL = sectors per track

	# do the conversion
	div %CL
	mov %AL, %CL
	mov %AH, %BL
	# CL = LBA / sectors per track (temp), BL = LBA % sectors per track
	inc %BL
	# BL = Sector
	xor %AX, %AX
	mov %CL, %AL
	div %CH
	mov %AL, %CH
	# CH = Cylinder
	mov %AH, %DH
	# DH = Head

	# Now set up the registers for the bios interrupt for reading
	mov %CH, %CL
	and $0xFF, %CH
	shr $2, %CL
	and $0xC0, %CL
	or %BL, %CL
	xor %AX, %AX
	mov %AX, %ES
	mov $2, %AH
	mov $1, %AL
	

	# pop BX
	pop %BX

	# call the interrupt
	int $0x13

	# pop the remaining registers and return
	pop %DX
	pop %CX
	pop %AX
	ret
	













.align 2
starting_string: .string "Booting PongOS\r\n\n"

.org 0x1FE
.byte 0x55, 0xAA
