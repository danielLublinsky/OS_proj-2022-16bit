
;*********************************************
;	Boot1.asm
;		- A Simple Bootloader
;
;	Operating Systems Development Series
;*********************************************

[bits    16]                          ; We are still in 16 bit Real Mode


;[org 0x7c00]						; we will set regisers later

start:	jmp	main					; jump to start of bootloader


clusterStage2 dw 25         ;0x3200 on disk


;************************************************;
; Reads a series of sectors
; CX=>Number of sectors to read
; AX=>Starting sector
; ES:BX=>Buffer to read to
;************************************************;



ReadSectors:
        mov     ah, 0x02                            ; BIOS read sector
        mov     al, 63                            ; read one sector
        mov     ch, 0                               ; cylinder
        mov     cl, 26                               ; sector
        mov     dh, 0                               ; head
        mov     dl, 0x80                            ; drive
        int     0x13  
        
        jnc     DONE                                ; test for read error
        xor     ax, ax                              ; BIOS reset disk
        int     0x13                                ; invoke BIOS
        dec     di                                  ; decrement error counter
        jnz     ReadSectors                         ; attempt to read again
        int     0x18


     
;*********************************************
;	Bootloader Entry Point
;*********************************************

main:

     ;----------------------------------------------------
     ; code located at 0000:7C00, adjust segment registers
     ;----------------------------------------------------
          cli						; disable interrupts
          mov     ax, 0x07C0				; setup registers to point to our segment
          mov     ds, ax
          mov     es, ax
          mov     fs, ax
          mov     gs, ax

     ;----------------------------------------------------
     ; create stack
     ;----------------------------------------------------
     
          mov     ax, 0x0000				; set the stack
          mov     ss, ax
          mov     sp, 0xFFFF
          sti						; restore interrupts

     ;read to:
         xor ax,ax
         mov es, ax

         mov bx, 0x8000  

          
     ;----------------------------------------------------
     ; Load Stage 2
     ;----------------------------------------------------

     LOAD_IMAGE:
          xor     cx, cx 
          mov     di, 0x0005    ;5 tries of reading 
          push    bx 
          call    ReadSectors
                
     DONE: 
          push 0x8000
          ret 

          
     
          TIMES 510-($-$$) DB 0
          DW 0xaa55
