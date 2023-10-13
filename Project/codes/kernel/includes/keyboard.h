
//keyboard.h
#ifndef KEYBOARD_HEADER
#define KEYBOARD_HEADER

#include "../source/typeDef.h"
#include "vga_print.h"

//scancodes
#define ERROR_KEY_SCANCODE  0xFF
#define BACKSPACE_SCANCODE  0x8
#define ENTER_SCANCODE      0x0D
#define LEFT_SCANCODE       0x4B
#define RIGHT_SCANCODE      0x4D
#define DOWN_SCANCODE       0x50
#define UP_SCANCODE         0x48
#define PAGEDOWN_SCANCODE   0x7D
#define PAGEUP_SCANCODE     0x7A


#define KEY_INFO_ADDRESS 0x1600


typedef struct {
    uint_8  key;   
    bool    shift;
    bool    ctrl;
} __attribute__((packed)) key_info_t;



key_info_t get_key(void);
#endif

