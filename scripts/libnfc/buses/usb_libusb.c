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

#if 0
static int _usb_transfer_sync(usb_dev_handle *dev, int control_code,
                              int ep, int pktsize, char *bytes, int size,
                              int timeout)
{
    void *context = NULL;
    int transmitted = 0;
    int ret;
    int requested;

	if (!timeout) timeout=INFINITE;
    ret = _usb_setup_async(dev, &context, control_code, (unsigned char )ep,
                           pktsize);

    if (ret < 0)
    {
        return ret;
    }

    do
    {
#ifdef LIBUSB_WIN32_DLL_LARGE_TRANSFER_SUPPORT
        requested = size > LIBUSB_MAX_READ_WRITE ? LIBUSB_MAX_READ_WRITE : size;
#else
        requested = size;
#endif
        ret = usb_submit_async(context, bytes, requested);

        if (ret < 0)
        {
            transmitted = ret;
            break;
        }

        ret = usb_reap_async(context, timeout);

        if (ret < 0)
        {
            transmitted = ret;
            break;
        }

        transmitted += ret;
        bytes += ret;
        size -= ret;
    }
    while (size > 0 && ret == requested);

    usb_free_async(&context);

    return transmitted;
}

int usb_bulk_write(libusb_dev_handle *dev, int ep, char *bytes, int size,
                   int timeout)
{
	return _usb_transfer_sync(dev, LIBUSB_IOCTL_INTERRUPT_OR_BULK_WRITE,
				  ep, 0, bytes, size, timeout);
}

int usb_bulk_read(libusb_dev_handle *dev, int ep, char *bytes, int size,
                  int timeout)
{
	return _usb_transfer_sync(dev, LIBUSB_IOCTL_INTERRUPT_OR_BULK_READ,
				  ep, 0, bytes, size, timeout);
}
#endif

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
