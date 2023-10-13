
#include "../includes//vga_print.h"

Color_params current_color;

Screen_charecter screen_charecter[VGA_HIGHT][VGA_WIDTH];
int screen_x_value = 0;
int screen_y_value = 0;


unsigned short CursorPosition;
//uint_32 staring_point = 0;
uint_16 clockPosition = 240;


void setCursorPosition(unsigned short position){
	outb(0x3D4, 0x0F);
	outb(0x3D5, (unsigned char)(position & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (unsigned char) ((position >> 8) & 0xFF));
	
	CursorPosition = position;
}

uint_16 PositionFromCoords(uint_8 x, uint_8 y){
	return y * VGA_WIDTH + x;
}

void initScreen() {
	uint_8* vga_memory_address = VGA_MEMORY;
	unsigned int i = 0;
	unsigned int j = 0;

	while (j < 80 * 25 * 2) {
		vga_memory_address[j] = ' ';
		//vga_memory_address[j + 1] = null;
		j = j + 2;
	}

	setCursorPosition(PositionFromCoords(12, 1));

    current_color.blinking_Foreground_Intensity = 0;
    current_color.backround = Black;
    current_color.Foreground_Intensity = 1;
    current_color.foreground = Green;

    puts_color("Welcome to my operating system: yudiel system\n\n", current_color);
    setCursorPosition(PositionFromCoords(50, 3));
    printf("developed by: Yehuda, Daniel");
	setCursorPosition(PositionFromCoords(0, 4));
    puts_color("********************************************************************************\n\n", current_color);

	setCursorPosition(PositionFromCoords(0, 5));
    current_color.foreground = Cyan;
  
}

void clearScreen()
{
    uint_8* vga_memory_address = VGA_MEMORY;
    unsigned int j = 480;

    while (j < 3040) {
        vga_memory_address[j] = ' ';
        //vga_memory_address[j + 1] = null;
        j = j + 2;
    }

    setCursorPosition(480);
}

//putc without color option
void putc(char c){
    current_color.foreground = Cyan;
    putc_color(c, current_color);
    return;
}

//puts without color option
void puts(const char* str) {
	uint_8* charPtr = (uint_8*)str;
	
	while(*charPtr != 0){
		putc(*charPtr);
		charPtr++;
	}

}


//puts without color option
void puts_system(const char* str,Color_params color) {
    uint_8* charPtr = (uint_8*)str;

    while (*charPtr != 0) {
        putc_system(*charPtr, color);
        charPtr++;
    }
    //putc_system(0, color);
}

void putc_system(char c, Color_params color) {
    uint_8* vga_memory_address = VGA_MEMORY;
    uint_16 index = CursorPosition;

    uint_8 blinking_Foreground_Intensity = color.blinking_Foreground_Intensity << 7;
    uint_8 backround = color.backround << 4;
    uint_8 foreground_Intensity = color.Foreground_Intensity << 3;
    uint_8 foreground = color.foreground;

    uint_8 char_color = blinking_Foreground_Intensity + backround + foreground_Intensity + foreground;
    if (c == 10) { index += 80; screen_y_value++; }
    else if (c == 13) { index -= index % 80; screen_x_value = 0; }
    else {
        *(VGA_MEMORY + CursorPosition * 2) = c;
        *(VGA_MEMORY + CursorPosition * 2 + 1) = char_color;
        index++;
        screen_charecter[screen_y_value][screen_x_value].data = c;
        screen_charecter[screen_y_value][screen_x_value].params = color;
        screen_x_value++;
    }
    setCursorPosition(index);
}


//putc with color option
void putc_color(char c, Color_params color) {
    uint_8* vga_memory_address = VGA_MEMORY;
    uint_16 index = CursorPosition;

    uint_8 blinking_Foreground_Intensity = color.blinking_Foreground_Intensity << 7;
    uint_8 backround = color.backround << 4;
    uint_8 foreground_Intensity = color.Foreground_Intensity << 3;
    uint_8 foreground = color.foreground;

    uint_8 char_color = blinking_Foreground_Intensity + backround + foreground_Intensity + foreground;
    if (c == 10) index += 80;
    else if (c == 13) index -= index % 80;
    else {
        *(VGA_MEMORY + CursorPosition * 2) = c;
        *(VGA_MEMORY + CursorPosition * 2 + 1) = char_color;
        index++;
    }
    setCursorPosition(index);
}

//puts with color option
void puts_color(const char* str, Color_params color) {
    uint_8* charPtr = (uint_8*)str;

    while (*charPtr != 0) {
        putc_color(*charPtr, color);
        charPtr++;
    }
}

void putc_clock(char c)
{
    *(VGA_MEMORY + clockPosition * 2) = c;
    *(VGA_MEMORY + clockPosition * 2 + 1) = 3;
    clockPosition++;
}

void puts_clock(uint_16 val)
{
    char str[100];
    itoa(val, str, 10);
    uint_8* charPtr = (uint_8*)str;

    while (*charPtr != 0) {
        putc_clock(*charPtr);
        charPtr++;
    }
}

unsigned short getCursorPosition(){
	return CursorPosition;
}


#define PRINTF_STATE_NORMAL 0
#define PRINTF_STATE_LENGTH 1
#define PRINTF_STATE_SPECIFIER 2

void printf(const char* fmt, ...) {
    current_color.foreground = Cyan;
    int* argp = (int*)&fmt;
    int state = PRINTF_STATE_NORMAL;
    uint_16 index = getCursorPosition();
    char str[100];
    for (int i = 0; i < 100; i++)   //flush str
    {
        str[i] = 0;
    }

    argp++;
    while (*fmt) {
        switch (state)
        {
        case PRINTF_STATE_NORMAL:
            switch (*fmt)
            {
            case '%':
                state = PRINTF_STATE_SPECIFIER;
                break;
            case 10:    //new line
                index += 80;
                setCursorPosition(index);
                break;
            case 13:
                index -= index % 80;
                setCursorPosition(index);
                break;
            default:
                putc(*fmt);
                break;
            }
            break;
        case PRINTF_STATE_LENGTH:
            break;
        case PRINTF_STATE_SPECIFIER:
            switch (*fmt)
            {
            case 'd':
                itoa(*argp, str, 10);
                puts(str);
                argp++;
                break;
            case 'c':
                putc((char)*argp);
                argp++;
                break;
            case 's':
                puts(*(char**)argp);
                argp++;
                break;
            case 'x':    //print hex- %x
               // printf("%d ", *argp);
                hexToString(*argp, str);
                puts(str);
                argp++;
                break;

            case '%':
                putc('%');
                break;
            default:
                break;
            }
            state = PRINTF_STATE_NORMAL;
            break;

        default:
            break;
        }
        fmt++;
    }
}



void printf_system(Color_params color,const char* fmt, ...) {
    if (screen_y_value >= 24)
        return;
    int* argp = (int*)&fmt;
    int state = PRINTF_STATE_NORMAL;
    uint_16 index = getCursorPosition();
    char str[100];
    for (int i = 0; i < 100; i++)   //flush str
    {
        str[i] = 0;
    }

    argp++;
    while (*fmt) {
        switch (state)
        {
        case PRINTF_STATE_NORMAL:
            switch (*fmt)
            {
            case '%':
                state = PRINTF_STATE_SPECIFIER;
                break;
            case 10:    //new line
                index += 80;
                setCursorPosition(index);
                screen_y_value++;
                break;
            case 13:
                index -= index % 80;
                setCursorPosition(index);
                screen_x_value = 0;
                break;
            default:
                putc_system(*fmt,color);
                break;
            }
            break;
        case PRINTF_STATE_LENGTH:
            break;
        case PRINTF_STATE_SPECIFIER:
            switch (*fmt)
            {
            case 'd':
                itoa(*argp, str, 10);
                puts_system(str,color);
                argp++;
                break;
            case 'c':
                putc_system((char)*argp,color);
                argp++;
                break;
            case 's':
                puts_system(*(char**)argp,color);
                argp++;
                break;
            case 'x': 
                hexToString(*argp, str);
                puts_system(str,color);
                argp++;
                break;

            case '%':
                putc_system('%',color);
                break;
            default:
                break;
            }
            state = PRINTF_STATE_NORMAL;
            break;

        default:
            break;
        }
        fmt++;
    }
}