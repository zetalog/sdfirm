#include <target/peci.h>

#define PECI_IDLE	0
#define PECI_REQ	1
#define PECI_DATA	2
#define PECI_RESP	3

#ifndef PECI_MAX_LEN
#define PECI_MAX_LEN					32
#endif

uint8_t peci_txbuf[PECI_MAX_LEN];
uint8_t peci_rxbuf[PECI_MAX_LEN];

uint8_t peci_state;
uint8_t peci_req_len;
uint8_t peci_rsp_len;
uint8_t peci_txlen = 0;
uint8_t peci_rxlen = 0;

void peci_submit_response(uint8_t cc, uint8_t wrlen,
			  uint8_t buf[])
{
	peci_state = PECI_RESP;
	peci_rsp_len = wrlen + 1;
	peci_txbuf[0] = cc;
	for (i = 0; i < wrlen; i++) {
		peci_txbuf[1+i] = buf[i];
	}
	spi_submit_write(peci_rsp_len);
}

void peci_submit_request(void)
{
	peci_state = PECI_REQ;
	spi_submit_read(3);
}

void peci_handle_request(void)
{
	switch (peci_rxbuf[0]) {
	case 0x30 ... 0x3F:
		peci_submit_response(0x40, 0);
		break;
	}
}

spi_device_t peci_spi = {
	peci_iocb,
};

void peci_iocb(uint8_t len)
{
	if (peci_state == PECI_REQ) {
		while (len) {
			if (peci_rxlen < 3) {
				peci_rxbuf[peci_rxlen++] = spi_read_byte();
				len--;
			} else {
				peci_state = PECI_DATA;
				peci_req_len = peci_rxbuf[1];
				spi_submit_read(peci_rxbuf[1]);
			}
		}
	}
	if (peci_state == PECI_DATA) {
		while (len) {
			if (peci_rxlen < peci_req_len) {
				peci_rxbuf[3+peci_rxlen++] = spi_read_byte();
				len--;
			} else {
				peci_handle_request();
			}
		}
	}
	if (peci_state == PECI_RESP) {
		while (len) {
			if (peci_txlen < peci_rsp_len)
				spi_write_byte(peci_txbuf[peci_txlen++]);
			else {
				peci_state = PECI_IDLE;
				peci_submit_request();
			}
		}
	}
}

void peci_init(void)
{
	spi_register_device();
	spi_submit_request();
}
