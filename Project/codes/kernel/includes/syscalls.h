//syscall.h

#ifndef SYSCALLS_HEADER
#define SYSCALLS_HEADER

#include "vga_print.h"
#include "../includes/pic.h"

#define MAX_SYSCALLS 3

void syscall_test0(void);

void syscall_test1(void);

void syscall_sleep(void);

// Syscall dispatcher
// naked attribute means no function prologue/epilogue, and only allows inline asm
__attribute__((naked)) void syscall_dispatcher(void);

#endif