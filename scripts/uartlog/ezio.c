/* Direct access to EZIO LCD, the program will display messages according
 * to the control button.
 * 1: display welcome message
 * 2: display UP message if "UP" button is pressed
 * 3: display ENTER message if "ENTER" button is pressed
 * 4: display ESC message if "ESC" button is pressed
 * 5: display DOWN message if "DOWN" button is pressed
 *
 * Copyright (c) Soliton Shanghai. All Rights Reserved.
 */

#include <host/ezio.h>
#include <stdlib.h>
#include <string.h>

uart_t ezio_uart;
int ezio_port;
unsigned char ezio_msg[EZIO_MAX_BUF];
int ezio_state;
unsigned char ezio_rxbuf[EZIO_MAX_BUF];
int ezio_rxlen;
int ezio_keys = 0x00;

const char *ezio_key_name(int i)
{
	switch (i) {
	case 0:
		return "Esc";
	case 1:
		return "U";
	case 2:
		return "Ent";
	case 3:
		return "D";
	}
	return "*";
}

void ezio_sync_cmd(int cmd)
{
	int ezi = EZIO_WRITE;

	uart_write(ezio_uart, &ezi, 1);
	uart_write(ezio_uart, &cmd, 1);
}

void __ezio_sync_display(unsigned char *msg)
{
	unsigned char nul[] = "                ";
	int a, b, i;

	a = strlen(msg);
	b = EZIO_MAX_LINE - a;
	uart_write(ezio_uart, msg, a);
	uart_write(ezio_uart, nul, b);

	fwrite(msg, 1, a, stdout);
	fwrite(nul, 1, b, stdout);
	fprintf(stdout, "\r\n");
	for (i = 0; i < a; i++)
		fprintf(stdout, "%02x ", msg[i]);
	for (i = 0; i < b; i++)
		fprintf(stdout, "%02x ", nul[i]);
	fprintf(stdout, "\r\n");
}

void ezio_sync_display(void)
{
	unsigned char str1[] = "Soliton EZIO LCD";
	unsigned char def2[] = "****************";
	unsigned char *str2 = ezio_msg;

	if (str2 == NULL || str2[0] == 0)
		str2 = def2;

	ezio_screen_clear();
	ezio_data_start();
	__ezio_sync_display(str1);
	__ezio_sync_display(str2);
	ezio_data_stop();
	ezio_state = EZIO_STATE_SCAN;
}

void ezio_async_display(unsigned char *str2)
{
	strcpy(ezio_msg, str2);
	ezio_state = EZIO_STATE_DUMP;
}

int ezio_init(const char *port, const char *baudrate)
{
	ezio_port = atoi(port);
	ezio_uart = uart_open(ezio_port);
	if (ezio_uart < 0) return -1;
	uart_config(ezio_uart, atoi(baudrate));
	ezio_rxlen = 0;
	ezio_keys = 0x00;
	ezio_async_display("");
	return 0;
}

int ezio_async_read(void)
{
	int keys;
	int res, i;
	unsigned char msg[EZIO_MAX_BUF];

	res = uart_read(ezio_uart, ezio_rxbuf+ezio_rxlen, EZIO_MAX_BUF-ezio_rxlen);
	if (res <= 0) {
		return res;
	}

	ezio_rxlen += res;
	while (ezio_rxlen > 2) {
		if (ezio_rxbuf[0] == EZIO_READ) {
			break;
		} else {
			ezio_rxlen--;
			memcpy(ezio_rxbuf, ezio_rxbuf+1, ezio_rxlen);
		}
	}
	if (ezio_rxlen <= 2)
		return 0;
	ezio_rxlen -= 2;

	if (ezio_state == EZIO_STATE_SCAN) {
		keys = (ezio_rxbuf[1] & ~0xF0);
		msg[0] = 0;
		for (i = 0; i < EZIO_MAX_SCAN; i++) {
			char tmp[6];

			if (EZIO_KEY_VAL(keys, i) !=
			    EZIO_KEY_VAL(ezio_keys, i)) {
				if (EZIO_KEY_DOWN(keys, i)) {
					sprintf(tmp, "+%s", ezio_key_name(i));
				} else {
					sprintf(tmp, "-%s", ezio_key_name(i));
				}
				strcat(msg, tmp);
			}
		}
		ezio_async_display(msg);
		ezio_keys = keys;
	}

	return 1;
}

void ezio_exit(void)
{
	uart_close(ezio_uart);
}
