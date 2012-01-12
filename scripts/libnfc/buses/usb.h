#ifndef __NFC_USB_H_INCLUDE__
#define __NFC_USB_H_INCLUDE__

#include <host/libusb.h>

struct usb_device {
	struct libusb_device *dev;
	struct libusb_device_descriptor descriptor;
	struct libusb_config_descriptor *config;
	libusb_device_handle *dev_handle;
};

int usb_init(void);
void usb_exit(void);

char *usb_strerror(void);

int usb_find_devices(void);
int usb_get_nr_devices(void);
struct usb_device *usb_get_device(int nr);

libusb_device_handle *usb_open(struct usb_device *dev);
int usb_close(libusb_device_handle *udev);
int usb_bulk_read(libusb_device_handle *dev, int ep,
		  char *bytes, int size, int timeout);
int usb_bulk_write(libusb_device_handle *dev, int ep,
		   char *bytes, int size, int timeout);
int usb_get_string_simple(libusb_device_handle *dev, int index,
			  char *buf, size_t buflen);

#endif /* __NFC_USB_H_INCLUDE__ */
