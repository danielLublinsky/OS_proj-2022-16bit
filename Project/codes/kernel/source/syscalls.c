//syscalls.c

#include "../includes/syscalls.h"
extern Color_params current_color;

void syscall_test0(void) {
    current_color.foreground = Blue;
    puts_color("\n\rtest- syscall: 0", current_color);
}

void syscall_test1(void) {
    current_color.foreground = Green;
    puts_color("\n\rtest- syscall: q", current_color);
}

extern uint_32* sleep_timer_ticks;
// Sleep for a given number of milliseconds
// INPUTS:
//  EBX = # of milliseconds
void syscall_sleep(void)
{
    __asm__ __volatile__("mov %%ebx, %0" : "=r"(*sleep_timer_ticks));
    // Wait ("Sleep") until # of ticks is 0
    while (*sleep_timer_ticks > 0) __asm__ __volatile__("sti;hlt;cli");
}

// Syscall table
void* syscalls[MAX_SYSCALLS] = {
    syscall_test0,
    syscall_test1,
    syscall_sleep
};

__attribute__((naked)) void syscall_dispatcher(void)
{
    __asm__ __volatile__(".intel_syntax noprefix\n"

        ".equ MAX_SYSCALLS, 3\n"  // Have to define again, inline asm does not see the #define

        "cmp eax, MAX_SYSCALLS-1\n"   // syscalls table is 0-based
        "ja invalid_syscall\n"        // invalid syscall number, skip and return

        "push eax\n"
        "push gs\n"
        "push fs\n"
        "push es\n"
        "push ds\n"
        "push ebp\n"
        "push edi\n"
        "push esi\n"
        "push edx\n"
        "push ecx\n"
        "push ebx\n"
        "push esp\n"
        "call [syscalls+eax*4]\n"
        "add esp, 4\n"
        "pop ebx\n"
        "pop ecx\n"
        "pop edx\n"
        "pop esi\n"
        "pop edi\n"
        "pop ebp\n"
        "pop ds\n"
        "pop es\n"
        "pop fs\n"
        "pop gs\n"
        "add esp, 4\n"    // Save eax value in case
        "iretd\n"         // Need interrupt return here! iret, NOT ret

        "invalid_syscall:\n"
        "iretd\n"

        ".att_syntax");
}