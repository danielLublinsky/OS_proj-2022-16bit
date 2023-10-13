nasm -fbin ../codes/stage1/Boot1.asm -o ../codes/stage1/boot1.bin
dd if=../codes/stage1/boot1.bin conv=notrunc bs=1 count=512 of=../CHS=203-16-63.img

nasm -fbin ../codes/stage2/Boot2.asm -o ../codes/stage2/boot2.bin
dd if=../codes/stage2/boot2.bin of=../CHS=203-16-63.img conv=notrunc  seek=25 

nasm ../codes/kernel/source/call_kernel.asm -f elf32 -o ../codes/kernel/object/call_kernel.o
nasm ../codes/kernel/source/gdt_asm.asm -f elf32 -o ../codes/kernel/object/gdt_asm.o
nasm ../codes/kernel/source/idt_asm.asm -f elf32 -o ../codes/kernel/object/idt_asm.o

i686-elf-gcc -Ttext 0x8000 -ffreestanding -mno-red-zone -m32 -c "../codes/kernel/source/kernel.c" -o "../codes/kernel/object/kernel.o"
i686-elf-gcc -Ttext 0x8000 -ffreestanding -mno-red-zone -m32 -c "../codes/kernel/source/io.c" -o "../codes/kernel/object/io.o"
i686-elf-gcc -Ttext 0x8000 -ffreestanding -mno-red-zone -m32 -c "../codes/kernel/source/vga_print.c" -o "../codes/kernel/object/vga_print.o"
i686-elf-gcc -Ttext 0x8000 -ffreestanding -mno-red-zone -m32 -c "../codes/kernel/source/string.c" -o "../codes/kernel/object/string.o"
i686-elf-gcc -Ttext 0x8000 -ffreestanding -mno-red-zone -m32 -c "../codes/kernel/source/ata_disk.c" -o "../codes/kernel/object/ata_disk.o"
i686-elf-gcc -Ttext 0x8000 -ffreestanding -mno-red-zone -m32 -c "../codes/kernel/source/ports.c" -o "../codes/kernel/object/ports.o"
i686-elf-gcc -Ttext 0x8000 -ffreestanding -mno-red-zone -m32 -c "../codes/kernel/source/idt.c" -o "../codes/kernel/object/idt.o"
i686-elf-gcc -Ttext 0x8000 -ffreestanding -mno-red-zone -m32 -c "../codes/kernel/source/exceptions.c" -o "../codes/kernel/object/exceptions.o"
i686-elf-gcc -Ttext 0x8000 -ffreestanding -mno-red-zone -m32 -c "../codes/kernel/source/syscalls.c" -o "../codes/kernel/object/syscalls.o"
i686-elf-gcc -Ttext 0x8000 -ffreestanding -mno-red-zone -m32 -c "../codes/kernel/source/pic.c" -o "../codes/kernel/object/pic.o"
i686-elf-gcc -Ttext 0x8000 -ffreestanding -mno-red-zone -m32 -c "../codes/kernel/source/keyboard.c" -o "../codes/kernel/object/keyboard.o"
i686-elf-gcc -Ttext 0x8000 -ffreestanding -mno-red-zone -m32 -c "../codes/kernel/source/Physical_memory_manager.c" -o "../codes/kernel/object/Physical_memory_manager.o"
i686-elf-gcc -Ttext 0x8000 -ffreestanding -mno-red-zone -m32 -c "../codes/kernel/source/heap.c" -o "../codes/kernel/object/heap.o"
i686-elf-gcc -Ttext 0x8000 -ffreestanding -mno-red-zone -m32 -c "../codes/kernel/source/FAT.c" -o "../codes/kernel/object/FAT.o"
i686-elf-gcc -Ttext 0x8000 -ffreestanding -mno-red-zone -m32 -c "../codes/kernel/source/Shell.c" -o "../codes/kernel/object/Shell.o"
i686-elf-gcc -Ttext 0x8000 -ffreestanding -mno-red-zone -m32 -c "../codes/kernel/source/interpreter.c" -o "../codes/kernel/object/interpreter.o"


i686-elf-ld -T"link.ld"
dd if=../codes/debug/kernel.bin of=../CHS=203-16-63.img conv=notrunc seek=1000 