#include "../includes/Physical_memory_manager.h"


// Global variable
uint_32* memory_map = 0;
uint_32 max_blocks = 0;
uint_32 used_blocks = 0;

//set bit in bit map
void  set_block(int bit) {
	memory_map[bit / 32] |= (1 << (bit % 32));
}

//unset bit in bit map
void  unset_block(int bit) {
	memory_map[bit / 32] &= ~(1 << (bit % 32));
}

//test if the specific bit is set
bool  test_block(int bit) {
	memory_map[bit / 32] &= (1 << (bit % 32));
}

//find the first unset(0) bit in the bitmap
int find_first_free_block()
{
	//! find the first free bit
	for (uint_32 i = 0; i < max_blocks / 32; i++)     //go over 32 memory's chunck
		if (memory_map[i] != 0xffffffff)
			for (int j = 0; j < 32; j++) {		//! test each bit in the dword

				int bit = 1 << j;
				if (!(memory_map[i] & bit))
					return i * 32 + j;
			}

	return -1;
}
#include "../includes/vga_print.h"
void init_memory_manager(uint_32 start_address, uint_32 size)
{
	memory_map = (uint_32*)start_address;
	max_blocks = size / BLOCK_SIZE;        //convert size to blocks
	used_blocks = max_blocks;            
	//setCursorPosition(800);
	//printf("%x     ", size);
	//printf("%x", max_blocks);
	//! By default, all of memory is in use
	memset(memory_map, 0xFF, max_blocks / BLOCK_PER_BYTE);
}

void init_memory_region(uint_32 base_address, uint_32 size)
{
	uint_32 align = base_address / BLOCK_SIZE;     //convert address to blocks
	uint_32 num_blocks = size / BLOCK_SIZE;        //convert size to blocks

	for (; num_blocks > 0; num_blocks--) {
		unset_block(align++);
		used_blocks--;
	}
	set_block(0);     //always going to be set, we can't alloc the first block it's for the interrupt table
					  //IVT/IDT, BIOS area
}

void deinit_memory_region(uint_32 base_address, uint_32 size)
{
	uint_32 align = base_address / BLOCK_SIZE;     //convert address to blocks
	uint_32 num_blocks = size / BLOCK_SIZE;        //convert size to blocks

	for (; num_blocks > 0; num_blocks--) {
		set_block(align++);
		used_blocks++;
	}
}

uint_32* alloc_block()
{
	if(max_blocks - used_blocks <= 0)
		return 0;	//out of memory

	int frame = find_first_free_block();

	if (frame == -1)
		return 0;	//out of memory

	set_block(frame);

	uint_32 addr = frame * BLOCK_SIZE;
	used_blocks++;

	return (uint_32*)addr;
}

void free_block(uint_32* p)
{
	int frame = (uint_32)p / BLOCK_SIZE;    //convert address to blocks  
	
	unset_block(frame);
	used_blocks--;
}