#ifndef SHELL_HEADER
#define SHELL_HEADER

#define COMMAND_AMOUNT 20
#define MAXIMUM_PARAMS 20
#define MAXIMUM_PARAM_LENGTH 40
#define MAXIMUM_CMD_LEN 100
#define CMD_NAME_LEN 20

#define FILE_OPENED_SUCCESSFULLY 1
#define ERROR_OPENING_FILE 0
#define GENERAL_ERROR 2
#define FILE_DOES_NOT_EXITS 3

#include "vga_print.h"
#include "../includes/FAT.h"
#include "interpreter.h"
#include "kernel.h"

typedef int (*shellFunctionPtr) ();

typedef struct{
	char* commandName;
	shellFunctionPtr function_ptr;
	int maxParams;
	int minParams;
}__attribute__((packed)) command_entry;

int initializeShell();
int parser_command(char* command);

int _parser_params(char* params);


//list of commands
int shell_help();		//Display information about builtin commands
int shell_touch();		//change file timestamps
int shell_mkdir();		//make directories
int shell_cat();		//concatenate files and print on the standard output
int shell_echo();		//display a line of text
int shell_rm();			//remove file and folder
int shell_cd();			//change directory
int shell_mv();			//move (rename) files
int shell_ls();			//list directory contents
int shell_nano();		//file editor
int shell_clear();		//clear the terminal screen
int shell_exit();		//cause normal process termination

int interpreter_shell(char* command);		//main function for any interpreter command


//private methods
int _createFile(const char* file, const int isDir);
int touchMkdir(int isDir);								//touch or mkdir(one parame diffrent)
uint_16 getFirstCluster(char* path);					//returns the first cluster the the given path
int backNevaigate();									//special case of cd: "cd .."
bool isValidName(char* fileName);						//checks validation of file name
bool isValidExtension(char* extension);					//checks validation of file extension
int getCurrentParam(char* dest, char* command);			//parse the current part of the command, part of the main parser function
FILE* getFile(char* filename, int isDir, int* status);	//search file in current directory
bool isOpenedFile(FILE* file);							//checks if values are not null
bool isInterpreterCommand(char* command);				//checks is command is interpreter command
int firstSign(char* str);								//first index of operation sign
#endif // !SHELL_HEADER
