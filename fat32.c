#include<avr/io.h>
#include<string.h>
#include "sdcard.h"
#include "fat32.h"
#include "play.h"

uint32_t lba_to_bytes(uint32_t lba)
{
  return SECTOR_SIZE * lba;
}

uint8_t Get_SectorByte(uint16_t sublocation)
{
  return buffer.data[sublocation];
}
/* Function to retriev a word from currentsector array*/
uint16_t Get_SectorWord(uint16_t sublocation) // Return Word at position specified
{
  uint16_t data; 
  
  memcpy(&data, (uint8_t *)buffer.data + sublocation, sizeof(uint16_t));

  return (data);   // Return value
}
/* Function to retriev a ui32 from currentsector array*/
uint32_t Get_SectorUI32(uint16_t sublocation) // Return UI32 at position specified
{
  uint32_t data; 
  
  memcpy(&data, (uint8_t *)buffer.data + sublocation, sizeof(uint32_t));

  return (data);   // Return value
}

void fat32_init(void)
{
  uint8_t Number_of_FATS;
  uint32_t Sectors_per_FAT;
  uint16_t Reserved_Sectors;

  // Find LBA address of FAT partition
  sd_card_read_bytes(454, (uint8_t *)&(fat.fat_begin_lba), sizeof(uint32_t));
  
  sd_card_read_bytes(lba_to_bytes(fat.fat_begin_lba), (uint8_t *)buffer.data, BOOT_SECTOR_SIZE);
  
   // Load Parameters of FAT32 
  fat.SectorsPerCluster = Get_SectorByte(0x0D);
  fat.HalfSectorsPerCluster = fat.SectorsPerCluster * 2;
  Reserved_Sectors = Get_SectorWord(0x0E);
  Number_of_FATS = Get_SectorByte(0x10);
  Sectors_per_FAT = Get_SectorUI32(0x24);
  fat.RootDir_First_Cluster = Get_SectorUI32(0x2C);

  fat.fat_begin_lba += Reserved_Sectors;// First FAT LBA address
  fat.fat_begin_byte = lba_to_bytes(fat.fat_begin_lba);
   
  // The address of the first data cluster on this volume
  fat.cluster_begin_lba = fat.fat_begin_lba + (Number_of_FATS * Sectors_per_FAT);
  
}

uint32_t FAT32_LBAofCluster(uint32_t Cluster_Number)
{
  return ((fat.cluster_begin_lba + ((Cluster_Number-2)*fat.SectorsPerCluster)));
}

uint8_t check_wav_file(uint8_t * data)
{
  uint8_t attr;
  
  attr = data[0x0b];
  if((attr == 0x0F) || ((attr & 0x10) != 0x00) || ((attr & 0x08) != 0x00))
    return 0;
    
  if(data[0x00] == 0xE5)
    return 0;
  //Check if extension is wav
  if(strncmp((char *)data + 0x08, "WAV", 3) == 0)
    return 1;
  return 0;
}
void fat32_get_next_cluster(void)
{
  fat.sectors = 0;
  sd_card_read_bytes(fat.fat_begin_byte + 4 * fat.current_cluster,(uint8_t *)&(fat.current_cluster), 4);
  if(fat.current_cluster == 0x0FFFFFFF)
    state = NEXT;
}
uint16_t fat32_read(uint16_t size)
{
  if(fat.pos % SECTOR_SIZE == 0)
  {
    sd_card_read_bytes(lba_to_bytes(FAT32_LBAofCluster(fat.current_cluster) + fat.sectors), (uint8_t *)buffer.data, BUFFER_HALF);
    fat.sectors++;
    fat.pos = 0;
  }
  if(fat.sectors == fat.SectorsPerCluster)
    fat32_get_next_cluster();
    
  fat.pos += size;
  return fat.pos - size;
}
void fat32_read_half()
{
  if(buffer.play_pos == 0)
  {
    sd_card_read_bytes(lba_to_bytes(FAT32_LBAofCluster(fat.current_cluster) + fat.sectors) + BUFFER_HALF, (uint8_t *)(buffer.data + BUFFER_HALF), BUFFER_HALF);
    fat.sectors++;
  }
  else
    sd_card_read_bytes(lba_to_bytes(FAT32_LBAofCluster(fat.current_cluster) + fat.sectors), (uint8_t *)buffer.data, BUFFER_HALF);
  
  if(fat.sectors == fat.SectorsPerCluster)
    fat32_get_next_cluster();
}
uint8_t list_wav_files(WAVE * wav)
{
  uint8_t count = 0;
  uint16_t pos;
  
  fat.current_cluster = fat.RootDir_First_Cluster;
  fat.sectors = 0;
  fat.pos = 0;
  
  pos = fat32_read(DIR_SIZE);
  
  while(buffer.data[pos] != 0x00)
  {
    if(check_wav_file((uint8_t *)buffer.data + pos))
    {
      memcpy(wav[count].filename,(uint8_t *) buffer.data + pos, 8);
      *((uint8_t *)&(wav[count].cluster)) = buffer.data[pos + 0x1A];
      *((uint8_t *)&(wav[count].cluster) + 1) = buffer.data[pos + 0x1B];
      *((uint8_t *)&(wav[count].cluster) + 2) = buffer.data[pos + 0x14];
      *((uint8_t *)&(wav[count].cluster) + 3) = buffer.data[pos + 0x15];
      count++;
    }
    pos = fat32_read(DIR_SIZE);
  }
  return count;
}

void read_wave_info(WAVE * wav)
{
  char word[4];
  uint16_t pos = 0;
  
  sd_card_read_bytes(lba_to_bytes(FAT32_LBAofCluster(wav -> cluster)), (uint8_t *)buffer.data, 512);
  do
  {
    memcpy(word, (uint8_t *)buffer.data + pos, 4);
    pos++;
  }while(strncmp("fmt ", word, 4) != 0);
  
  memcpy(&(wav -> sampling_freq), (uint8_t *)buffer.data + pos + 11, 4);
  
}

