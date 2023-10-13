#include "../includes/io.h"

void outb(unsigned short port, unsigned char val){
	asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

unsigned char inb(unsigned short port){
	unsigned char returnVal;
	asm volatile("inb %1, %0" : "=a"(returnVal) : "Nd"(port));
	return returnVal;
}

// Wait 1 I/O cycle for I/O operations to complete
void io_wait(void)
{
    // Port 0x80 is used for checkpoints during POST
    //  Linux uses this, so we "should" be OK to use it also
    __asm__ __volatile__ ("outb %%al, $0x80" : : "a"(0) );
}