#ifndef ATA_H
#define ATA_H

void read_sectors_ATA_PIO(char* target_address, int LBA, char sector_count);
void write_sectors_ATA_PIO(int LBA, char sector_count, int* bytes);
void write_memory(int LBA, char sector_count, char* bytes);
//void read_memory(int LBA, char sector_count, int* bytes);
//void clear_memory(int LBA, char sector_count);

#endif