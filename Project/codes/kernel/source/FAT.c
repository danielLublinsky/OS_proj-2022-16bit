#include "../includes/FAT.h"

BootSector bootSector;
FILE rootDirectory[16];				//should update this variable + the original root on disk when user updates the root
uint_16 FAT_bitMap[500];
extern char* currPath;
int globalLastBitMap = 400;
int indexFromBitMap = 0;
extern FILE* fileOpened;

void InitializeBootSector()
{
	bootSector.FAT_LBA = 3000;				//start of FAT
	bootSector.bytesPerSector = 512;
	bootSector.headsPerCylinder = 16;
	bootSector.rootEntries = 16;
	bootSector.sectorsPerCluster = 1;
	bootSector.sectorsPerFat = 500;
	bootSector.sectorsPerTrack = 63;
	bootSector.totalSectors = 204624;     //C * H * S
}

void InitializeFileSystem()
{
	//strncpy(currPath, "root", 4);
	//rootDirectory = malloc(bootSector.bytesPerSector);
	read_sectors_ATA_PIO(FAT_bitMap, BIT_MAP_LBA, 1);
	read_sectors_ATA_PIO(rootDirectory, bootSector.FAT_LBA + bootSector.sectorsPerFat, 1);
	//memset(FAT_bitMap, 0, bootSector.sectorsPerFat);
	FAT_bitMap[0] = 1;			//index 0 is the rootDirectory, shloud be save
}

FILE* fopen(const char* filename, const char* mode)
{
	//dir1/dir2/file.txt
	read_sectors_ATA_PIO(rootDirectory, bootSector.FAT_LBA + bootSector.sectorsPerFat, 1);
	char* path, subPath, lba;
	uint_16 FAT_cluster = 0;
	uint_8 isDir = 0;
	int isOnlyOneDirectory = 0;

	path = (char*)malloc(strlen(filename));
	strncpy(path, filename, strlen(filename));
	subPath = strtok(path, '/');			//parent Dir
	path = path + strlen(subPath) + 1;

	//free(subPath);
	subPath = strtok(path, '/');			//parent Dir

	if (strlen(subPath) == 0) 		//if is the last file on path
		isOnlyOneDirectory = 1;		//path sould be the contect of the last file

	//path = path + strlen(subPath) + 1;
	//read the Root directory
	FILE* curr_file = rootDirectory;

	//read_sectors_ATA_PIO(curr_file, bootSector.FAT_LBA + bootSector.sectorsPerFat, 1);
	
	//printf("path- %s, %d\n\r", path, isOnlyOneDirectory);
	int count = 0;
	while (true) {
		//printf("%s ", curr_file->fileName);
		if (strcmp(subPath, curr_file->fileName) == 1) {
			break;
		}
		if (count == 16)
		{
			return -1;
		}
		count++;
		curr_file++;
	}

	if (curr_file->firstCluster == 0) {

		return -1;			//not found
	}

	isDir = IS_DIR(curr_file->attributes);	
	count = strCount(path, '/');
	if (count == 0) return curr_file;

	//at this point, we know it's a folder and we already found the parent folder on root
	
	//dir1/dir2/file.txt
	FILE* curr_fileFAT;
	int found = 0;
	path = path + strlen(subPath) + 1;
	count = strCount(path, '/') + 1;
	
	for(int i = 0; i < count; i++) {
		found = 0;
		isDir = IS_DIR(curr_file->attributes);
		subPath = strtok(path, '/');

		if (isDir) {	//for dir: should be only one index of cluster list- maximum 16 files on one folder
			//memset(curr_file, 0, 16);	  //clear the buffer
			read_sectors_ATA_PIO(curr_file, bootSector.FAT_LBA + bootSector.sectorsPerFat + curr_file->firstCluster, 1);
			int countFiles = 0;
			while (true) {	//loop over the 16 entries or till we got empty entry(end of folder)
				if (strcmp(subPath, curr_file->fileName) == 1) {
					subPath = strtok(path, '/');			//parent Dir
					path = path + strlen(subPath) + 1;
					found = 1;
					break;
				}
				if (countFiles == 16)
				{
					return -1;
				}

				countFiles++;
				curr_file++;
			}
			if (!found) {
				return 0;		//sub dir or file does not in this folder
			}
		}
		else {			//it's file
			//at this case we actually done and found the target file
			return curr_file;
		}
	}

	return curr_file;	//return the DIR cluster- all the subPath's was a folders(includes the last one)
}

