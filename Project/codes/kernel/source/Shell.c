#include "../includes/Shell.h"

static command_entry shellFuncArray[COMMAND_AMOUNT];
int numberOfParams = 0;
char currPath[255];		//path from kernel
static char params_array[MAXIMUM_PARAMS][MAXIMUM_PARAM_LENGTH];
bool cleared = false;
FILE* fileOpened;

int initializeShell()
{
	strcpy(currPath, "root@-");

	shellFuncArray[0].commandName = (char*)malloc(4);
	strcpy(shellFuncArray[0].commandName, "help");
	shellFuncArray[0].function_ptr = shell_help;
	shellFuncArray[0].maxParams = 0;
	shellFuncArray[0].minParams = 0;

	shellFuncArray[1].commandName = (char*)malloc(5);
	strcpy(shellFuncArray[1].commandName, "touch");
	shellFuncArray[1].function_ptr = shell_touch;
	shellFuncArray[1].maxParams = 3;
	shellFuncArray[1].minParams = 1;

	shellFuncArray[2].commandName = (char*)malloc(5);
	strcpy(shellFuncArray[2].commandName, "mkdir");
	shellFuncArray[2].function_ptr = shell_mkdir;
	shellFuncArray[2].maxParams = 3;
	shellFuncArray[2].minParams = 1;

	shellFuncArray[3].commandName = (char*)malloc(2);
	strcpy(shellFuncArray[3].commandName, "ls");
	shellFuncArray[3].function_ptr = shell_ls;
	shellFuncArray[3].maxParams = 0;
	shellFuncArray[3].minParams = 0;

	shellFuncArray[4].commandName = (char*)malloc(2);
	strcpy(shellFuncArray[4].commandName, "cd");
	shellFuncArray[4].function_ptr = shell_cd;
	shellFuncArray[4].maxParams = 1;
	shellFuncArray[4].minParams = 1;

	shellFuncArray[5].commandName = (char*)malloc(4);
	strcpy(shellFuncArray[5].commandName, "echo");
	shellFuncArray[5].function_ptr = shell_echo;
	shellFuncArray[5].maxParams = 3;
	shellFuncArray[5].minParams = 3;

	shellFuncArray[6].commandName = (char*)malloc(3);
	strcpy(shellFuncArray[6].commandName, "cat");
	shellFuncArray[6].function_ptr = shell_cat;
	shellFuncArray[6].maxParams = 1;
	shellFuncArray[6].minParams = 1;

	shellFuncArray[7].commandName = (char*)malloc(2);
	strcpy(shellFuncArray[7].commandName, "rm");
	shellFuncArray[7].function_ptr = shell_rm;
	shellFuncArray[7].maxParams = 3;
	shellFuncArray[7].minParams = 1;

	shellFuncArray[8].commandName = (char*)malloc(2);
	strcpy(shellFuncArray[8].commandName, "clear");
	shellFuncArray[8].function_ptr = shell_clear;
	shellFuncArray[8].maxParams = 0;
	shellFuncArray[8].minParams = 0;
}

