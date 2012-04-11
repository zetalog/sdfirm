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
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	int last_keys = 0x00, keys;
	int res, i, len;
	unsigned char buf[EZIO_MAX_BUF];
	unsigned char msg[EZIO_MAX_BUF];

	if (argc < 3)
		return -1;

	if (ezio_init(argv[1], argv[2]) < 0) {
		fprintf(stderr, "Open port %d failure\n",
			ezio_port);
		return -1;
	}

	/* initialize EZIO */
	ezio_screen_clear();
	/* display default message */
	ezio_sync_display(NULL);

	len = 0;
	while (1) {
		ezio_keypad_scan();
		res = uart_read(ezio_uart, buf+len, EZIO_MAX_BUF-len);
		if (res < 0) {
			fprintf(stderr, "Read port %d failure\n",
				ezio_port);
			goto end;
		}
		if (res == 0)
			continue;

		len += res;
		while (len > 2) {
			if (buf[0] == EZIO_READ) {
				break;
			} else {
				len--;
				memcpy(buf, buf+1, len);
			}
		}

		if (len <= 2)
			continue;
		len -= 2;
		keys = (buf[1] & ~0xF0);

		msg[0] = 0;
		for (i = 0; i < EZIO_MAX_SCAN; i++) {
			char tmp[6];

			if (EZIO_KEY_VAL(keys, i) !=
			    EZIO_KEY_VAL(last_keys, i)) {
				if (EZIO_KEY_DOWN(keys, i)) {
					sprintf(tmp, "+%s", ezio_key_name(i));
				} else {
					sprintf(tmp, "-%s", ezio_key_name(i));
				}
				strcat(msg, tmp);
			}
		}

		ezio_sync_display(msg);
		last_keys = keys;
	}

end:
	ezio_exit();
	return res;
}
