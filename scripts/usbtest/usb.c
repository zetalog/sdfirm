#include "usb_priv.h"

#define USB_DEBUG_LEVEL		0

uint16_t g_usb_pid = 0;
uint16_t g_usb_vid = 0;
uint16_t g_usb_did = 0;
uint8_t g_usb_iid = 0;
uint8_t g_usb_sid = 0;
uint8_t g_usb_cid = 0;
int g_usb_bus = -1;
int g_usb_dev = -1;

static libusb_device_handle *usb_dev_handle = NULL;
static libusb_context *g_ctx = NULL;
static libusb_device **g_devs = NULL;
static int g_dev_cnt = 0;
static boolean g_usb_claimed = false;

static const char *usb_endpoint_type_name(uint8_t attr)
{
	/* ep type bit 0,1 */
	if (attr == LIBUSB_TRANSFER_TYPE_INTERRUPT)
		return "interrupt";
	if (attr == LIBUSB_TRANSFER_TYPE_BULK)
		return "bulk";
	if (attr == LIBUSB_TRANSFER_TYPE_ISOCHRONOUS)
		return "isochronous";
	if (attr == LIBUSB_TRANSFER_TYPE_CONTROL)
		return "control";
	return "!!unknown!!";
}

void usb_dump_ep_descriptor(struct libusb_endpoint_descriptor *ep)
{
	usb_log(USB_LOG_DEBUG, "Endpoint Descriptor:");
	usb_log(USB_LOG_DEBUG, "  bLength             %d", ep->bLength);
	usb_log(USB_LOG_DEBUG, "  bDescriptorType     %d", ep->bDescriptorType);
	usb_log(USB_LOG_DEBUG, "  bEndpointAddress    %d", ep->bEndpointAddress);
	usb_log(USB_LOG_DEBUG, "  bmAttributes        %d", ep->bmAttributes);
	usb_log(USB_LOG_DEBUG, "    Type              %s",
		usb_endpoint_type_name(ep->bmAttributes));
	usb_log(USB_LOG_DEBUG, "  wMaxPacketSize      %d", ep->wMaxPacketSize);
	usb_log(USB_LOG_DEBUG, "  bInterval           %d", ep->bInterval);
	usb_log(USB_LOG_DEBUG, "  bRefresh            %d", ep->bRefresh);
	usb_log(USB_LOG_DEBUG, "  bSynchAddress       %d", ep->bSynchAddress);
	usb_log(USB_LOG_DEBUG, "  bRefresh            %d", ep->bRefresh);
}

int usb_get_context(struct usb_driver_ctx *ctx)
{
	if (ctx) {
		ctx->idProduct = g_usb_pid;
		ctx->idVendor = g_usb_vid;
		ctx->bcdDevice = g_usb_did;
		ctx->bConfigurationValue = g_usb_cid;
		ctx->bInterfaceValue = g_usb_iid;
		ctx->bAlternateSetting = g_usb_sid;
		ctx->nBusIdentifier = g_usb_bus;
		ctx->nDevIdentifier = g_usb_dev;
		return 0;
	}
	return -1;
}

boolean usb_scan_interface(struct libusb_interface *intf,
			   uint8_t clazz, usb_parse_cb parser)
{
	int nr_alt;
	int altid;

	if (g_usb_claimed) {
		libusb_release_interface(usb_dev_handle, g_usb_iid);
		g_usb_claimed = false;
	}
	if (libusb_claim_interface(usb_dev_handle, g_usb_iid)) {
		usb_log(USB_LOG_ERR, "libusb_claim_interface failure.");
		return false;
	}
	g_usb_claimed = true;

	nr_alt = intf->num_altsetting;
	for (altid = 0; altid < nr_alt; altid++) {
		if (intf->altsetting->bInterfaceClass == clazz &&
		    parser(g_usb_cid, intf->altsetting)) {
			g_usb_sid = altid;
			if (nr_alt != 1 &&
			    libusb_set_interface_alt_setting(usb_dev_handle,
							     g_usb_iid, altid)) {
				usb_log(USB_LOG_ERR,
					"libusb_set_interface_alt_setting failure.");
				continue;
			}
			return true;
		}
	}

	return false;
}

boolean usb_scan_config(uint8_t clazz, usb_parse_cb parser)
{
	int infid;
	struct libusb_config_descriptor *dt_cnf = NULL;
	boolean res = false;
	struct libusb_interface *intf;

	libusb_get_config_descriptor(g_devs[g_usb_dev],
				     (uint8_t)(g_usb_cid-1), &dt_cnf);
	if (!dt_cnf) {
		return res;
	}

	for (infid = 0; infid < dt_cnf->bNumInterfaces; infid++) {
		intf = (struct libusb_interface *)dt_cnf->Interface + infid;

		g_usb_iid = infid;
		if (usb_scan_interface(intf, clazz, parser)) {
			res = true;
			goto out;
		}
	}

out:
	libusb_free_config_descriptor(dt_cnf);
	return res;
}

