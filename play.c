#include<avr/io.h>
#include<avr/interrupt.h>
#include "play.h"
#include "fat32.h"

//initialise audio output hardware
void snd_output_init(void)
{
    //config for audio hardware, Timer 1 PWM
    //set output pins   
    DDRD |= (_BV(DDD5)) | (_BV(DDD4)); 
    
    TCCR1A = 0x00;
    TCCR1B = 0x00;
    TCCR1B =  _BV(WGM12) | _BV(CS10);
    TCCR1A = (_BV(COM1A1)) | (_BV(COM1B1)) | _BV(WGM10);
    
    ICR1 = 255;
    
    OCR1A = 100;
    OCR1B = 200;
    
    //config for Sampling frequency Timer 0
    TCCR0 = 0x00;
    //CTC mode
    //prescaler = 8
    TCCR0 = _BV(WGM01) | _BV(CS01);
    
}

void start_music(WAVE * wav)
{
  
  buffer.play_pos = BUFFER_SIZE;
  
  fat.current_cluster = wav -> cluster;
  fat.sectors = 0;
  fat.pos = 0;
  fat32_read_half();
  
  OCR0 = (2000000/(wav -> sampling_freq)) - 1;
  //enable Interrupt
  TIMSK |= _BV(OCIE0);
  TCNT0 = 0;
  
  state = PLAYING;
}

void config_buttons(void)
{
  MCUCR |= _BV(ISC11) | _BV(ISC10) | _BV(ISC01) | _BV(ISC00);
  GICR |= _BV(INT0) | _BV(INT1);
  DDRD &= ~(_BV(DDD2) | _BV(DDD3));
}

//sampling freq timer interrupt
ISR(TIMER0_COMP_vect)
{
  OCR1A = buffer.data[buffer.play_pos];
  OCR1B = buffer.data[(buffer.play_pos) + 1];
  buffer.play_pos += 2;
}

//play/pause interrupt
ISR(INT0_vect)
{
  if(state == PLAYING)
  {
    state = PAUSED;
    TIMSK &= ~(_BV(OCIE0));
  }
  else if(state == PAUSED)
  {
    state = PLAYING;
    TIMSK |= _BV(OCIE0);
  }
}

//next song interrupt
ISR(INT1_vect)
{
  state = NEXT;
}