uint_32 fread(void* buffer, uint_32 size, uint_32 count, FILE* stream)
{
	//IMPORANT: when allocating buffer, DO NOT allocate the file size! we should allocating by block size alighnment 
	uint_16* collection = collectFileEnries(stream->firstCluster, stream->fileSize);
	int len = sizeof(collection) / sizeof(uint_16) - 1;		//-1 for the EOF cluster

	for (int i = 0; i < len; i++) {
		read_sectors_ATA_PIO(buffer, bootSector.FAT_LBA + bootSector.sectorsPerFat + *collection, 1);
		//all the cases should be the size of 1 sector exept the rest of the last sector 
		buffer += bootSector.bytesPerSector;
		collection++;
	}
	
	return 1;			//current support is only 1 count
}

uint_32 fwrite(const void* ptr, uint_32 size, uint_32 nmemb, FILE* stream)
{
	void* tempPtr = ptr;
	int sizeSector = bootSector.bytesPerSector;
	uint_16* collection;
	if (stream->fileSize > 0) {
		collection = collectFileEnries(stream->firstCluster, stream->fileSize);
		deletedataFile(collection);
	}

	uint_32 remainder = size % bootSector.bytesPerSector;
	uint_32 ClusterCount = size / bootSector.bytesPerSector;
	ClusterCount = size % bootSector.bytesPerSector != 0 ? ClusterCount + 1 : ClusterCount;
	uint_16 cluster = stream->firstCluster;
	static char* cluster_str;// = (char*)malloc(bootSector.bytesPerSector);
	uint_16 lastCluster = cluster;
	static char tempDataCluster[512];// = malloc(bootSector.bytesPerSector);
	
	for (int i = 0; i < ClusterCount; i++)
	{
		memset(tempDataCluster, 0, bootSector.bytesPerSector);

		if (i == ClusterCount - 1) {
			strncpy(tempDataCluster, tempPtr, remainder);
		}

		else {
			strncpy(tempDataCluster, tempPtr, bootSector.bytesPerSector);
			tempPtr += (sizeof(tempDataCluster) / sizeof(char));
		}
		write_sectors_ATA_PIO(bootSector.FAT_LBA + bootSector.sectorsPerFat + cluster , 1, tempDataCluster);
		memset(cluster_str, 0, bootSector.bytesPerSector);
		lastCluster = cluster;
		cluster = findFirstFreeCluster();
		itoa(cluster, cluster_str, 10);
		write_sectors_ATA_PIO(bootSector.FAT_LBA + lastCluster, 1, cluster_str);
		signCluster(cluster, 1);
		memset(cluster_str, 0, bootSector.bytesPerSector);
	}
	
	//the last cluster is always the EOF
	memset(cluster_str, 0, bootSector.bytesPerSector);
	strncpy(cluster_str, "4088", 4);
	write_sectors_ATA_PIO(bootSector.FAT_LBA + cluster, 1, cluster_str);

	return 1;
}


int createFile(FILE* stream, uint_16 parentCluster) {
	//uint_16 firstCluster = 0;
	FILE* parentDir;
	uint_16 parent = parentCluster;
	read_sectors_ATA_PIO(parentDir, bootSector.FAT_LBA + bootSector.sectorsPerFat + parent, 2);  //read all parent DIR
	
	FILE* firstPtr = parentDir;
	
	int i = 0;
//	print(fileOpened);

	//print(parentDir);
	while (firstPtr->firstCluster != 0) {
		i++;
		firstPtr++;
	}
	
	if (i == 16) {
		printf("fffffffffffffffffffff");

		return 0;			//not enougth space in parent DIR
	}
	//print(fileOpened);
	copyStream(firstPtr, fileOpened);
	write_sectors_ATA_PIO(bootSector.FAT_LBA + bootSector.sectorsPerFat + parentCluster, 1, parentDir);
	read_sectors_ATA_PIO(rootDirectory, bootSector.FAT_LBA + bootSector.sectorsPerFat, 1);	//any case, read to ram
	
	char eof[512];
	memset(eof, 0, bootSector.bytesPerSector);
	strncpy(eof, "4088", 4);
	write_sectors_ATA_PIO(bootSector.FAT_LBA + fileOpened->firstCluster, 1, eof);

	return 1;
}

