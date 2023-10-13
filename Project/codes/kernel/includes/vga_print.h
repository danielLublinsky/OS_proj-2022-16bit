#ifndef VGA_PRINT_HEADER
#define VGA_PRINT_HEADER

#include "../source/typeDef.h"
#include "io.h"
#include "string.h"

#define VGA_MEMORY (unsigned char*)0xb8000
#define VGA_WIDTH 80
#define VGA_HIGHT 25

#define CYAN_SYSTEMCOLOR   (Color_params){ 0, Black, 1, Cyan }
#define MAGENTA_SYSTEMCOlOR  (Color_params){ 0, Black, 1, Magenta }
#define DEFAULT_SYSTEMCOlOR  (Color_params){ 0, Black, 1, Green }
#define DEFAULT_ERORR_SYSTEMCOlOR  (Color_params){ 0, Black, 1, Red }

typedef struct {
	uint_8 blinking_Foreground_Intensity;      //1 bit:   1-blink, 0-no
	uint_8 backround;                          //3 bits:  backround color
	uint_8 Foreground_Intensity;               //1 bit:   0- weak, 1- strong
	uint_8 foreground;                         //3 bits:  foreground color
}__attribute__((packed)) Color_params;

typedef struct {
	Color_params params;
	char data;
}__attribute__((packed))  Screen_charecter;


//00001011
enum colors {
	Black, Blue, Green, Cyan, Red, Magenta, Brown, Gray, Dark_Gray, Light_Blue, Light_Green, Light_Cyan, Light_Red, Light_Magenta, Light_Brown, White,
};


void setCursorPosition(unsigned short position);
uint_16 PositionFromCoords(uint_8 x, uint_8 y);
void initScreen();
void clearScreen();

//void initScreenPage(uint_16 startCoords);
//
//void putc_input_debug(char c);
void putc(char c);
void puts(const char* str);


void puts_system(const char* str, Color_params);
void putc_color_system(char c, Color_params color);
void printf_system(Color_params color,const char* fmt, ...);

void putc_color(char c, Color_params color);
void puts_color(const char* c, Color_params color);

void putc_clock(char c);
void puts_clock(uint_16 val);

unsigned short getCursorPosition();
void printf(const char* fmt, ...);


#endif