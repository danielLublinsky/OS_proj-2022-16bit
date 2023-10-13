[bits 32]
;[org 0x100000]
[extern _start]       ;kernel             

load_kernel:       
    call _start  
    jmp $      
         
