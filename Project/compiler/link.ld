OUTPUT_FORMAT(binary)

ENTRY(_start)

INPUT
(
	../codes/kernel/object/call_kernel.o
	../codes/kernel//object/kernel.o
	../codes/kernel/object/io.o
	../codes/kernel/object/vga_print.o
	../codes/kernel/object/string.o
	../codes/kernel/object/ata_disk.o
	../codes/kernel/object/ports.o
	../codes/kernel/object/exceptions.o
	../codes/kernel/object/syscalls.o
	../codes/kernel/object/gdt_asm.o
	../codes/kernel/object/idt.o
	../codes/kernel/object/idt_asm.o
	../codes/kernel/object/keyboard.o
	../codes/kernel/object/pic.o
	../codes/kernel/object/Physical_memory_manager.o
	../codes/kernel/object/heap.o
	../codes/kernel/object/FAT.o
	../codes/kernel/object/Shell.o
	../codes/kernel/object/interpreter.o

)

OUTPUT
(
	../codes/debug/kernel.bin
)

SECTIONS
{
	. = 0x100000;
	
	.text : ALIGN(0X1000)
	{
	*(.text)
	}
	
	.idt BLOCK(0X1000) : ALIGN(0X1000)
	{
		_idt = .;
		. = . + 0x1000;
	}
	
	.data : ALIGN(0x1000)
	{
		*(.data)
	}

	.rodata : ALIGN(0x1000)
	{
		*(.rodata)
	}

	.bss : ALIGN(0x1000)
	{
		*(COMMON)
		*(.bss)
	}
	
	
	end = .;

}