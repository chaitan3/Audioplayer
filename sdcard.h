#ifndef SDCARD_H
#define SDCARD_H

void sd_card_init(void);
void sd_send_null_args(void);
void sd_set_block_length(uint16_t length);
void sd_card_read_block(uint32_t addr, uint8_t * data);
void sd_card_read_bytes(uint32_t addr, uint8_t * data, uint16_t size);

#endif