uint_16* collectFileEnries(const uint_16 firstCluster, uint_32 fileSize)
{
	static uint_16 clusterList[512];// = (uint_16*)malloc(fileSize / 512 + (fileSize % 512;		//allocating fileSize in blocks
	uint_16 currCluster = firstCluster;
	static uint_8 readedCluster[512];
	clusterList[0] = currCluster;
	int index = 1;

	int count = 0;
	while (currCluster != 4088) {		//FF8 in decimal
		if (count == 3)	return firstCluster;			//not found, some error occored
		read_sectors_ATA_PIO(readedCluster, bootSector.FAT_LBA + currCluster, 1); 	//read value in this cluster and then updte it
		currCluster = atoi(readedCluster);
		clusterList[index] = currCluster;
		index++;
		memset(readedCluster, 0, bootSector.bytesPerSector);
		count++;
	}

	//free(readedCluster);
	return clusterList;
}

uint_16 findFirstFreeCluster()
{
	read_sectors_ATA_PIO(FAT_bitMap, BIT_MAP_LBA, 1);
	for (int i = 1; i < bootSector.sectorsPerFat; i++) {
		if (FAT_bitMap[i] == 0)	return i;		//found free cluster
	}

	indexFromBitMap++;
	return globalLastBitMap + indexFromBitMap;	//not enougth spase on all over the file system, very rare case
}

uint_32 signCluster(uint_16 cluster, uint_16 sign)
{
	if (FAT_bitMap[cluster])
		return 0;
	FAT_bitMap[cluster] = sign;
	
	write_sectors_ATA_PIO(BIT_MAP_LBA, 1, FAT_bitMap);	//update disk
	return 1;
}

int deletedataFile(uint_16* clusterList)
{
	int size = sizeof(clusterList) / sizeof(uint_16);
	uint_16 *emptyCluster = (uint_16*)malloc(bootSector.bytesPerSector);
	memset(emptyCluster, 0, bootSector.bytesPerSector);
	for (int i = 0; i < size; i++)
	{
		FAT_bitMap[clusterList[i]] = 0;
		//printf("cluster in delete- %d\n\r", clusterList[i]);
		write_sectors_ATA_PIO(bootSector.FAT_LBA + clusterList[i], 1, emptyCluster);
		write_sectors_ATA_PIO(bootSector.FAT_LBA + bootSector.sectorsPerFat + clusterList[i], 1, emptyCluster);
	}
	
	write_sectors_ATA_PIO(BIT_MAP_LBA, 1, FAT_bitMap);	//update bitMap on disk

	return 1;
}

int updateStream(FILE* stream, uint_16 firstCluster)
{
	write_sectors_ATA_PIO(bootSector.FAT_LBA + bootSector.sectorsPerFat + firstCluster, '1', stream);

	return 1;
}

void copyStream(FILE* dest, const FILE* src)
{
	dest->attributes = src->attributes;
	dest->createdTime = src->createdTime;
	dest->createdYear = src->createdYear;
	dest->createInMs = src->createInMs;
	dest->EA_Index = src->EA_Index;
	strncpy(dest->fileExtension, src->fileExtension, 2);
	strncpy(dest->fileName, src->fileName, 7);
	dest->fileSize = src->fileSize;
	dest->firstCluster = src->firstCluster;
	dest->lastAccessDate = src->lastAccessDate;
	dest->lastModifiedDate = src->lastModifiedDate;
	dest->lastModifiedTime = src->lastModifiedTime;
	dest->unused = src->unused;
}

FILE* getFilesOfDirectory(uint_16 firstCluster)
{
	static FILE* files;
	read_sectors_ATA_PIO(files, bootSector.FAT_LBA + bootSector.sectorsPerFat + firstCluster, 1);

	return files;
}

