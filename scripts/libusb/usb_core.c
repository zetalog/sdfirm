/*
 * Core functions for libusb
 * Copyright (C) 2007-2008 Daniel Drake <dsd@gentoo.org>
 * Copyright (c) 2001 Johannes Erdfelt <johannes@erdfelt.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "usb_int.h"

#if defined(LINUX)
const struct usbi_os_backend * const usbi_backend = &linux_usbfs_backend;
#elif defined(WIN32)
const struct usbi_os_backend * const usbi_backend = &win32_backend;
#else
#error "Unsupported OS"
#endif

struct libusb_context *usbi_default_context = NULL;

/* we traverse usbfs without knowing how many devices we are going to find.
 * so we create this discovered_devs model which is similar to a linked-list
 * which grows when required. it can be freed once discovery has completed,
 * eliminating the need for a list node in the libusb_device structure
 * itself. */
#define DISCOVERED_DEVICES_SIZE_STEP 8

static struct discovered_devs *discovered_devs_alloc(void)
{
	struct discovered_devs *ret =
		malloc(sizeof(*ret) + (sizeof(void *) * DISCOVERED_DEVICES_SIZE_STEP));

	if (ret) {
		ret->len = 0;
		ret->capacity = DISCOVERED_DEVICES_SIZE_STEP;
	}
	return ret;
}

/* append a device to the discovered devices collection. may realloc itself,
 * returning new discdevs. returns NULL on realloc failure. */
struct discovered_devs *discovered_devs_append(struct discovered_devs *discdevs,
					       struct libusb_device *dev)
{
	size_t len = discdevs->len;
	size_t capacity;

	/* if there is space, just append the device */
	if (len < discdevs->capacity) {
		discdevs->devices[len] = libusb_ref_device(dev);
		discdevs->len++;
		return discdevs;
	}

	/* exceeded capacity, need to grow */
	usbi_dbg("need to increase capacity");
	capacity = discdevs->capacity + DISCOVERED_DEVICES_SIZE_STEP;
	discdevs = realloc(discdevs,
		sizeof(*discdevs) + (sizeof(void *) * capacity));
	if (discdevs) {
		discdevs->capacity = capacity;
		discdevs->devices[len] = libusb_ref_device(dev);
		discdevs->len++;
	}

	return discdevs;
}

static void discovered_devs_free(struct discovered_devs *discdevs)
{
	size_t i;

	for (i = 0; i < discdevs->len; i++)
		libusb_unref_device(discdevs->devices[i]);

	free(discdevs);
}

/* Allocate a new device with a specific session ID. The returned device has
 * a reference count of 1. */
struct libusb_device *usbi_alloc_device(struct libusb_context *ctx,
					unsigned long session_id)
{
	size_t priv_size = usbi_backend->device_priv_size;
	struct libusb_device *dev = malloc(sizeof(*dev) + priv_size);

	if (!dev)
		return NULL;

	dev->ctx = ctx;
	dev->refcnt = 1;
	dev->session_data = session_id;
	memset(&dev->os_priv, 0, priv_size);

	list_add(&dev->list, &ctx->usb_devs);
	return dev;
}

/* Perform some final sanity checks on a newly discovered device. If this
 * function fails (negative return code), the device should not be added
 * to the discovered device list. */
int usbi_sanitize_device(struct libusb_device *dev)
{
	int r;
	unsigned char raw_desc[LIBUSB_DT_DEVICE_SIZE];
	uint8_t num_configurations;
	int host_endian;

	r = usbi_backend->get_device_descriptor(dev, raw_desc, &host_endian);
	if (r < 0)
		return r;

	num_configurations = raw_desc[LIBUSB_DT_DEVICE_SIZE - 1];
	if (num_configurations > USB_MAXCONFIG) {
		usbi_err(DEVICE_CTX(dev), "too many configurations");
		return LIBUSB_ERROR_IO;
	} else if (num_configurations < 1) {
		usbi_dbg("no configurations?");
		return LIBUSB_ERROR_IO;
	}

	dev->num_configurations = num_configurations;
	return 0;
}

/* Examine libusb's internal list of known devices, looking for one with
 * a specific session ID. Returns the matching device if it was found, and
 * NULL otherwise. */
struct libusb_device *usbi_get_device_by_session_id(struct libusb_context *ctx,
						    unsigned long session_id)
{
	struct libusb_device *dev;
	struct libusb_device *ret = NULL;

	list_for_each_entry(struct libusb_device, dev, &ctx->usb_devs, list) {
		if (dev->session_data == session_id) {
			ret = dev;
			break;
		}
	}

	return ret;
}

