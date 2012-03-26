#ifndef FAT32_H
#define FAT32_H
#include "play.h"

#define SECTOR_SIZE 512
#define BOOT_SECTOR_SIZE 0x5A
#define DIR_SIZE 32

typedef struct struct_FAT32 {
   // Filesystem globals
  uint8_t SectorsPerCluster;
  uint8_t HalfSectorsPerCluster;
  uint32_t cluster_begin_lba;
  uint32_t RootDir_First_Cluster;//should be xero in our case
  uint32_t fat_begin_lba;
  uint32_t fat_begin_byte;
  
  uint32_t current_cluster;
  uint8_t sectors;
  uint32_t pos;
} FAT32;

extern volatile FAT32 fat;
//-----------------------------------------------------------------------------
// Prototypes
//-----------------------------------------------------------------------------
uint32_t lba_to_bytes(uint32_t lba);
uint8_t Get_SectorByte(uint16_t sublocation);
uint16_t Get_SectorWord(uint16_t sublocation);
uint32_t Get_SectorUI32(uint16_t sublocation);
uint32_t FAT32_LBAofCluster(uint32_t Cluster_Number);
void fat32_init(void);
uint16_t fat32_read(uint16_t size);
void fat32_get_next_cluster(void);
void fat32_read_half(void);

uint8_t list_wav_files(WAVE * wav);
void read_wave_info(WAVE * wav);
uint8_t check_wav_files(uint8_t * data);

#endif
