#include <host/libusb.h>
#include <stdlib.h>
#include "usb.h"

#define USB_WAIT_TIMEOUT		60*1000

static libusb_context *g_ctx = NULL;
static int g_usb_err = LIBUSB_SUCCESS;
static struct usb_device *g_devs = NULL;
static int g_nr_devs = 0;
static struct libusb_device **g_udevs = NULL;
static int g_nr_udevs = 0;

int usb_init(void);
void usb_exit(void);

void usb_free_devices(void)
{
	int i;

	if (g_devs) {
		for (i = 0; i < g_nr_devs; i++) {
			if (g_devs[i].config)
				free(g_devs[i].config);
		}
		free(g_devs);
		g_devs = NULL;
	}
	if (g_udevs) {
		libusb_free_device_list(g_udevs, 0);
		g_udevs = NULL;
	}
	g_nr_devs = 0;
}

int usb_find_devices(void)
{
	int err;
	int devid;
	uint8_t cnfid = 1;
	libusb_device_handle *udev;

	if (g_udevs)
		return LIBUSB_SUCCESS;
	if (!g_ctx) {
		g_usb_err = usb_init();
		if (g_usb_err != LIBUSB_SUCCESS)
			return LIBUSB_ERROR_NO_DEVICE;
	}

	g_nr_udevs = libusb_get_device_list(g_ctx, &g_udevs);
	if (g_nr_udevs <= 0)
		return LIBUSB_SUCCESS;
	g_devs = malloc(sizeof (struct usb_device) * g_nr_udevs);
	if (!g_devs)
		return LIBUSB_ERROR_NO_MEM;

	for (devid = 0; devid < g_nr_udevs; devid++) {
		g_devs[g_nr_devs].dev = g_udevs[devid];
		libusb_get_device_descriptor(g_udevs[devid],
					     &(g_devs[g_nr_devs].descriptor));
		if (libusb_open(g_devs[g_nr_devs].dev, &udev)) {
			continue;
		}
		err = libusb_set_configuration(udev, cnfid);
		if (err) {
			libusb_close(udev);
			continue;
		}
		libusb_get_config_descriptor(g_devs[g_nr_devs].dev,
					     (uint8_t)(cnfid-1),
					     &g_devs[g_nr_devs].config);
		libusb_close(udev);
		g_nr_devs++;
	}
	return LIBUSB_SUCCESS;;
}

int usb_get_nr_devices(void)
{
	return g_nr_devs;
}

struct usb_device *usb_get_device(int nr)
{
	if (nr < g_nr_devs)
		return &g_devs[nr];
	return NULL;
}

libusb_device_handle *usb_open(struct usb_device *dev)
{
	libusb_device_handle *udev = NULL;
	g_usb_err = libusb_open(dev->dev, &udev);
	return udev;
}

int usb_close(libusb_device_handle *udev)
{
	if (udev)
		libusb_close(udev);
	return 0;
}

char *usb_strerror(void)
{
	switch (g_usb_err) {
	case LIBUSB_SUCCESS:
		return "Success";
	case LIBUSB_ERROR_IO:
		return "Input/output error";
	case LIBUSB_ERROR_INVALID_PARAM:
		return "Invalid parameter";
	case LIBUSB_ERROR_ACCESS:
		return "Access denied";
	case LIBUSB_ERROR_NO_DEVICE:
		return "No such device";
	case LIBUSB_ERROR_NOT_FOUND:
		return "Not found";
	case LIBUSB_ERROR_BUSY:
		return "Resouce busy";
	case LIBUSB_ERROR_TIMEOUT:
		return "Openration timed out";
	case LIBUSB_ERROR_OVERFLOW:
		return "Overflow";
	case LIBUSB_ERROR_PIPE:
		return "Pipe error";
	case LIBUSB_ERROR_INTERRUPTED:
		return "System call interrupted";
	case LIBUSB_ERROR_NO_MEM:
		return "Insufficient memory";
	case LIBUSB_ERROR_NOT_SUPPORTED:
		return "Not supported";
	default:
		return "Other error";
	}
}

int usb_bulk_transfer(libusb_device_handle *udev,
		      unsigned char endpoint,
		      uint8_t *data, int length,
		      int *transferred, int timeout)
{
	return libusb_bulk_transfer(udev, endpoint,
				    data, length,
				    transferred, timeout);
}

int usb_get_device_descriptor(libusb_device_handle *udev,
			      struct libusb_device_descriptor *desc)
{
	libusb_device *dev = libusb_get_device(udev);
	if (!dev)
		return -1;
	return libusb_get_device_descriptor(dev, desc);
}

int usb_bulk_read(libusb_device_handle *dev, int ep,
		  char *bytes, int size, int timeout)
{
	int transfered;
	g_usb_err = usb_bulk_transfer(dev, (uint8_t)ep,
				      bytes, size,
				      &transfered, timeout);
	return transfered;
}

int usb_bulk_write(libusb_device_handle *dev, int ep,
		   char *bytes, int size, int timeout)
{
	int transfered;
	g_usb_err = usb_bulk_transfer(dev, (uint8_t)ep,
				      bytes, size,
				      &transfered, timeout);
	return transfered;
}

int usb_get_string(libusb_device_handle *dev,
		   uint8_t desc_index, uint16_t langid,
		   unsigned char *data, uint16_t length,
		   int timeout)
{
	return libusb_get_string_descriptor(dev, desc_index,
					    langid, data, length,
					    timeout);
}

int usb_get_string_simple(libusb_device_handle *dev, int index,
			  char *buf, size_t buflen)
{
	char tbuf[255];	/* Some devices choke on size > 255 */
	uint16_t langid;
	int si, di;
	int ret;
	
	ret = usb_get_string(dev, 0, 0, tbuf, sizeof(tbuf), USB_WAIT_TIMEOUT);
	if (ret < 0)
		return g_usb_err;
	if (ret < 4) {
		g_usb_err = LIBUSB_ERROR_IO;
		return g_usb_err;
	}
	langid = tbuf[2] | (tbuf[3] << 8);
	ret = usb_get_string(dev, (uint8_t)index, langid,
			     tbuf, sizeof(tbuf), USB_WAIT_TIMEOUT);
	if (ret < 0)
		return ret;
	if (tbuf[1] != LIBUSB_DT_STRING) {
		g_usb_err = LIBUSB_ERROR_IO;
		return g_usb_err;
	}
	if (tbuf[0] > ret) {
		g_usb_err = LIBUSB_ERROR_OVERFLOW;
		return g_usb_err;
	}
	for (di = 0, si = 2; si < tbuf[0]; si += 2) {
		if (di >= ((int)buflen - 1))
			break;
		if (tbuf[si + 1])	/* high byte */
			buf[di++] = '?';
		else
			buf[di++] = tbuf[si];
	}
	buf[di] = 0;
	return di;
}

int usb_init(void)
{
	int res;
	if (g_ctx)
		return LIBUSB_SUCCESS;
	res = libusb_init(&g_ctx);
	if (res)
		return res;
	libusb_set_debug(g_ctx, 0);
	return LIBUSB_SUCCESS;
}

void usb_exit(void)
{
	usb_free_devices();
	if (g_ctx) {
		libusb_exit(g_ctx);
		g_ctx = NULL;
	}
}
