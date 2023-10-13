#ifndef STRUCTS_H
#define STRUCTS_H

//general system's information for init
typedef struct{
	uint_64 FAT_LBA;
	uint_16 bytesPerSector;
	uint_8 sectorsPerCluster;
	uint_16 rootEntries;
	uint_16 totalSectors;
	uint_16 sectorsPerFat;
	uint_16 sectorsPerTrack;
	uint_16 headsPerCylinder;
}BootSector;

typedef struct {
	uint_8 fileName[8];					//should be maximum 8 bytes		
	uint_8 fileExtension[3];				//should be maximum 3 bytes
	uint_8 attributes;					//attribute format
	uint_8 unused;						//should be 0
	uint_8 createInMs;
	uint_16 createdTime;				//time format
	uint_16 createdYear;				//year format
	uint_16 lastAccessDate;			//year format
	uint_16 EA_Index;				  //should be 0
	uint_16 lastModifiedTime;		  //time format
	uint_16 lastModifiedDate;		  //year format
	uint_16 firstCluster;
	uint_32 fileSize;				//size in bytes- number of bytes of the file or number of files in folder
}FILE;

/*attributes:
Bit 0 : Read Only
Bit 1 : Hidden
Bit 2 : System
Bit 3 : Volume Label
Bit 4 : This is a subdirectory
Bit 5 : Archive
Bit 6 : Device (Internal use)
Bit 6 : Unused
*/

#endif