int parser_command(char* command)
{
	if (strlen(command) < 1)	return 0;				//invalid input- empty command
	char* command_cpy = command;
	
	if (isInterpreterCommand(command_cpy))
		return interpreter_shell(command_cpy);

	char* cmd_name = my_strtok(command_cpy, ' ');		//get the command itshelf
	char* command_ptr = command_cpy + strlen(cmd_name);
	deleteSpaces(command_ptr);
	numberOfParams = 0;
	printf_system(DEFAULT_SYSTEMCOlOR,"");		//don't delete this line, it's help to sync the processor between this two lines- important!!
	
	int status = _parser_params(command_ptr);
	if (status != 1)	goto end;

	int i = 0;
	for (i = 0; i < COMMAND_AMOUNT; i++) {
		if (strcmp(cmd_name, shellFuncArray[i].commandName) == 1) {	//command found
			if (numberOfParams > shellFuncArray[i].maxParams || numberOfParams < shellFuncArray[i].minParams) {
				printf_system(DEFAULT_SYSTEMCOlOR, "%s", "\n\r");
				printf_system(DEFAULT_ERORR_SYSTEMCOlOR, "%s\r", "-shell: arguments not in range!");
				printf_system(DEFAULT_ERORR_SYSTEMCOlOR, "num- %d\r", numberOfParams);
				goto end;
			}

			shellFuncArray[i].function_ptr(params_array);	//call the command handler

			goto end;
		}
	}
	
	printf_system(DEFAULT_SYSTEMCOlOR, "%s", "\n\r");
	printf_system(DEFAULT_ERORR_SYSTEMCOlOR, "%s\r", "-shell: Command not found!");

end:
	free(cmd_name);

	if (status == 0)					//some error was occured		
	{
		return status;
	}
	
	//printf_system(DEFAULT_SYSTEMCOlOR, "\n\rname- %s\n\r", cmd_name);
	//for (int j = 0; j < numberOfParams; j++) {
	//	printf_system(DEFAULT_SYSTEMCOlOR, "\n\r|%s|", params_array[j]);
	//}
	//printf_system(DEFAULT_SYSTEMCOlOR, "\r");
	
	//for (int j = 0; j < numberOfParams; j++) {
	//	free(params_array[i]);
	//}
	for (int j = 0; j < MAXIMUM_PARAMS; j++) {
		memset(params_array[j], 0, MAXIMUM_PARAM_LENGTH);
	}
	//free(params_array);

	return 0;
}

int _parser_params(char* params)
{
	char* ptr = params;
	numberOfParams = 0;
	bool foundFirst_apostrophes = false;					//find the " sign for the first time

	while(*ptr) 
	{
		while (*ptr == ' ') ptr++;
		static char* currParam;
		int status = getCurrentParam(currParam, ptr);		//get the current parameter from command

		if (status != 1)	return status;

		memset(params_array[numberOfParams], 0, MAXIMUM_PARAM_LENGTH);	
		strcpy(params_array[numberOfParams], currParam);

		ptr += strlen(currParam) + 1;						//next command
		//printf("\n\r%s", "second2");
		deleteSpaces(ptr);									//clear spaces
		//printf("\n\r%s", "second3");

		numberOfParams++;
	}
	return 1;
}


int shell_help(char** params)
{
	printf_system(DEFAULT_SYSTEMCOlOR, "\n\r%s\n\r", "*******Official Help file:*******");
	printf_system(DEFAULT_SYSTEMCOlOR,"\n\r%s", "Usage- [command] [parameters]");
	printf_system(DEFAULT_SYSTEMCOlOR,"\n\r%s", "help- display functions help");
	printf_system(DEFAULT_SYSTEMCOlOR, "\n\r%s", "clear- clear the terminal screen");
	printf_system(DEFAULT_SYSTEMCOlOR,"\n\r%s", "touch- change file timestamps");
	printf_system(DEFAULT_SYSTEMCOlOR,"\n\r%s", "mkdir- make directories");
	printf_system(DEFAULT_SYSTEMCOlOR,"\n\r%s", "cat- concatenate files and print on the standard output");
	printf_system(DEFAULT_SYSTEMCOlOR,"\n\r%s", "echo- display a line of text");
	printf_system(DEFAULT_SYSTEMCOlOR,"\n\r%s", "rm- remove file&folder");
	printf_system(DEFAULT_SYSTEMCOlOR,"\n\r%s", "cd- change directory");
	printf_system(DEFAULT_SYSTEMCOlOR,"\n\r%s", "ls- list directory contents");
}

int shell_touch()
{
	touchMkdir(0);
}

int shell_mkdir()
{
	touchMkdir(1);
}

int shell_cat()
{
	if (strCount(params_array[0], '.') != 1) {
		return 0;
	}

	int status = 1;
	FILE* files = getFile(params_array[0], 0, &status);		//get file (the flag 0 means file)
	if (status != FILE_OPENED_SUCCESSFULLY) {				//file is not exist
		printf_system(DEFAULT_ERORR_SYSTEMCOlOR, "\n\r%s", "-shell: file not found");
		return 0;
	}
	char* data;

	fread(data, 0, 1, files);
	printf_system(DEFAULT_SYSTEMCOlOR, "\n\r%s", data);
	return 1;
}

