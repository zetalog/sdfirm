#ifndef __NFC_USB_H_INCLUDE__
#define __NFC_USB_H_INCLUDE__

#include <host/libusb.h>

struct usb_device {
	struct libusb_device_descriptor descriptor;
	struct libusb_config_descriptor *config;
	int devid;
};

int usb_init(void);
void usb_exit(void);

int usb_get_nr_devices(void);
struct usb_device *usb_get_device(int nr);

libusb_device_handle *usb_open(struct usb_device *dev);
int usb_close(libusb_device_handle *udev);

#endif /* __NFC_USB_H_INCLUDE__ */