ssize_t libusb_get_device_list(libusb_context *ctx,
			       libusb_device ***list)
{
	struct discovered_devs *discdevs = discovered_devs_alloc();
	struct libusb_device **ret;
	int r = 0;
	size_t i;
	ssize_t len;
	USBI_GET_CONTEXT(ctx);

	if (!discdevs)
		return LIBUSB_ERROR_NO_MEM;

	r = usbi_backend->get_device_list(ctx, &discdevs);
	if (r < 0) {
		len = r;
		goto out;
	}

	/* convert discovered_devs into a list */
	len = discdevs->len;
	ret = malloc(sizeof(void *) * (len + 1));
	if (!ret) {
		len = LIBUSB_ERROR_NO_MEM;
		goto out;
	}

	ret[len] = NULL;
	for (i = 0; i < len; i++) {
		struct libusb_device *dev = discdevs->devices[i];
		ret[i] = libusb_ref_device(dev);
	}
	*list = ret;

out:
	discovered_devs_free(discdevs);
	return len;
}

void libusb_free_device_list(libusb_device **list, int unref_devices)
{
	if (!list)
		return;

	if (unref_devices) {
		int i = 0;
		struct libusb_device *dev;

		while ((dev = list[i++]) != NULL)
			libusb_unref_device(dev);
	}
	free(list);
}

uint8_t libusb_get_bus_number(libusb_device *dev)
{
	return dev->bus_number;
}

uint8_t libusb_get_device_address(libusb_device *dev)
{
	return dev->device_address;
}

int libusb_get_max_packet_size(libusb_device *dev,
			       unsigned char endpoint)
{
	int iface_idx;
	struct libusb_config_descriptor *config;
	int r;

	r = libusb_get_active_config_descriptor(dev, &config);
	if (r < 0) {
		usbi_err(DEVICE_CTX(dev),
			"could not retrieve active config descriptor");
		return LIBUSB_ERROR_OTHER;
	}

	r = LIBUSB_ERROR_NOT_FOUND;
	for (iface_idx = 0; iface_idx < config->bNumInterfaces; iface_idx++) {
		const struct libusb_interface *iface = &config->Interface[iface_idx];
		int altsetting_idx;

		for (altsetting_idx = 0; altsetting_idx < iface->num_altsetting;
				altsetting_idx++) {
			const struct libusb_interface_descriptor *altsetting
				= &iface->altsetting[altsetting_idx];
			int ep_idx;

			for (ep_idx = 0; ep_idx < altsetting->bNumEndpoints; ep_idx++) {
				const struct libusb_endpoint_descriptor *ep =
					&altsetting->endpoint[ep_idx];
				if (ep->bEndpointAddress == endpoint) {
					r = ep->wMaxPacketSize;
					goto out;
				}
			}
		}
	}

out:
	libusb_free_config_descriptor(config);
	return r;
}

libusb_device *libusb_ref_device(libusb_device *dev)
{
	dev->refcnt++;
	return dev;
}

void libusb_unref_device(libusb_device *dev)
{
	int refcnt;

	if (!dev)
		return;

	refcnt = --dev->refcnt;

	if (refcnt == 0) {
		usbi_dbg("destroy device %d.%d", dev->bus_number, dev->device_address);

		if (usbi_backend->destroy_device)
			usbi_backend->destroy_device(dev);

		list_del(&dev->list);

		free(dev);
	}
}

int libusb_open(libusb_device *dev, libusb_device_handle **handle)
{
	struct libusb_context *ctx = DEVICE_CTX(dev);
	struct libusb_device_handle *_handle;
	size_t priv_size = usbi_backend->device_handle_priv_size;
	int r;
	usbi_dbg("open %d.%d", dev->bus_number, dev->device_address);

	_handle = malloc(sizeof(*_handle) + priv_size);
	if (!_handle)
		return LIBUSB_ERROR_NO_MEM;

	_handle->dev = libusb_ref_device(dev);
	_handle->claimed_interfaces = 0;
	memset(&_handle->os_priv, 0, priv_size);

	r = usbi_backend->open(_handle);
	if (r < 0) {
		libusb_unref_device(dev);
		free(_handle);
		return r;
	}

	list_add(&_handle->list, &ctx->open_devs);
	*handle = _handle;

	return 0;
}

libusb_device_handle *libusb_open_device_with_vid_pid(libusb_context *ctx,
						      uint16_t vendor_id,
						      uint16_t product_id)
{
	struct libusb_device **devs;
	struct libusb_device *found = NULL;
	struct libusb_device *dev;
	struct libusb_device_handle *handle = NULL;
	size_t i = 0;
	int r;

	if (libusb_get_device_list(ctx, &devs) < 0)
		return NULL;

	while ((dev = devs[i++]) != NULL) {
		struct libusb_device_descriptor desc;
		r = libusb_get_device_descriptor(dev, &desc);
		if (r < 0)
			goto out;
		if (desc.idVendor == vendor_id && desc.idProduct == product_id) {
			found = dev;
			break;
		}
	}

	if (found) {
		r = libusb_open(found, &handle);
		if (r < 0)
			handle = NULL;
	}

out:
	libusb_free_device_list(devs, 1);
	return handle;
}

