#include <host/libusb.h>
#include <stdlib.h>
#include "usb.h"

static libusb_context *g_ctx = NULL;
static struct usb_device **g_devs = NULL;
static int g_dev_cnt = 0;
static int g_init_ok = 0;

int usb_get_nr_devices(void)
{
	return g_dev_cnt;
}

struct usb_device *usb_get_device(int nr)
{
	if (nr < g_dev_cnt)
		return g_devs[nr];
	return NULL;
}

int usb_init(void)
{
	int res = -1;
	struct libusb_device **devs;

	if (g_init_ok)
		return 0;
	if (libusb_init(&g_ctx)) goto end;
	if (g_ctx)
		libusb_set_debug(g_ctx, 0);
	g_dev_cnt = libusb_get_device_list(g_ctx, &devs);
	if (g_dev_cnt <= 0) goto end;
	g_devs = malloc(sizeof (struct usb_device) * g_dev_cnt);
	if (!g_devs) goto end;
	res = 0;
	g_init_ok = 1;
end:
	libusb_free_device_list(devs, 0);
	return res;
}

void usb_exit(void)
{
	int i;

	if (!g_init_ok)
		return;
	if (g_devs) {
		for (i = 0; i < g_dev_cnt; i++) {
			if (g_devs[i]) {
				if (g_devs[i]->config)
					free(g_devs[i]->config);
				free(g_devs[i]);
			}
		}
		free(g_devs);
		g_devs = NULL;
	}
	if (g_ctx) {
		libusb_exit(g_ctx);
		g_ctx = NULL;
	}
}
