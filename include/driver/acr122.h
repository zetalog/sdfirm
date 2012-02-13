#ifndef __ACR122_H_INCLUDE__
#define __ACR122_H_INCLUDE__

#include <driver/pn53x.h>

struct acr122_pseudo_apdu {
	uint8_t ins;
	uint8_t p1;
	scs_size_t nc;
	scs_size_t ne;
	uint8_t sw1;
	uint8_t sw2;
#define ACR122_SW2_TIMEOUT	0x01
#define ACR122_SW2_PARITY	0x27
#define ACR122_SW2_SANITY	0x7F
};
#define ACR122_HEAD_SIZE	5

scs_size_t acr122_xchg_avail(void);
scs_err_t acr122_xchg_block(scs_size_t nc, scs_size_t ne);
uint8_t acr122_read_byte(scs_off_t index);
void acr122_write_byte(scs_off_t index, uint8_t value);
void acr122_register_completion(scs_cmpl_cb completion);
scs_err_t acr122_get_error(void);
void acr122_init(void);

#endif /* __ACR122_H_INCLUDE__ */
