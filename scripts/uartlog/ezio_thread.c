#include <host/ezio.h>
#include <stdio.h>
#include <string.h>
#include <process.h>

void ezio_scan_thread(void *arg)
{
	int res;
again:
	res = ezio_async_read();
	if (res < 0) {
		fprintf(stderr, "Read port %d failure\n",
			ezio_port);
		goto end;
	}
	if (res == 0) {
		goto again;
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
			ezio_sync_display();
		}
		if (ezio_state == EZIO_STATE_SCAN) {
			ezio_keypad_scan();
		}
	}

	ezio_exit();
	return 0;
}
