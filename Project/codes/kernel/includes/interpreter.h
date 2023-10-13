//interpreter.h

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "../source/typeDef.h"
#include "Shell.h"
#include "vga_print.h"
#include "heap.h"

#define ERROR_CALC -99999
#define NUMBER_OF_SIGNS 30

unsigned int extract_number(char* str, unsigned int endstr);
int power(int e, int n);
int expr(int n1, char symb, int n2);
int calc(char* str);

#endif