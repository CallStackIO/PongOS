# PongOS

x86 Operating system that plays pong. Thats all it does.

### Compiling
You need:
- gcc
- binutils
- make

Run make in the project directory. You will get bootimg.bin. This is the raw binary file.

### Running
You must use a BIOS compatible x86 system or a UEFI system with BIOS compatibility mode (most motherboards have this). Flash bootimg.bin to a usb and select it in the boot menu.

If you wish to run this OS in a virtual machine, you can use bochs (recommended) or qemu (qemu was unbearably slow on my system) or some other x86 emulator.

