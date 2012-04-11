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

void ezio_sync_display(unsigned char *str2)
{
	unsigned char str1[] = "Soliton EZIO LCD";
	unsigned char def2[] = "****************";

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
	return 0;
}

void ezio_exit(void)
{
	uart_close(ezio_uart);
}
