#ifndef IO_HEADER
#define IO_HEADER

void outb(unsigned short port, unsigned char val);
unsigned char inb(unsigned short port);
void io_wait(void);

#endif