static void do_close(struct libusb_context *ctx,
		     struct libusb_device_handle *dev_handle)
{
	list_del(&dev_handle->list);

	usbi_backend->close(dev_handle);
	libusb_unref_device(dev_handle->dev);
	free(dev_handle);
}

void libusb_close(libusb_device_handle *dev_handle)
{
	struct libusb_context *ctx;

	if (!dev_handle)
		return;

	ctx = HANDLE_CTX(dev_handle);

	/* Close the device */
	do_close(ctx, dev_handle);
}

libusb_device *libusb_get_device(libusb_device_handle *dev_handle)
{
	return dev_handle->dev;
}

int libusb_get_configuration(libusb_device_handle *dev,
			     uint8_t *config)
{
	int r = LIBUSB_ERROR_NOT_SUPPORTED;

	if (usbi_backend->get_configuration)
		r = usbi_backend->get_configuration(dev, config);

	if (r == LIBUSB_ERROR_NOT_SUPPORTED) {
		uint8_t tmp = 0;
		usbi_dbg("falling back to control message");
		r = libusb_control_transfer(dev, LIBUSB_ENDPOINT_IN,
			LIBUSB_REQUEST_GET_CONFIGURATION, 0, 0, &tmp, 1, 1000);
		if (r == 0) {
			usbi_err(HANDLE_CTX(dev), "zero bytes returned in ctrl transfer?");
			r = LIBUSB_ERROR_IO;
		} else if (r == 1) {
			r = 0;
			*config = tmp;
		} else {
			usbi_dbg("control failed, error %d", r);
		}
	}

	if (r == 0)
		usbi_dbg("active config %d", *config);

	return r;
}

int libusb_set_configuration(libusb_device_handle *dev,
			     uint8_t configuration)
{
	usbi_dbg("configuration %d", configuration);
	return usbi_backend->set_configuration(dev, configuration);
}

int libusb_claim_interface(libusb_device_handle *dev,
			   int interface_number)
{
	int r = 0;

	usbi_dbg("interface %d", interface_number);
	if (interface_number >= sizeof(dev->claimed_interfaces) * 8)
		return LIBUSB_ERROR_INVALID_PARAM;

	if (dev->claimed_interfaces & (1 << interface_number))
		goto out;

	r = usbi_backend->claim_interface(dev, interface_number);
	if (r == 0)
		dev->claimed_interfaces |= 1 << interface_number;
out:
	return r;
}

int libusb_release_interface(libusb_device_handle *dev,
			     int interface_number)
{
	int r;

	usbi_dbg("interface %d", interface_number);
	if (interface_number >= sizeof(dev->claimed_interfaces) * 8)
		return LIBUSB_ERROR_INVALID_PARAM;

	if (!(dev->claimed_interfaces & (1 << interface_number))) {
		r = LIBUSB_ERROR_NOT_FOUND;
		goto out;
	}

	r = usbi_backend->release_interface(dev, interface_number);
	if (r == 0)
		dev->claimed_interfaces &= ~(1 << interface_number);
out:
	return r;
}

int libusb_set_interface_alt_setting(libusb_device_handle *dev,
				     int interface_number, int alternate_setting)
{
	usbi_dbg("interface %d altsetting %d",
		interface_number, alternate_setting);
	if (interface_number >= sizeof(dev->claimed_interfaces) * 8)
		return LIBUSB_ERROR_INVALID_PARAM;

	if (!(dev->claimed_interfaces & (1 << interface_number))) {
		return LIBUSB_ERROR_NOT_FOUND;
	}

	return usbi_backend->set_interface_altsetting(dev, interface_number,
		alternate_setting);
}

int libusb_clear_halt(libusb_device_handle *dev,
		      unsigned char endpoint)
{
	usbi_dbg("endpoint %x", endpoint);
	return usbi_backend->clear_halt(dev, endpoint);
}

int libusb_reset_device(libusb_device_handle *dev)
{
	return usbi_backend->reset_device(dev);
}

int libusb_kernel_driver_active(libusb_device_handle *dev,
				int Interface)
{
	usbi_dbg("interface %d", Interface);
	if (usbi_backend->kernel_driver_active)
		return usbi_backend->kernel_driver_active(dev, Interface);
	else
		return LIBUSB_ERROR_NOT_SUPPORTED;
}

int libusb_detach_kernel_driver(libusb_device_handle *dev,
				int Interface)
{
	usbi_dbg("interface %d", Interface);
	if (usbi_backend->detach_kernel_driver)
		return usbi_backend->detach_kernel_driver(dev, Interface);
	else
		return LIBUSB_ERROR_NOT_SUPPORTED;
}

