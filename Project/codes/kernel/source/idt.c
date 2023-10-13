#include "../includes/idt.h"


extern idt_entry_32_t _idt[INTERRIPUTS_NUMBER];
idtr_32_t _idtr;


//default exception handler(no error code)
void default_excp_handler(int_frame_32_t *int_frame_32)
{
	printf("%s", "DEFAULT EXCEPTION HANDLER - NO ERROR CODE");
}

//default exception handler(include error code)
void default_excp_handler_err_code(int_frame_32_t* int_frame_32, uint_32 error_code) {
	printf("%s, %d", "DEFAULT EXCEPTION HANDLER - ERROR CODE: ", error_code);
}

//Default interrupt handler
void default_int_handler(int_frame_32_t* int_frame_32)
{
	printf("%s", "DEFAULT INERRUPT HANDLER");
}

//Add an ISR to the IDT
void set_idt_descriptor_32(uint_8 entry_number, void* isr, uint_8 flags)
{
	idt_entry_32_t *descriptor = &_idt[entry_number];
	descriptor->isr_address_low = (uint_32)isr & 0xFFFF;	       //low 16 bit of isr
	descriptor->kernel_cs = 0x08;                                  //kernel code segment
	descriptor->reserved = 0;                                      //must be 0
	descriptor->attributes = flags;                                //type & attributes
	descriptor->isr_address_high = ((uint_32)isr >> 16) & 0xFFFF;  //high 16 bit of isr
}

//setup/initilize the IDT
void init_idt_32(void)
{
	_idtr.limit = (uint_16)(sizeof _idt);
	_idtr.base = (uint_32)&_idt;

	//Set up exception handlers fiest(0-31)
	for (uint_8 entry; entry < 32; entry++) {
		if (entry == 8 || entry == 10 || entry == 11 || entry == 12 ||
			entry == 13 || entry == 14 || entry == 17 || entry == 21)
			//exception takes an error code
			set_idt_descriptor_32(entry, default_excp_handler_err_code, TRAP_GATE_FLAGS);
		else
		{
			//exception does not takes an error code
			set_idt_descriptor_32(entry, default_excp_handler, TRAP_GATE_FLAGS);   //default_excp_handler
		}
	}
	//set up regular interrupts(ISRs 32-255)
	for (uint_16 entry = 32; entry < 256; entry++) {
		set_idt_descriptor_32(entry, default_int_handler, INT_GATE_FLAGS);
	}
	LoadIDT();
	//asm ( "lidt %0" : : "m"(_idtr) );  // this also working
}