uint_8 rm_FAT(char* filename, FILE* files)
{
	char fileName[MAXIMUM_FILE_LENGTH];
	char fileExtension[5];
	static char* ptr;

	if (strCount(filename, '.') > 0)			//get extension from a file name
	{
		strncpy(fileName, filename, strFind(filename, '.'));
		ptr = filename + strFind(filename, '.') + 1;
		strncpy(fileExtension, ptr, 2);				//copy the extenssion only
	}
	else
		strcpy(fileName, filename);

	//while not found the file
	int count = 0;
	while (true)
	{
		if (count == 15)	return 0;

		if (strcmp(files->fileName, fileName) == 1)
		{
			if (IS_DIR(files->attributes))
				break;						//found folder

			if (strcmp(files->fileExtension, fileExtension) == 1)
				break;						//found file
		}

		files++;
		count++;
	}

	signCluster(files->firstCluster, 0);	//unsign in bitMap
	memset(files, '\0', 32);				//sign as empty
	return 1;
}

void print(FILE* stream)
{
	printf("FILE:\n\r");
	printf("attributes-%d, ", stream->attributes);
	printf("createdTime-%d, ", stream->createdTime);
	printf("createdYear-%d, ", stream->createdYear);
	printf("createInMs-%d, ", stream->createInMs);
	printf("EA_Index-%d\n\r", stream->EA_Index);
	printf("fileExtension-%s, ", stream->fileExtension);
	printf("fileName-%s, ", stream->fileName);
	printf("fileSize-%d, ", stream->fileSize);
	printf("firstCluster-%d, ", stream->firstCluster);
	printf("lastAccessDate-%d\n\r", stream->lastAccessDate);
	printf("lastModifiedDate-%d, ", stream->lastModifiedDate);
	printf("lastModifiedTime-%d, ", stream->lastModifiedTime);
	printf("unused-%d\n\r", stream->unused);
}


int updateParentDir(char* parentPath, FILE* stream)
{
	int firstCluster = 0;
	int isRoot = 1;
	if (strCount(parentPath, '/') != 0)    //if parent is not the root directory
	{
		FILE* parent = fopen(parentPath, "");
		firstCluster = parent->firstCluster;
		isRoot = 0;
	}

	FILE* parentData;
	//char* ptr = (char*)FILE*;
	memset(parentData, 0, bootSector.bytesPerSector);
	FILE* parent_ptr = parentData;
	read_sectors_ATA_PIO(parentData, bootSector.FAT_LBA + bootSector.sectorsPerFat + firstCluster, 1);
	while (strcmp(parent_ptr->fileName, stream->fileName) != 1) {
		parent_ptr++;
	}
	copyStream(parent_ptr, stream);	//update the actual list files/dirs on parent DIR with the new stream

	write_sectors_ATA_PIO(bootSector.FAT_LBA + bootSector.sectorsPerFat + firstCluster, 1, parentData);  //update on disk
	if (isRoot)    //if parent is the root directory
	{
		read_sectors_ATA_PIO(rootDirectory, bootSector.FAT_LBA + bootSector.sectorsPerFat, 1);
	}
	//else
		//free(parentData);	//after writhng to disk we don't need this information anymore			

	return 1;
}


uint_8 deleteFile(const uint_8* fileName, const uint_8* path)
{
	FILE* dir = fopen(path, "");
	if (dir == -1)	return -1;

	uint_16* clusters;

	FILE* curr;
	int* ptr = curr;
	read_sectors_ATA_PIO(curr, bootSector.FAT_LBA + bootSector.sectorsPerFat + dir->firstCluster, 1);
	int i = 0;
	for (i = 0; i < 16; i++) {
		if (strcmp((*curr).fileName, fileName)) {
			clusters = collectFileEnries(curr->firstCluster, curr->fileSize);
			break;
		}
		curr++;
	}
	for (; i < 15; i++) {
		copyStream(curr, curr+1);
		curr++;
	}
	memset(curr, 0, (sizeof(FILE) / sizeof(uint_8)));		//fill the entry with 0's
	write_sectors_ATA_PIO(bootSector.FAT_LBA + bootSector.sectorsPerFat + dir->firstCluster, 1, ptr);
	deletedataFile(clusters);

	read_sectors_ATA_PIO(rootDirectory, bootSector.FAT_LBA + bootSector.sectorsPerFat, 1);	//any case, read to ram
}