int shell_echo()
{
	char* oldData;
	FILE fileTemp;

	if (strCount(params_array[2], '.') != 1) {
		return 0;
	}
	int status = 1;
	FILE* fileObj = getFile(params_array[2], 0, &status);

	copyStream(&fileTemp, fileObj);
	if (status != FILE_OPENED_SUCCESSFULLY) {	//file is not exist
		printf_system(DEFAULT_ERORR_SYSTEMCOlOR, "\n\r%s", "-shell: file not found");
		return 0;				
	}

	static char* dataToWrite = params_array[0] + 1;		//'remove' the open quotation marks
	dataToWrite[strlen(dataToWrite) - 1] = '\0';		//'remove' the close quotation marks

	if (strcmp(params_array[1], ">") == 1) 			//override
	{
		fwrite(dataToWrite, strlen(dataToWrite), 0, fileObj);
	}
	else if (strcmp(params_array[1], ">>") == 1)	//overload
	{
		// read and append the new data right after the old data and then write it back
		fread(oldData, 0, 1, fileObj);

		//dataToWrite[strlen(dataToWrite)] = '\0';
		oldData = append(oldData, dataToWrite);

		fwrite(oldData, strlen(oldData), 0, &fileTemp);
	}
	else {
		printf_system(DEFAULT_ERORR_SYSTEMCOlOR, "%s\r", "-shell: echo syntax error");
		return 0;
	}

	return 1;
}

int shell_rm()
{
	uint_16 firstCluster = getFirstCluster(currPath);
	FILE* files = getFilesOfDirectory(firstCluster);
	FILE* filePtr = files;

	for (int i = 0; i < numberOfParams; i++) 
	{
		int status = rm_FAT(params_array[i], files);
		if (status != 1)
			printf_system(DEFAULT_ERORR_SYSTEMCOlOR, "\n\r-shell: file '%s' cannot be removed", params_array[i]);
	}

	updateStream(filePtr, firstCluster);		//update disk
	return 1;
}


int interpreter_shell(char* command)
{
	int status = _parser_params(command);
	if (status != 1)	return status;

	return calc(command);
}

int _createFile(const char* file, const int isDir)
{
	char fileName[strlen(file)];
	char* extensionPtr;
	char* fileCpy;

	if (isDir == 0) {
		if (strCount(file, '.') == 0) return 0;

		fileCpy = file;
		strncpy(fileName, file, strFind(file, '.'));
		extensionPtr = fileCpy + strlen(fileName) + 1;
		if (!isValidExtension(extensionPtr)) {
			printf_system(DEFAULT_ERORR_SYSTEMCOlOR, "\n\r%s", "-shell: Extension is not valid");
			return 0;
		}
	}

	else 
		strcpy(fileName, file);
	
	if (strlen(fileName) >= 8) {
		printf_system(DEFAULT_ERORR_SYSTEMCOlOR, "\n\r%s", "-shell: name error: length is over 7");
		return 0;
	}

	if (!isValidName(fileName)) {
		printf_system(DEFAULT_ERORR_SYSTEMCOlOR, "\n\r%s", "-shell: name error: Alphabetic/numbers characters only");
		return 0;
	}
	//FILE* fileOpened = (FILE*)malloc(512);
	fileOpened->attributes = isDir ? 16 : 0;		//dir	
	fileOpened->createdTime = 0;
	fileOpened->createdYear = 0;
	fileOpened->createInMs = 0;
	fileOpened->createInMs = 0;
	fileOpened->EA_Index = 0;
	strncpy(fileOpened->fileExtension, extensionPtr, 2);
	strncpy(fileOpened->fileName, fileName, 7);
	fileOpened->fileSize = 0;
	fileOpened->firstCluster = -1;			//FAT should take care of the fistCluster
	fileOpened->lastAccessDate = 0;
	fileOpened->lastModifiedDate = 0;
	fileOpened->lastModifiedTime = 0;
	fileOpened->unused = 0;
	fileOpened->firstCluster = findFirstFreeCluster();
	signCluster(fileOpened->firstCluster, 1);

	return 1;
}

