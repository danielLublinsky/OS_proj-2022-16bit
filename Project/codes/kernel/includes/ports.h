#ifndef TYPE_DEF_HEADER
#define TYPE_DEF_HEADER
#include <stdint.h>

unsigned char port_byte_in (unsigned short port);
void port_byte_out (unsigned short port, unsigned char data);

short port_word_in (short port);
void port_word_out (unsigned short port, unsigned short data);

int port_long_in(int port);
void port_long_out(int port, int value);
#endif
