#include <stdint.h>
#include "../includes/ports.h"
#include "../includes/ports.h"

unsigned char port_byte_in (unsigned short port) {
    unsigned char result;
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

void port_byte_out (unsigned short port, unsigned char data) {
    __asm__("out %%al, %%dx" : : "a" (data), "d" (port));
}

short port_word_in (short port) {
    short result;
    __asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
}

void port_word_out (unsigned short port, unsigned short data) {
    __asm__("out %%ax, %%dx" : : "a" (data), "d" (port));
}

void port_long_out(int port, int value) {
	   __asm__ __volatile__("outl %%eax,%%dx"::"d" (port), "a" (value));
};

int port_long_in(int port) {
    int result;
    __asm__ __volatile__("inl %%dx,%%eax":"=a" (result):"d"(port));
    return result;
}