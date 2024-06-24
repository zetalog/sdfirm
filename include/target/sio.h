#ifndef __SIO_H_INCLUDE__
#define __SIO_H_INCLUDE__

#include <target/lpc.h>

#define SIO_INDEX_0	0x2E
#define SIO_DATA_0	0x2F
#define SIO_INDEX_1	0x4E
#define SIO_DATA_1	0x4F

typedef uint8_t sio_dev_t;

#include <driver/sio.h>

void sio_enter(void);
void sio_exit_mode(void);
void sio_select(sio_dev_t dev);
void sio_write8(uint8_t v, uint8_t a);
uint8_t sio_read8(uint8_t a);

#endif /* __SIO_H_INCLUDE__ */
