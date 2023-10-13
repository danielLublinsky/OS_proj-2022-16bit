#include "../includes/kernel.h"


extern void div_by_0_asm();    //isr 0
extern void timer_pit_asm();   //isr 0x20
extern void rtc_irq8_asm();    //isr 0x28
extern void keyboard_asm();    //isr 0x21

extern bool show_datetime;
extern Color_params current_color;

//extern from physical memory
extern uint_32* memory_map;
extern uint_32 max_blocks;
extern uint_32 used_blocks;
extern BootSector bootSector;
uint_32 staring_point;
bool running = true;
extern char currPath[255];
extern bool cleared;


//physical memory entry from Bios Int 15, EBX E820H 
typedef struct {
	uint_64 base_address;
	uint_64 length;
	uint_32 type;
	uint_32 acpi;
}SMAP_entry_t;


char cmd_previous_command[256];

char cmd_string[256];
int input_length = 0;
int input_index = 0;
char input_char;

extern Screen_charecter screen_charecter[VGA_HIGHT][VGA_WIDTH];
extern int screen_x_value;
extern int screen_y_value;

int screen_start_point = 0; 


void _start(){
	initScreen();       //clean the bochs's screen
	InstallGDT();
	init_idt_32();      //Set up IDT

	set_idt_descriptor_32(0, div_by_0_asm, TRAP_GATE_FLAGS);               //add divide by 0  to IDT
	set_idt_descriptor_32(0x80, syscall_dispatcher, INT_GATE_USER_FLAGS);  //add syscall tes  to IDT

	disable_pic();
	remap_pic();        //Remap pic1 and pic2

	set_idt_descriptor_32(0x20, timer_pit_asm, INT_GATE_FLAGS);            //add timer- PIT tes  to IDT
	set_idt_descriptor_32(0x28, rtc_irq8_asm, INT_GATE_FLAGS);             //add rtc handler- real time clock
	set_idt_descriptor_32(0x21, keyboard_asm, INT_GATE_FLAGS);             //add keyboard handler to IDT

	clear_irq_mask(0);   //enable timer
	clear_irq_mask(2);   //enable pic2 line
	clear_irq_mask(1);   //enable keyboard
	clear_irq_mask(8);   //enable CMOS RTC IRQ8

	// Enable CMOS RTC
	enable_rtc();

	__asm__ __volatile__("sti");

	//__asm__ __volatile__ ("movl $0, %eax; int $0x80");
	//__asm__ __volatile__("movl $1, %eax; int $0x80");
	//__asm__ __volatile__("movl $2, %eax; int $0x80");   // Should do nothing

	set_pit_channel_mode_frequency(0, 2, 1193);   // 1193182 MHZ / 1193 = ~1000hz

	//clock:
	show_datetime = true;

	//show_datetime = !show_datetime;

	//Todo: when user clicks exit-clock -> return show_datatime to false

	//sleep:
	//__asm__ __volatile__("int $0x80" : : "a"(2), "b"(5000));

	//setCursorPosition(800);

	uint_32 SMAP_entries_amount = *(uint_32*)0x8500;
	SMAP_entry_t* SMAP_entry = (SMAP_entry_t*)0x8504;

	SMAP_entry += SMAP_entries_amount - 2;     //temporary -2,  TODO: change to -1
	uint_32 total_memory = SMAP_entry->base_address + SMAP_entry->length - 1;

	int countMemoryRegion = 0;
	init_memory_manager(0x30000, total_memory);
	SMAP_entry = (SMAP_entry_t*)0x8504;
	for (uint_32 i = 0; i < SMAP_entries_amount; i++) {
		if (SMAP_entry->type == 1) {     //Avialable region
			init_memory_region(SMAP_entry->base_address, SMAP_entry->length);
			//if(countMemoryRegion == 0)
				//InitializeHeap(SMAP_entry->base_address, 100000);
			countMemoryRegion++;
		}

	}

	deinit_memory_region(0x1000, 0x9000);                           //reserved for kernel/os
	deinit_memory_region(0x30000, max_blocks / BLOCK_PER_BYTE);     //reserved for memory map
	//print_physical_memory_info();

	
	//init the heap section
	InitializeHeap(0x100000, 100000);


	InitializeBootSector();
	InitializeFileSystem();
	initializeShell();

	//user input section
	key_info_t key_info;
	memset(cmd_string, 0, 256);
	memset(cmd_previous_command, 0, 256);
	userNewLine(false);
	screen_start_point = getCursorPosition();
	staring_point = screen_start_point;
	while (1)
	{
		//if (!running) return;
	
		key_info = get_key();
		input_char = key_info.key;
		
		//printf("%d", input_length);

		if (input_char == ERROR_KEY_SCANCODE)
		{
			printf_system(DEFAULT_ERORR_SYSTEMCOlOR,"err");
		}
		else if (input_char == BACKSPACE_SCANCODE) {
			if (input_index > 0) {
				setCursorPosition(getCursorPosition() - 1);
				int temp = input_index;
				while (input_index < input_length) {
					cmd_string[input_index - 1] = cmd_string[input_index];
					input_index++;
				}
				cmd_string[input_length - 1] = 0;
				input_index = temp - 1;
				setCursorPosition(staring_point);
				printf("%s", cmd_string);
				putc(' ');
				setCursorPosition(staring_point + input_index);
				input_length--;
			}
		}
		else if (input_char == ENTER_SCANCODE) {			 //enter key
			int start = 0;
			memset(cmd_previous_command, 0, 256);
			while (start < input_length)
			{
				cmd_previous_command[start] = cmd_string[start];
				screen_charecter[screen_y_value][screen_x_value].data = cmd_string[start];
				screen_charecter[screen_y_value][screen_x_value].params = CYAN_SYSTEMCOLOR;
				screen_x_value++;
				cmd_string[start] = 0;
				start++;
			}
			if (screen_y_value >= 18)
				pageDown();

			//call parser of shell
			char* input = deleteSpaces(cmd_previous_command);
			parser_command(input);

			if(!cleared)
				userNewLine(true);
			//userNewLine(cmd_string);
			staring_point = getCursorPosition();
			memset(cmd_string, 0, 256);
			input_length = 0;
			input_index = 0;
			cleared = false;
		}
		//arrow control section (temp code):
        else if(input_char == LEFT_SCANCODE){
            if(input_index > 0){					//Left arrow
                setCursorPosition(getCursorPosition() - 1); 
				input_index--; 
			}
        }
        else if(input_char == RIGHT_SCANCODE){		//right arrow
			if(input_index < input_length){
            	setCursorPosition(getCursorPosition() + 1); 
				input_index++; 
			}
		}
		else if (input_char == PAGEDOWN_SCANCODE) {	//pg down
			pageDown();
		}
		else if (input_char == PAGEUP_SCANCODE) {				//pg up
			printf_system(DEFAULT_SYSTEMCOlOR,"\r\n%s\n", "hello world");
			userNewLine(true);
		}


		else if (input_char == DOWN_SCANCODE) {			//down
			writeToScreen(cmd_previous_command);
		}
		else if (input_char == UP_SCANCODE) { //up
			char* newLine;
			strcpy(newLine, "                                                                               ");
			printf("\r%s", newLine);
			userNewLine(false);

		}

		else
		{
			while (screen_y_value >= 18)
				pageDown();
			input_length++;
			if (input_index == input_length - 1)
				goto print_char;

			int temp = input_length;
			while (input_length > input_index) {
				cmd_string[input_length - 1] = cmd_string[input_length - 2];
				input_length--;
			}
			input_length = temp;
			
			print_char:
				cmd_string[input_index] = input_char; //saving user input
				input_index++;

			setCursorPosition(staring_point);
			printf("%s", cmd_string);
			setCursorPosition(staring_point + input_index);
		end: continue;
		}
	}
	return;
}

