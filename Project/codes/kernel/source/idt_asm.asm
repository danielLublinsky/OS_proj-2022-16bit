;idt.asm

;*****  devide by zero interrupt: isr 0  *****
[extern div_by_0_handler]
div_by_0_asm:  
	call div_by_0_handler     
	iret 
	GLOBAL div_by_0_asm


;*****  Timer pit interrupt: isr 0x20  *****
[extern timer_irq0_handler]
timer_pit_asm:  
	call timer_irq0_handler     
	iret 
	GLOBAL timer_pit_asm


;*****  RTC, interrupt: isr 0x28  *****
[extern cmos_rtc_irq8_handler]
rtc_irq8_asm: 
	pusha
	call cmos_rtc_irq8_handler     
	popa
	iret 
	GLOBAL rtc_irq8_asm

                                   
                                   
;*****  keyboard pit interrupt: isr 0x21  *****
[extern keyboard_irq1_handler]
keyboard_asm:
    pusha 
	call keyboard_irq1_handler 
	popa  
	iret       
	GLOBAL keyboard_asm        
	

[extern _idtr]
[extern _idt]

idtDescriptor:
	dw 4095
	dd _idt


LoadIDT:
	lidt[idtDescriptor]
;	sti
	ret
	GLOBAL LoadIDT