int touchMkdir(int isDir)
{
	int parentCluster = 0;
	int status = 1;

	if (strCount(currPath, '/') > 0)				//if root directory is not the parent directory
	{
		parentCluster = fopen(currPath, 'w')->firstCluster;
	}

	for (int i = 0; i < numberOfParams; i++) {
		//first, check if file/folder is already found
		status = 1;
		getFile(params_array[i], isDir, &status);
		if (status != FILE_DOES_NOT_EXITS)
		{
			printf_system(DEFAULT_ERORR_SYSTEMCOlOR, "\n\r-shell: can't create file- '%s'", params_array[i]);
		}

		else
		{
			status = _createFile(params_array[i], isDir);
			if (status == 1)  //creates only valid files
				createFile(fileOpened, parentCluster);	//fat now creates the actual file
			//free(fileOpened);
		}
	}
}

uint_16 getFirstCluster(char* path)
{
	uint_16 firstCluster = 0;

	if (strCount(currPath, '/') > 0) {
		FILE* dir = fopen(currPath, "r");
		firstCluster = dir->firstCluster;
	}
	return firstCluster;
}

bool isValidName(char* fileName)
{
	//Alphabetic characters only
	for (int i = 0; i < strlen(fileName); i++) {
		if (!(fileName[i] <= 'z' && fileName[i] >= 'a') &&
			!(fileName[i] <= 'Z' && fileName[i] >= 'A') &&
			!((fileName[i] <= '9' && fileName[i] >= '0')))
			return false;
	}
	return true;
}

bool isValidExtension(char* extension)
{
	return strcmp(extension, "txt") == 1 ? true : false;
}

//when find the '"' sign, this function parses the command and copy it into dest
int getCurrentParam(char* dest, char* command)
{
	char* ptr = command;
	char firstSign;
	int countCopy = 0;

	//case of string: "/' at the begining
	if (*ptr == '"' || *ptr == '\'')
	{
		firstSign = *ptr;				//save it, to find the close
		ptr++;							//skip the apostrophes

		while (*ptr != firstSign)		//search firstSign
		{
			countCopy++;
			ptr++;
		}

		ptr++;							//skip the apostrophes for the next check

		//in this point, should not be found any apostrophes at the rest of the command
		if (strCount(ptr, '"') != 0 || strCount(ptr, '\'') != 0) {
			printf_system(DEFAULT_ERORR_SYSTEMCOlOR, "%s\r", "-shell: Illegal amount of Apostrophes!");
			return 0;
		}

		ptr = command;					//base to points on the first latter to copy
		countCopy += 2;					//+2 is the double "/' at the begining and at the end
	}

	//normal case: normal parameter
	else 
	{
		ptr = my_strtok(ptr, ' ');
		countCopy = strlen(ptr);
		free(ptr);					//ptr was allocated into my_strtok
	}
	if (countCopy > MAXIMUM_PARAM_LENGTH)	return 0;

	strncpy(dest, ptr, countCopy);		
	
	return 1;
}


