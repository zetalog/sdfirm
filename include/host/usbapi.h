#ifndef __USBAPI_H_INCLUDE__
#define __USBAPI_H_INCLUDE__

/* This file is a wrapper for libusb, please do not confuse it with
 * winusb.h which is a wrapper for windows USB facilities.
 */

#include <host/missing.h>
#include <stdint.h>
#include <host/libusb.h>

#define USB_STRING_LANGID		0x00
#define USB_STRING_FACTORY		0x01
#define USB_STRING_PRODUCT		0x02
#define USB_STRING_SERIALNO		0x03

struct usb_driver_ctx {
	uint16_t idProduct;
	uint16_t idVendor;
	uint16_t bcdDevice;
	uint8_t bConfigurationValue;
	uint8_t bInterfaceValue;
	uint8_t bAlternateSetting;
	int nBusIdentifier;
	int nDevIdentifier;
};

static inline void i2w(uint16_t value, unsigned char buffer[])
{
	buffer[0] = (uint8_t)(value & 0xFF);
	buffer[1] = (uint8_t)((value >> 8) & 0xFF);
}

static inline uint16_t w2i(const uint8_t *dw)
{
	return (dw[0]) | (dw[1] << 8); 
}

static inline void i2dw(uint32_t value, unsigned char buffer[])
{
	buffer[0] = (uint8_t)(value & 0xFF);
	buffer[1] = (uint8_t)((value >> 8) & 0xFF);
	buffer[2] = (uint8_t)((value >> 16) & 0xFF);
	buffer[3] = (uint8_t)((value >> 24) & 0xFF);
}

static inline uint32_t dw2i(const uint8_t *dw)
{
	return (dw[0]) | (dw[1] << 8) | (dw[2] << 16) | (dw[3] << 24); 
}

int usb_bulk_transfer(unsigned char endpoint,
		      uint8_t *data, int length,
		      int *transferred, int timeout);
int usb_control_transfer(uint8_t bmRequestType,
			 uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
			 uint8_t *data, uint16_t length, int timeout);
int usb_sync_interrupt_transfer(uint8_t endpoint, uint8_t *data, int length,
				int *actual_length, int timeout);

typedef void (*usb_intr_cb)(void *user_data);
typedef boolean (*usb_parse_cb)(uint8_t bConfigurationValue,
				const struct libusb_interface_descriptor *dt_inf);
boolean usb_probe_class_driver(uint8_t clazz, usb_parse_cb parser);
void usb_dump_ep_descriptor(struct libusb_endpoint_descriptor *ep);
int usb_get_context(struct usb_driver_ctx *ctx);
int usb_get_descriptor(uint8_t desc_type, uint8_t desc_index,
		       unsigned char *data, uint16_t length,
		       int timeout);
int usb_get_string(uint8_t desc_index, uint16_t langid,
		   unsigned char *data, uint16_t length,
		   int timeout);
int usb_get_device(unsigned char *data, uint16_t length, int timeout);
int usb_get_device_descriptor(struct libusb_device_descriptor *desc);
int usb_get_config_descriptor(uint8_t bConfigurationValue,
			      struct libusb_config_descriptor **desc);

int usb_init(void);
void usb_exit(void);

#endif /* __USBAPI_H_INCLUDE__ */
