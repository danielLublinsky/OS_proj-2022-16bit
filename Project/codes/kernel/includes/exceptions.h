#include "../includes/idt.h"
#include "vga_print.h"

#ifndef EXCEPTION_H
#define EXCEPTION_H

void div_by_0_handler(int_frame_32_t *frame);

#endif