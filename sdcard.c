#include<avr/io.h>
#include<util/delay.h>
#include "sdcard.h"
#include "spi.h"
static uint16_t block_length = 0;

//null args for some cmd's
void sd_send_null_args(void)
{
    spi_transfer(0x00);
    spi_transfer(0x00);
    spi_transfer(0x00);
    spi_transfer(0x00);
    spi_transfer(0x95);
}

//initialise the SD Card
void sd_card_init(void)
{
    uint8_t i;
    
    for(i = 0; i < 80; i++)
      spi_transfer(0xFF);
      
    _delay_ms(10);
    //send CMD0
    PORTB &= ~(1<<PB4);
    spi_transfer(0x40);
    sd_send_null_args();
    
    while(spi_transfer(0xFF) != 0x01);
    
    
    //send CMD1
    spi_transfer(0x41);
    sd_send_null_args();
    
    while(spi_transfer(0xFF) != 0x01);

    //check voltage range if required
    //set clock speed=clock/16=1 mHz
    SPCR &= ~(1<<SPR1);
    SPCR |= 1<<SPR0;
}


    
//set block length
void sd_set_block_length(uint16_t length)
{
    block_length = length;
    spi_transfer(0x50);
    spi_transfer(0x00);
    spi_transfer(0x00);
    spi_transfer(length >> 8);
    spi_transfer(length);
    spi_transfer(0x95);
    while(spi_transfer(0xFF) != 0x00);
    
}

//read block
void sd_card_read_block(uint32_t addr, uint8_t * data)
{
    uint16_t temp;
    
    spi_transfer(0x51);
    spi_transfer(addr >> 24);
    spi_transfer(addr >> 16);
    spi_transfer(addr >> 8);
    spi_transfer(addr);
    spi_transfer(0x95);
    
    while((spi_transfer(0xFF) != 0x00));
    
    while((spi_transfer(0xFF) != 0xFE));
    
    for(temp = 0; temp < block_length; temp++)
        *(data++) = spi_transfer(0xFF);
    
    spi_transfer(0xFF);
    spi_transfer(0xFF);
}
void sd_card_read_bytes(uint32_t addr, uint8_t * data, uint16_t size)
{
    if(block_length != size)
      sd_set_block_length(size);
    
    sd_card_read_block(addr, data);
}