FILE* getFile(char* filename, int isDir, int* status)
{
	char fileName[MAXIMUM_PARAM_LENGTH];
	char fileExtension[5];
	char* ptr;

	if (isDir == 0)
	{
		if (strCount(filename, '.') != 1)
		{
			*status = ERROR_OPENING_FILE;	//error
			return 0;		//file must has an extension
		}

		strncpy(fileName, filename, strFind(filename, '.'));
		ptr = filename + strFind(filename, '.') + 1;

		if (strcmp(ptr, "txt") != 1)
		{
			*status = GENERAL_ERROR;	//error
			return 0;		//file must has an extension
		}
		strncpy(fileExtension, ptr, 2);

	}
	else
	{
		if (strCount(filename, '.') != 0) {
			*status = ERROR_OPENING_FILE;	//error
			return 0;	//folder without extension
		}

		strcpy(fileName, filename);
	}


	uint_16 firstCluster = getFirstCluster(currPath);
	FILE* files = getFilesOfDirectory(firstCluster);

	//while not found the file
	int count = 0;
	while (true)
	{
		if (count == 15) {
			*status = FILE_DOES_NOT_EXITS;	//error
			return 0;
		}
		
		if (strcmp(files->fileName, fileName) == 1)
		{
			if (isDir == 0 && strcmp(fileExtension, files->fileExtension) == 1)
				return files;

			if (isDir == 1)
			{
				*status = FILE_OPENED_SUCCESSFULLY;
				return files;
			}
		}
		files++;
		count++;
	}

	*status = ERROR_OPENING_FILE;	//error
	return 0;
}

bool isOpenedFile(FILE* file)
{
	//if file is null, those field should be with trash values and not 0
	return file->EA_Index == 0 && file->unused == 0 && file->firstCluster != 0;
}

bool isInterpreterCommand(char* command)
{
	if (strCount(command, '+') > 0 || strCount(command, '-') > 0 || strCount(command, '*') > 0 || strCount(command, '/') > 0)
		return true;

	return false;
}

int backNevaigate(int numberOfFolder)
{
	//updates the currPath
	if (strCount(currPath, '/') == 0 || (strCount(currPath, '/') < numberOfFolder))
		return 0;

	int count = 0;
	int i = strlen(currPath) - 1;

	while(true) {
		currPath[i] == '/' ? count++ : count;
		currPath[i] = '\0';
		if (count == numberOfFolder)
			return 1;
		i--;
	}
		
	return 0;
}

int shell_cd()
{
	//special option is the "cd .."
	if (params_array[0] == '.') return 1;		//stay on current directory
	if (strCount(params_array[0], '.') % 2 != 0) return 0;	//ivalid navigate
	if (strCount(params_array[0], '.') > 0)
		return backNevaigate(strCount(params_array[0], '.') / 2);

	char* tempPath;
	char* pathPtr = tempPath;
	strcpy(tempPath, currPath);
	tempPath = tempPath + strlen(tempPath);
	*tempPath = '/';
	tempPath++;

	strcpy(tempPath, params_array[0]);		//adding the directory to the temp path

	FILE* dir = fopen(pathPtr, "r");	//try to open that directory
	uint_16 firstCluster = dir->firstCluster;
	//print(dir);
	if (dir->unused != 0 || dir->EA_Index != 0) {	//in case that dir not found, or it's a file
		printf_system(DEFAULT_ERORR_SYSTEMCOlOR, "\n\r%s", "-shell: directory not found");
		return 0;
	}
	
	strcpy(currPath, pathPtr);			//update the main path
	
	return 1;
}

int shell_ls()
{
	printf_system(DEFAULT_SYSTEMCOlOR, "%s", "\n\r");

	uint_16 firstCluster = getFirstCluster(currPath);
	FILE* files = getFilesOfDirectory(firstCluster);
	int count = 0;
	while (true) {		//pass 16 files and folder in directory
		if (isOpenedFile(files))
		{
			printf_system(DEFAULT_SYSTEMCOlOR, "%s", files->fileName);
			if (IS_DIR(files->attributes) == 0)		//directery has not extension
				printf_system(DEFAULT_SYSTEMCOlOR, ".%st", files->fileExtension);

			printf_system(DEFAULT_SYSTEMCOlOR, " ");
		}
		if (count == 16) {
			break;
		}
		files++;
		count++;
	}
	printf_system(DEFAULT_SYSTEMCOlOR, "%s", "\r");

	return 0;
}

//clears the screen
int shell_clear()
{
	int pages = getCursorPosition() / 80 - 5;
	for(int i = 0; i < pages; i++)
		pageDown();

	cleared = true;

	return 1;
}
