#include<avr/io.h>
#include "spi.h"

//initialise the SPI interface
void spi_init(void)
{
    //set mosi and sck and SS to output 
    DDRB = 0;
    DDRB = (1<<DDB7)|(1<<DDB5)|(1<<DDB4);
    PORTB |= (1<<PB4);
    //enable spi, set master 
    SPCR = (1<<SPE)| (1<<MSTR) | (1<<SPR1);
    //set speed between 100 to 400 kHz
}

//transfer byte thru SPI
uint8_t spi_transfer(uint8_t byte_to_send)
{
    SPDR = byte_to_send;
    while(!(SPSR & (1<<SPIF)));
    return SPDR;
}
