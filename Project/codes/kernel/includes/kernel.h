#ifndef KERNEL_H
#define KERNEL_H

#include "../source/typeDef.h"
#include "vga_print.h"
#include "string.h"
#include "ata_disk.h"
#include "idt.h"
#include "exceptions.h"
#include "syscalls.h"
#include "pic.h"
#include "keyboard.h"
#include "Physical_memory_manager.h"
#include "heap.h"s
#include "FAT.h"
#include "Shell.h"

//! install gdtr
void gdt_install();
void InstallGDT();
int userNewLine(bool new_line);
int userEnter(char* input);

void print_physical_memory_info(void);

void pageDown();

#endif