#ifndef __SPD_H_INCLUDE__
#define __SPD_H_INCLUDE__
#include <target/generic.h>
#include <target/i2c.h>


int spd_hw_init(void);
int spd_hw_read_bytes(uint8_t dev, uint16_t addr, uint8_t *buffer, int len);
int spd_hw_write_bytes(uint8_t dev, uint16_t addr, uint8_t *buffer, int len);

#endif