int libusb_attach_kernel_driver(libusb_device_handle *dev,
				int Interface)
{
	usbi_dbg("interface %d", Interface);
	if (usbi_backend->attach_kernel_driver)
		return usbi_backend->attach_kernel_driver(dev, Interface);
	else
		return LIBUSB_ERROR_NOT_SUPPORTED;
}

/* Set message verbosity.
 *  - Level 0: no messages ever printed by the library (default)
 *  - Level 1: error messages are printed to stderr
 *  - Level 2: warning and error messages are printed to stderr
 *  - Level 3: informational messages are printed to stdout, warning and error
 *    messages are printed to stderr
 *
 * The default level is 0, which means no messages are ever printed. If you
 * choose to increase the message verbosity level, ensure that your
 * application does not close the stdout/stderr file descriptors.
 */
void libusb_set_debug(libusb_context *ctx, int level)
{
	USBI_GET_CONTEXT(ctx);
	if (!ctx->debug_fixed)
		ctx->debug = level;
}

int libusb_init(libusb_context **context)
{
	char *dbg = getenv("LIBUSB_DEBUG");
	struct libusb_context *ctx = malloc(sizeof(*ctx));
	int r;

	if (!ctx)
		return LIBUSB_ERROR_NO_MEM;
	memset(ctx, 0, sizeof(*ctx));

	if (dbg) {
		ctx->debug = atoi(dbg);
		if (ctx->debug)
			ctx->debug_fixed = 1;
	}

	if (!usbi_default_context) {
		usbi_default_context = ctx;
		usbi_dbg("created default context");
	}

	if (usbi_backend->init) {
		r = usbi_backend->init(ctx);
		if (r)
			goto err;
	}

	list_init(&ctx->usb_devs);
	list_init(&ctx->open_devs);

	list_init(&ctx->flying_transfers);
	list_init(&ctx->pollfds);

	if (context)
		*context = ctx;
	return 0;

err:
	free(ctx);
	return r;
}

void libusb_exit(struct libusb_context *ctx)
{
	USBI_GET_CONTEXT(ctx);

	/* a little sanity check. doesn't bother with open_devs locking because
	 * unless there is an application bug, nobody will be accessing this. */
	if (!list_empty(&ctx->open_devs))
		usbi_warn(ctx, "application left some devices open");

	if (usbi_backend->exit)
		usbi_backend->exit();

	if (ctx == usbi_default_context) {
		usbi_dbg("freeing default context");
		usbi_default_context = NULL;
	}

	free(ctx);
}

void usbi_logv(struct libusb_context *ctx, enum usbi_log_level level,
	       const char *format, va_list args)
{
	FILE *stream = stdout;
	const char *prefix;

#ifndef ENABLE_DEBUG_LOGGING
	USBI_GET_CONTEXT(ctx);
	if (!ctx->debug)
		return;
	if (level == LOG_LEVEL_WARNING && ctx->debug < 2)
		return;
	if (level == LOG_LEVEL_INFO && ctx->debug < 3)
		return;
#endif

	switch (level) {
	case LOG_LEVEL_INFO:
		prefix = "info";
		break;
	case LOG_LEVEL_WARNING:
		stream = stderr;
		prefix = "warning";
		break;
	case LOG_LEVEL_ERROR:
		stream = stderr;
		prefix = "error";
		break;
	case LOG_LEVEL_DEBUG:
		stream = stderr;
		prefix = "debug";
		break;
	default:
		stream = stderr;
		prefix = "unknown";
		break;
	}

	fprintf(stream, "libusb:%s ", prefix);
	vfprintf(stream, format, args);

	fprintf(stream, "\n");
}

void usbi_log(struct libusb_context *ctx, enum usbi_log_level level,
	      const char *format, ...)
{
	va_list args;

	va_start(args, format);
	usbi_logv(ctx, level, format, args);
	va_end(args);
}

void _usbi_logv(libusb_context *ctx, enum usbi_log_level level,
		const char *fmt, va_list args)
{
	usbi_logv(ctx, level, fmt, args);
}

void _usbi_log(libusb_context *ctx, enum usbi_log_level level,
	       const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	_usbi_logv(ctx, level, fmt, args);
	va_end(args);
}

void usbi_dbg(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	_usbi_logv(NULL, LOG_LEVEL_DEBUG, fmt, args);
	va_end(args);
}

void usbi_info(libusb_context *ctx, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	_usbi_logv(ctx, LOG_LEVEL_INFO, fmt, args);
	va_end(args);
}

void usbi_warn(libusb_context *ctx, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	_usbi_logv(ctx, LOG_LEVEL_WARNING, fmt, args);
	va_end(args);
}

void usbi_err(libusb_context *ctx, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	_usbi_logv(ctx, LOG_LEVEL_ERROR, fmt, args);
	va_end(args);
}
