#ifndef STRING_HEADER
#define STRING_HEADER

#include "../source/typeDef.h"
#include "heap.h"

int strlen(char* str);
//void substr(char* str, int start, int end);
void swap(char* p1, char* p2);
void reverse(char str[], int length);
int itoa(int num, char* str, int base);
int atoi(char* str);
int strcpy(char* destination, const char* source);
int strncpy(char* dest, const char* src, uint_64 n);
int boolToChar(bool vIn, char* str);
int hexToString(uint_64 num, char* str);
void* memset(void* s, int c, uint_32 count);
void memcpy(void* destination, void* source, uint_64 num);
int strcmp(char* str1, char* str2);
int strncmp(char* str1, char* str2, int n);
char* strchr(const char* str, int c);
char* strtok(char* s, char d);
int strCount(char* str, const char c);		//countd how many times c found in str
char* deleteSpaces(char* str);		//delete spaces from begining & end
char* my_strtok(char* s, char d);
char* append(char* dest, char* src);		//append src into dest
int strFind(char* str, char c);				//looks for the first c in str
int isNumber(char* num);					//check if num is number
#endif