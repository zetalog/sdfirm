#include <host/ezio.h>
#include <stdio.h>
#include <string.h>
#include <process.h>

void ezio_scan_thread(void *arg)
{
	int last_keys = 0x00, keys;
	int res, i, len;
	unsigned char buf[EZIO_MAX_BUF];
	unsigned char msg[EZIO_MAX_BUF];

	len = 0;
again:
	res = uart_read(ezio_uart, buf+len, EZIO_MAX_BUF-len);
	if (res < 0) {
		fprintf(stderr, "Read port %d failure\n",
			ezio_port);
		goto end;
	}
	if (res == 0) {
		goto again;
	}

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
		goto again;
	len -= 2;

	if (ezio_state == EZIO_STATE_SCAN) {
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
		ezio_async_display(msg);
		last_keys = keys;
	}

end:
	_endthread();
}

int ezio_thread_init(const char *port, const char *baudrate)
{
	if (ezio_init(port, baudrate) < 0) {
		return -1;
	}
	_beginthread(ezio_scan_thread, 1024, NULL);
	ezio_async_display("");
	return 0;
}

int main(int argc, char **argv)
{

	if (argc < 3)
		return -1;

	if (ezio_thread_init(argv[1], argv[2]) < 0) {
		fprintf(stderr, "Open port %d failure\n",
			ezio_port);
		return -1;
	}

	/* display default message */

	while (1) {
		if (ezio_state == EZIO_STATE_DUMP) {
			ezio_sync_display(ezio_msg);
		}
		if (ezio_state == EZIO_STATE_SCAN) {
			ezio_keypad_scan();
		}
	}

	ezio_exit();
	return 0;
}
