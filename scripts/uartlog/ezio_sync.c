#include <host/ezio.h>
#include <stdio.h>
#include <string.h>

int ezio_sync_init(const char *port, const char *baudrate)
{
	if (ezio_init(port, baudrate) < 0) {
		return -1;
	}
	ezio_sync_display();
	return 0;
}

int main(int argc, char **argv)
{
	int res;

	if (argc < 3)
		return -1;

	if (ezio_sync_init(argv[1], argv[2]) < 0) {
		fprintf(stderr, "Open port %d failure\n",
			ezio_port);
		return -1;
	}

	while (1) {
		ezio_keypad_scan();
		res = ezio_async_read();
		if (res < 0) {
			fprintf(stderr, "Read port %d failure\n",
				ezio_port);
			goto end;
		}
		if (res == 0)
			continue;
		ezio_sync_display();
	}

end:
	ezio_exit();
	return res;
}
