#include<avr/io.h>
#include<avr/interrupt.h>
#include<stdlib.h>
#include "spi.h"
#include "play.h"
#include "sdcard.h"
#include "fat32.h"

volatile BUFFER buffer;
volatile FAT32 fat;
volatile uint8_t playlist, max, state;

int main(void)
{
  
  DDRC = 0xFF;
  PORTC = 0xFF;
  
  //Enable Interrupts
  sei();
  
  spi_init();
  sd_card_init();
  config_buttons();
  
  PORTC = 0x00;
  
  fat32_init();
  
  WAVE wav[5];
  snd_output_init();
  
  max = list_wav_files(wav);
  
  playlist = 0;
  
  read_wave_info(&wav[playlist]);
  
  start_music(&wav[playlist]);
  
  while(1)
  {
    if(buffer.play_pos >= BUFFER_SIZE)
    {
      buffer.play_pos = 0;
      fat32_read_half();
    }
    else if(buffer.play_pos == BUFFER_HALF)
      fat32_read_half();
    
    if(state == NEXT)
    {
      playlist++;
      if(playlist == max)
        playlist = 0;
  
      read_wave_info(&(wav[playlist]));
      start_music(&(wav[playlist]));
      state = PLAYING;
    }
  }
  return 0;
}



