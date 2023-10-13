//idt.h

#ifndef IDT_HEADER
#define IDT_HEADER

#include "../source/typeDef.h"
#include "vga_print.h"

#define TRAP_GATE_FLAGS 0x8F      //p-1, DPL- 00, S- 0, TYPE-1111(32bit trap gate)
#define INT_GATE_FLAGS 0x8E       //p-1, DPL- 00, S- 0, TYPE-1110(32bit interrupt gate)
#define INT_GATE_USER_FLAGS 0xEE  //p-1, DPL- 11, S- 0, TYPE-1110(32bit trap gate called from user )

#define INTERRIPUTS_NUMBER 256

//idt entry
typedef struct {
	uint_16 isr_address_low; 
	uint_16 kernel_cs;           
	uint_8 reserved;            //set to 0
	uint_8 attributes;         //type and attributes, fkags
	uint_16 isr_address_high;
}__attribute__((packed)) idt_entry_32_t;


//idtr layout
typedef struct {
	uint_16 limit;
	uint_32 base;
}__attribute__((packed)) idtr_32_t;


//Interrupt "frame" to pass to interrpt handlers
typedef struct {
	uint_32 eip;
	uint_32 cs;
	uint_32 eflags;
	uint_32 sp;
	uint_32 ss;
}__attribute__((packed)) int_frame_32_t;

//default exception handler(no error code)
void default_excp_handler(int_frame_32_t *int_frame_32);

//default exception handler(include error code)
void default_excp_handler_err_code(int_frame_32_t* int_frame_32, uint_32 error_code);

//Default interrupt handler
void default_int_handler(int_frame_32_t* int_frame_32);

//Add an ISR to the IDT
void set_idt_descriptor_32(uint_8 entry_number, void* isr, uint_8 flags);

//setup/initilize the IDT
void init_idt_32(void);

#endif