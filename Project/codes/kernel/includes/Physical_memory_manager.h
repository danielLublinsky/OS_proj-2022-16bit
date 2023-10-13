#ifndef PHYSICAL_MEMORY_MANAGER
#define PHYSICAL_MEMORY_MANAGER

#include "../source/typeDef.h"
#include "string.h"

#define BLOCK_SIZE 4096      //size of 1 block in memory- 4KB
#define BLOCK_PER_BYTE 8     //using bitmap- each byte will hold 8 bits/blocks

void     set_block(int bit);
void     unset_block(int bit);
bool     test_block(int bit);
int      find_first_free_block();
void     init_memory_manager(uint_32 start_address, uint_32 size);
void     init_memory_region(uint_32 base_address, uint_32 size);
void     deinit_memory_region(uint_32 base_address, uint_32 size);
uint_32* alloc_block();
void     free_block(uint_32* p);

#endif
