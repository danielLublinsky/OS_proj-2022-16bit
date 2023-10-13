//#include<stdint.h>
#include "../includes/ports.h"
#include "../includes/ata_disk.h"

/*
BSY: a 1 means that the controller is busy executing a command. No register should be accessed (except the digital output register) while this bit is set.
RDY: a 1 means that the controller is ready to accept a command, and the drive is spinning at correct speed..
WFT: a 1 means that the controller detected a write fault.
SKC: a 1 means that the read/write head is in position (seek completed).
DRQ: a 1 means that the controller is expecting data (for a write) or is sending data (for a read). Don't access the data register while this bit is 0.
COR: a 1 indicates that the controller had to correct data, by using the ECC bytes (error correction code: extra bytes at the end of the sector that allows to verify its integrity and, sometimes, to correct errors).
IDX: a 1 indicates the the controller retected the index mark (which is not a hole on hard-drives).
ERR: a 1 indicates that an error occured. An error code has been placed in the error register.
*/

#define STATUS_BSY 0x80
#define STATUS_RDY 0x40
#define STATUS_DRQ 0x08
#define STATUS_DF 0x20
#define STATUS_ERR 0x01

//This is really specific to out OS now, assuming ATA bus 0 master 
//Source - OsDev wiki
static void ATA_wait_BSY();
static void ATA_wait_DRQ();
void read_sectors_ATA_PIO(char* target_address, int LBA, char sector_count)
{

	ATA_wait_BSY();
	port_byte_out(0x1F6, 0xE0 | ((LBA >> 24) & 0xF));
	port_byte_out(0x1F2, sector_count);
	port_byte_out(0x1F3, (char)LBA);
	port_byte_out(0x1F4, (char)(LBA >> 8));
	port_byte_out(0x1F5, (char)(LBA >> 16));
	port_byte_out(0x1F7, 0x20); //Send the read command

	short* target = (short*)target_address;


	for (int j = 0; j < sector_count; j++)
	{
		ATA_wait_BSY();
		ATA_wait_DRQ();
		for (int i = 0; i < 256; i++) {
			//*target_address = port_word_in(0x1F0);
			//target_address++;
			target[i] = port_word_in(0x1F0);
		}
		//target[i] = port_word_in(0x1F0);
		target += 256;
	}
}

void write_sectors_ATA_PIO(int LBA, char sector_count, int* bytes)
{
	ATA_wait_BSY();
	port_byte_out(0x1F6, 0xE0 | ((LBA >> 24) & 0xF));
	port_byte_out(0x1F2, sector_count);
	port_byte_out(0x1F3, (char)LBA);
	port_byte_out(0x1F4, (char)(LBA >> 8));
	port_byte_out(0x1F5, (char)(LBA >> 16));
	port_byte_out(0x1F7, 0x30); //Send the write command

	for (int j = 0; j < sector_count; j++)
	{
		ATA_wait_BSY();
		ATA_wait_DRQ();
		for (int i = 0; i < 256; i++)
		{
			port_long_out(0x1F0, bytes[i]);
		}
	}
}

void write_memory(int LBA, char sector_count, char* bytes) {

	char bwrite1[512 * sector_count];
	char bwrite2[strlen(bytes)];

	for (int i = 0; i < 512 * sector_count; i++)
	{
		bwrite1[i] = 0x0;
	}
	write_sectors_ATA_PIO(LBA, sector_count, bwrite1);

	int i = 0;
	while (*bytes != '\0') {
		bwrite2[i++] = *bytes;
		bytes++;
	}
	write_sectors_ATA_PIO(LBA, sector_count, bwrite2);
}

static void ATA_wait_BSY()   //Wait for bsy to be 0
{
	while (port_byte_in(0x1F7) & STATUS_BSY);
}
static void ATA_wait_DRQ()  //Wait fot drq to be 1
{
	while (!(port_byte_in(0x1F7) & STATUS_RDY));
}