int userNewLine(bool new_line) {
	if (new_line == true) {
		printf_system(DEFAULT_SYSTEMCOlOR,"%s\n", "");
	}
	printf_system(MAGENTA_SYSTEMCOlOR, "\r%s$ ", currPath);
	staring_point = getCursorPosition();
	memset(cmd_string, 0, input_length);
	input_length = 0;
	input_index = 0;
	return 0;
}



void writeToScreen(char* str)
{	
	uint_8* charPtr = (uint_8*)str;
	while (*charPtr != 0) {
		input_length++;

		cmd_string[input_index] = *charPtr; //saving user input
		input_index++;
		setCursorPosition(staring_point);
		printf("%s", cmd_string);
		setCursorPosition(staring_point + input_index);
		charPtr++;
	}
}


//should be syscall
void print_physical_memory_info(void) {
	uint_32 num_entries = *(uint_32*)0x8500;
	SMAP_entry_t* SMAP_entry = (SMAP_entry_t*)0x8504;

	setCursorPosition(800);  //for now, print to the middle of the screen
	for (uint_32 i = 0; i < num_entries - 1; i++) {
		printf_system(DEFAULT_SYSTEMCOlOR, "Reigon %x", i);
		printf_system(DEFAULT_SYSTEMCOlOR, ", Base %x", SMAP_entry->base_address);
		printf_system(DEFAULT_SYSTEMCOlOR, ", Length %x", SMAP_entry->length);
		printf_system(DEFAULT_SYSTEMCOlOR, ", Type %x", SMAP_entry->type);
		switch (SMAP_entry->type)
		{
		case 1:
			printf_system(DEFAULT_SYSTEMCOlOR, "\r%s", " (Avialable)");
			break;
		case 2:
			printf_system(DEFAULT_SYSTEMCOlOR, "\r%s", " (Reserved)");
			break;
		case 3:
			printf_system(DEFAULT_SYSTEMCOlOR, "\r%s", " (ACPI Rclaim)");
			break;
		case 4:
			printf_system(DEFAULT_SYSTEMCOlOR, "\r%s", " (ACPI NVC Rclaim)");
			break;
		default:
			printf_system(DEFAULT_SYSTEMCOlOR, "\r%s", " (Reserved)");
			break;
		}
		SMAP_entry++;     //next entry to print
		puts("\n\r");
	}

	SMAP_entry--;      //get the last entry
	SMAP_entry--;     //temporary.. 
	printf_system(DEFAULT_SYSTEMCOlOR, "\nTotal memory in bytes: %x\n", SMAP_entry->base_address + SMAP_entry->length - 1);
	printf_system(DEFAULT_SYSTEMCOlOR, "Total memory in 4KB blocks: %x\n", max_blocks);
	printf_system(DEFAULT_SYSTEMCOlOR, "Used of reserved blocks: %x\n", used_blocks);
	printf_system(DEFAULT_SYSTEMCOlOR, "Free of avaliable blocks: %x\n", max_blocks - used_blocks);
}


void pageDown()
{
	int curr_point = staring_point;
	initScreen();
	if (curr_point == screen_start_point) {
		userNewLine(false);
		return;
	}
	memset(screen_charecter[0], 0, VGA_WIDTH);
	memset(screen_charecter[screen_y_value], 0, VGA_WIDTH);
	for (int z = 0; z < VGA_HIGHT-1; z++)
	{
		for (int i = 0; i < VGA_WIDTH; i++)
		{
			//TODO: change to deep copy!!!
			screen_charecter[z][i] = screen_charecter[z + 1][i];
			if (screen_charecter[z][i].data != 0)
				putc_color(screen_charecter[z][i].data, screen_charecter[z][i].params);
			else
				printf("%c", screen_charecter[z][i].data);
		}

	}
	screen_y_value -= 2;
	setCursorPosition(curr_point - 160);
	strcpy(cmd_previous_command, cmd_string);
	userNewLine(true);
	writeToScreen(cmd_previous_command);
}

//! install gdtr
void gdt_install() {
	InstallGDT();
}