boolean usb_probe_class_driver(uint8_t clazz, usb_parse_cb parser)
{
	struct libusb_device_descriptor desc;
	int devid;
	uint8_t nr_cnf;

	for (devid = 0; devid < g_dev_cnt; devid++) {
		libusb_get_device_descriptor(g_devs[devid], &desc);
		nr_cnf = desc.bNumConfigurations;

		g_usb_pid = desc.idProduct;
		g_usb_vid = desc.idVendor;
		g_usb_did = desc.bcdDevice;
		g_usb_dev = devid;

		if (libusb_open(g_devs[devid], &usb_dev_handle)) {
			usb_log(USB_LOG_ERR, "libusb_open failure");
			continue;
		}

		for (g_usb_cid = 1; g_usb_cid <= nr_cnf; g_usb_cid++) {
			if (libusb_set_configuration(usb_dev_handle, g_usb_cid)) {
				usb_log(USB_LOG_ERR, "set_configuration failure.");
				/* Just report status and not exit or else since:
				 * Linux has set config for all one config
				 * device so we will get fail in this case.
				 * Windows always need set config.
				 */
			}
			if (usb_scan_config(clazz, parser)) {
				return true;
			}
		}
		libusb_close(usb_dev_handle);
		usb_dev_handle = NULL;
	}
	return false;
}

int usb_control_transfer(uint8_t bmRequestType,
			 uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
			 uint8_t *data, uint16_t length, int timeout)
{
	return libusb_control_transfer(usb_dev_handle, bmRequestType,
				       bRequest, wValue, wIndex,
				       data, length, timeout);
}

int usb_sync_interrupt_transfer(uint8_t endpoint, uint8_t *data, int length,
				int *actual_length, int timeout)
{
	return libusb_interrupt_transfer(usb_dev_handle, endpoint, 
					 data, length, actual_length, timeout);
}

#if 0
static void intr_transfer_cb(struct libusb_transfer *transfer)
{}

int usb_async_interrupt_transfer(uint8_t endpoint, uint8_t *data, int length,
				 int *actual_length, usb_intr_cb cb, void *user_data,
				 int timeout)
{
	struct libusb_transfer *transfer = libusb_alloc_transfer(0);

	if (!transfer)
		return LIBUSB_ERROR_NO_MEM;

	libusb_fill_interrupt_transfer(transfer, usb_dev_handle, endpoint,
				       data, length, intr_transfer_cb, user_data, timeout);
	transfer->type = LIBUSB_TRANSFER_TYPE_INTERRUPT;
}
#endif

int usb_bulk_transfer(unsigned char endpoint,
		      uint8_t *data, int length,
		      int *transferred, int timeout)
{
	return libusb_bulk_transfer(usb_dev_handle, endpoint,
				    data, length,
				    transferred, timeout);
}

int usb_get_device_descriptor(struct libusb_device_descriptor *desc)
{
	libusb_device *dev = libusb_get_device(usb_dev_handle);
	if (!dev)
		return -1;
	return libusb_get_device_descriptor(dev, desc);
}

int usb_get_config_descriptor(uint8_t bConfigurationValue,
			      struct libusb_config_descriptor **desc)
{
	libusb_device *dev = libusb_get_device(usb_dev_handle);
	if (!dev)
		return -1;
	return libusb_get_config_descriptor_by_value(dev, bConfigurationValue, desc);
}

int usb_get_descriptor(uint8_t desc_type, uint8_t desc_index,
		       unsigned char *data, uint16_t length,
		       int timeout)
{
	return libusb_get_descriptor(usb_dev_handle, desc_type,
				     desc_index, data, length, timeout);
}

int usb_get_string(uint8_t desc_index, uint16_t langid,
		   unsigned char *data, uint16_t length,
		   int timeout)
{
	return libusb_get_string_descriptor(usb_dev_handle, desc_index,
					    langid, data, length, timeout);
}

int usb_get_device(unsigned char *data, uint16_t length, int timeout)
{
	return usb_get_descriptor(LIBUSB_DT_DEVICE, 0, data,
				  length, timeout);
}

int usb_init(void)
{
	if (libusb_init(&g_ctx)) {
		usb_log(USB_LOG_ERR, "libusb_init failure.");
		return -1;
	}
	if (g_ctx)
		libusb_set_debug(g_ctx, USB_DEBUG_LEVEL);

	g_dev_cnt = libusb_get_device_list(g_ctx, &g_devs);
	if (g_dev_cnt <= 0) {
		usb_log(USB_LOG_ERR, "libusb_get_device_list failure.");
		return -1;
	}
	usb_log(USB_LOG_INFO, "usb_init success.");
	return 0;
}

void usb_exit(void)
{
	if (g_usb_claimed) {
		libusb_release_interface(usb_dev_handle, g_usb_iid);
		g_usb_claimed = false;
	}
	if (usb_dev_handle) {
		libusb_close(usb_dev_handle);
		usb_dev_handle = NULL;
	}
	if (g_devs) {
		libusb_free_device_list(g_devs, 0);
		g_devs = NULL;
	}
	if (g_ctx) {
		libusb_exit(g_ctx);
		g_ctx = NULL;
	}
}
