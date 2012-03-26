#ifndef PLAY_H
#define PLAY_H
#define BUFFER_SIZE 512
#define BUFFER_HALF 256

#define PAUSED 0
#define PLAYING 1
#define NEXT 2

typedef struct struct_WAVE{
  uint32_t cluster;
  uint8_t filename[8];
  uint32_t sampling_freq;
}WAVE;


typedef struct struct_BUFFER{
  uint8_t data[BUFFER_SIZE];
  uint16_t play_pos;
}BUFFER;

void snd_output_init(void);
void config_buttons(void);
void start_music(WAVE * wav);

extern volatile BUFFER buffer;
extern volatile uint8_t playlist, max, state;

#endif
