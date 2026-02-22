CFLAGS := -std=c99 -O0 -no-pie -m32 -nostartfiles -nostdlib -ffreestanding
LDFLAGS := -no-pie --print-memory-usage -Map=output.map -m elf_i386 -T entry.ld
ASFLAGS := --32

.DEFAULT_GOAL := bootimg.bin

bootx86.o: bootx86.S
	as --32 bootx86.S -o bootx86.o
	objcopy --remove-section .note.gnu.property bootx86.o

entry.o: entry.S bootx86.o
	as --32 entry.S -o entry.o

interrupts.o: interrupts.S
	as --32 interrupts.S -o interrupts.o

bootimg.o: entry.o main.o graphics.o io.o pong.o interrupts.o
	$(LD) $(LDFLAGS) $^ -o $@

bootimg.bin: bootimg.o
	objcopy --output=binary bootimg.o bootimg.bin

.PHONY: clean
clean:
	rm -rf *.bin *.o output.map