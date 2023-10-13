//pic.h

#include "../source/typeDef.h"
#include "../includes/io.h"
#include "../includes/vga_print.h"
#include "idt.h"
#include "../includes/keyboard.h"

#define PIC_1_CMD  0x20
#define PIC_1_DATA 0x21

#define PIC_2_CMD  0xA0
#define PIC_2_DATA 0xA1

#define NEW_IRQ_0  0x20 // IRQ 0-7  will be mapped to interrupts 0x20-0x27 (32-39)
#define NEW_IRQ_8  0x28 // IRQ 8-15 will be mapped to interrupts 0x28-0x2F (40-47)

#define PIC_EOI    0x20 // "End of interrupt" command

#define PS2_DATA_PORT 0x60



// Send end of interrupt command to signal IRQ has been handled
void send_pic_eoi(uint_8 irq);

// Disable PIC, if using APIC or IOAPIC
void disable_pic(void);

// Set IRQ mask by setting the bit in the IMR (interrupt mask register)
//   This will ignore the IRQ
void set_irq_mask(uint_8 irq);

// Clear IRQ mask by clearing the bit in the IMR (interrupt mask register)
//   This will enable recognizing the IRQ
void clear_irq_mask(uint_8 irq);

// Remap PIC to use interrupts above first 15, to not interfere with exceptions (ISRs 0-31)
void remap_pic(void);

void timer_irq0_handler(void);

void set_pit_channel_mode_frequency(const uint_8 channel, const uint_8 operating_mode, const uint_16 frequency);

//RTC, CMOS: get the real time clock:
bool cmos_update_in_progress(void);
uint_8 get_rtc_register(uint_8 reg);
void enable_rtc(void);
void disable_rtc(void);
void cmos_rtc_irq8_handler();
void keyboard_irq1_handler(void);
