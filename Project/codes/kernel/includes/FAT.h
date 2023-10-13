#ifndef FAT_H
#define FAT_H

#include "../source/typeDef.h"
#include "../includes/heap.h"
#include "../includes/ata_disk.h"
#include "../includes/string.h"
#include "structs.h"
#include "heap.h"
#include "vga_print.h"

#define IS_DIR(b) (b << 4) >> 8				//gets byte(attribute) and check if it's a folder
#define BIT_MAP_LBA 2999	//one sector before the file system
#define END_OF_FILE "4088"
#define MAXIMUM_FILE_LENGTH 40

//memory struct:
/*
BootSector   |   FileAllocationTable   |   RootDirectory   |   DATA_SECTION
*/


//init functions
void InitializeBootSector();
void InitializeFileSystem();		//load FAT to memory

//general file system functions:
FILE* fopen(const char* filename, const char* mode);
uint_32 fread(void* buffer, uint_32 size, uint_32 count, FILE* stream);
uint_32 fwrite(const void* ptr, uint_32 size, uint_32 nmemb, FILE* stream);

//shell functions
int createFile(FILE* stream, uint_16 parentCluster);
uint_16* collectFileEnries(const uint_16 firstCluster, uint_32 fileSize);	//folowing the 'linked list' on FAT and build an array on this blocks numbers
uint_8 rm_FAT(char* fileName, FILE* files);			//remove file or folder
uint_8 deleteFile(const uint_8* fileName, const uint_8* path);		//delete file from DIR + call  to deletedataFile

//helper functions
uint_16 findFirstFreeCluster();
uint_32 signCluster(uint_16 cluster, uint_16 sign);	//sign cluster as used
int updateParentDir(char* parentPath, FILE* stream);		//updates *by searching the fileName*
int deletedataFile(uint_16* clusterList);		//delete data from file system
int updateStream(FILE* stream, uint_16 firstCluster);		//update disk with the new stream
void copyStream(FILE* dest, const FILE* src);		//deep copy of FILE
FILE* getFilesOfDirectory(uint_16 firstCluster);	//returns a list if FILE of all files in this directory
uint_8 signFileAsEmpty(FILE* file);					//fill with null
void print(FILE* stream);

#endif // !FAT_H