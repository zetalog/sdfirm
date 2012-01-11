/* LIBUSB-WIN32, Generic Windows USB Library
 * Copyright (c) 2002-2005 Stephan Meyer <ste_meyer@web.de>
 * Copyright (c) 2000-2005 Johannes Erdfelt <johannes@erdfelt.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "usb_int.h"
#include "usb_win32.h"

#define LIBUSB_DEFAULT_TIMEOUT	5000
#define LIBUSB_DEVICE_NAME	"\\\\.\\libusb0-"
#define LIBUSB_BUS_NAME		"bus-0"
#define LIBUSB_MAX_DEVICES	256

/* PATH_MAX from limits.h can't be used on Windows if the dll and
 * import libraries are build/used by different compilers 
 */
#define LIBUSB_PATH_MAX 512

/* Version information, Windows specific */
struct usb_version {
	struct {
		int major;
		int minor;
		int micro;
		int nano;
	} dll;
	struct {
		int major;
		int minor;
		int micro;
		int nano;
	} driver;
};

struct win32_device_priv {
	uint8_t bus_number;
	uint8_t device_address;
	char filename[LIBUSB_PATH_MAX];
	unsigned char dev_descriptor[LIBUSB_DT_DEVICE_SIZE];
	unsigned char *config_descriptor;
};

struct win32_device_handle_priv {
	HANDLE dev_handle;
};

enum reap_action {
	NORMAL = 0,
	/* submission failed after the first URB, so await
	 * cancellation/completion of all the others
	 */
	SUBMIT_FAILED,

	/* cancelled by user or timeout */
	CANCELLED,

	/* completed multi-URB transfer in non-final URB */
	COMPLETED_EARLY,
};

struct win32_transfer_priv {
	OVERLAPPED ol;
	int submitted;
	int requested;
	int replied;
	DWORD control_code;
	libusb_request req;
	enum reap_action reap_action;
};

#define VERSION_MAJOR	0
#define VERSION_MINOR	1
#define VERSION_MICRO	12
#define VERSION_NANO	1

static struct usb_version _usb_version = {
	{
		VERSION_MAJOR, 
		VERSION_MINOR, 
		VERSION_MICRO, 
		VERSION_NANO,
	},
	{
		-1, -1, -1, -1,
	}
};

static int _usb_io_sync(HANDLE dev, unsigned int code,
			void *in, int in_size,
                        void *out, int out_size, int *ret);

void chomp(char *line)
{
	char *last;
	
	for (last = line + strlen(line); last >= line; last--) {
		if (strchr(" \r\n\t", *last))
			*last = '\0';
		else
			break;
	}
}

static const char *usb_win_error_to_string(void)
{
	static char tmp[512];

	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
		      LANG_USER_DEFAULT, tmp, sizeof(tmp) - 1, NULL);
	chomp(tmp);
	return tmp;
}

int usb_win_error_to_errno(void)
{
	switch (GetLastError()) {
	case ERROR_SUCCESS:
		return 0;
	case ERROR_INVALID_PARAMETER:
		return EINVAL;
	case ERROR_SEM_TIMEOUT: 
	case ERROR_OPERATION_ABORTED:
		return ETIMEDOUT;
	case ERROR_NOT_ENOUGH_MEMORY:
		return ENOMEM;
	default:
		return EIO;
	}
}

static void __win32_libusb_filename(int devaddr, char *filename)
{
	_snprintf(filename, LIBUSB_PATH_MAX - 1,"%s%04d",
		  LIBUSB_DEVICE_NAME, devaddr);
}

static struct win32_device_priv *__device_priv(struct libusb_device *dev)
{
	return (struct win32_device_priv *)dev->os_priv;
}


/* takes a usbfs fd, attempts to find the requested config and copy a certain
 * amount of it into an output buffer. */
static int get_config_descriptor(struct libusb_context *ctx, HANDLE fd,
				 uint8_t config_index,
				 unsigned char *buffer, size_t len)
{
	int ret;
	libusb_request req;

	/* retrieve config descriptor */
	req.descriptor.type = LIBUSB_DT_CONFIG;
	req.descriptor.recipient = LIBUSB_RECIPIENT_DEVICE;
	req.descriptor.index = config_index;
	req.descriptor.language_id = 0;
	req.timeout = LIBUSB_DEFAULT_TIMEOUT;
	
	if (!_usb_io_sync(fd, LIBUSB_IOCTL_GET_DESCRIPTOR,
			  &req, sizeof(libusb_request), buffer, len, &ret)) {
		usbi_err(ctx,
			 "sending control message failed, "
			 "win error: %s", usb_win_error_to_string());
		return -usb_win_error_to_errno();
	}
	return 0;
}

/* cache the active config descriptor in memory. a value of -1 means that
 * we aren't sure which one is active, so just assume the first one. 
 * only for usbfs. */
static int cache_active_config(struct libusb_device *dev, HANDLE fd,
			       uint8_t active_config)
{
	struct win32_device_priv *priv = __device_priv(dev);
	struct libusb_config_descriptor config;
	unsigned char tmp[8];
	unsigned char *buf;
	uint8_t idx;
	int r;

	if (active_config == -1) {
		idx = 0;
	} else {
		r = usbi_get_config_index_by_value(dev, active_config, &idx);
		if (r < 0)
			return r;
		if (idx == -1)
			return LIBUSB_ERROR_NOT_FOUND;
	}

	r = get_config_descriptor(DEVICE_CTX(dev), fd, idx, tmp, sizeof(tmp));
	if (r < 0) {
		usbi_err(DEVICE_CTX(dev), "first read error %d", r);
		return r;
	}

	usbi_parse_descriptor(tmp, "bbw", &config, 1);
	buf = malloc(config.wTotalLength);
	if (!buf)
		return LIBUSB_ERROR_NO_MEM;

	r = get_config_descriptor(DEVICE_CTX(dev), fd, idx, buf,
				  config.wTotalLength);
	if (r < 0) {
		free(buf);
		return r;
	}

	if (priv->config_descriptor)
		free(priv->config_descriptor);
	priv->config_descriptor = buf;
	return 0;
}

static struct win32_device_handle_priv *__device_handle_priv(struct libusb_device_handle *handle)
{
	return (struct win32_device_handle_priv *)handle->os_priv;
}

static int get_active_config(struct libusb_device *dev, HANDLE fd)
{
	unsigned char active_config = 0;
	libusb_request req;
	int ret;

	req.configuration.configuration = 0;
	req.timeout = LIBUSB_DEFAULT_TIMEOUT;

	/* retrieve config descriptor */
	if (!_usb_io_sync(fd, LIBUSB_IOCTL_GET_CONFIGURATION,
			  &req, sizeof (libusb_request), &active_config, 1, &ret)) {
		usbi_err(DEVICE_CTX(dev),
			 "sending control message failed, "
			 "win error: %s", usb_win_error_to_string());
		return -usb_win_error_to_errno();
	}
	return active_config;
}

static int initialize_device(struct libusb_device *dev, uint8_t busnum,
			     uint8_t devaddr)
{
	struct win32_device_priv *priv = __device_priv(dev);
	HANDLE fd;
	uint8_t active_config = 0;
	int device_configured = 1;
	int ret;
	libusb_request req;

	dev->bus_number = priv->bus_number = busnum;
	dev->device_address = priv->device_address = devaddr;

	/* cache device descriptor in memory so that we can retrieve it later
	 * without waking the device up (op_get_device_descriptor) */

	priv->config_descriptor = NULL;

	__win32_libusb_filename(dev->device_address, priv->filename);
	
	fd = CreateFile(priv->filename, 0, 0, NULL, OPEN_EXISTING, 
			FILE_FLAG_OVERLAPPED, NULL);

	if (dev == INVALID_HANDLE_VALUE) {
		usbi_err(DEVICE_CTX(dev), "open failed, ret=%d errno=%d", fd, errno);
		return LIBUSB_ERROR_IO;
	}

	/* retrieve device descriptor */
	req.descriptor.type = LIBUSB_DT_DEVICE;
	req.descriptor.recipient = LIBUSB_RECIPIENT_DEVICE;
	req.descriptor.index = 0;
	req.descriptor.language_id = 0;
	req.timeout = LIBUSB_DEFAULT_TIMEOUT;
	
	if (!_usb_io_sync(fd, LIBUSB_IOCTL_GET_DESCRIPTOR,
			  &req, sizeof(libusb_request),
			  &priv->dev_descriptor, LIBUSB_DT_DEVICE_SIZE, &ret)) {
		usbi_err(DEVICE_CTX(dev),
			 "sending control message failed, "
			 "win error: %s", usb_win_error_to_string());
		CloseHandle(fd);
		return -usb_win_error_to_errno();
	}
  
	dev->num_configurations = priv->dev_descriptor[LIBUSB_DT_DEVICE_SIZE - 1];

	active_config = get_active_config(dev, fd);
	if (active_config < 0) {
		CloseHandle(fd);
		return active_config;
	} else if (active_config == 0) {
		/* some buggy devices have a configuration 0, but we're
		 * reaching into the corner of a corner case here, so let's
		 * not support buggy devices in these circumstances.
		 * stick to the specs: a configuration value of 0 means
		 * unconfigured. */
		usbi_dbg("assuming unconfigured device");
		device_configured = 0;
	}

	if (device_configured) {
		ret = cache_active_config(dev, fd, active_config);
		if (ret < 0) {
			CloseHandle(fd);
			return ret;
		}
	}

	CloseHandle(fd);
	return 0;
}

static int enumerate_device(struct libusb_context *ctx,
			    struct discovered_devs **_discdevs,
			    const char *sysfs_dir)
{
	struct discovered_devs *discdevs;
	unsigned long session_id;
	int need_unref = 0;
	struct libusb_device *dev;
	int i, r = 0;
	uint8_t busnum, devaddr;

	for (i = 1; i < LIBUSB_MAX_DEVICES; i++) {
		busnum = 0;
		devaddr = i;
		
		session_id = busnum << 8 | devaddr;
		
		dev = usbi_get_device_by_session_id(ctx, session_id);
		if (dev) {
			usbi_dbg("using existing device for %d/%d (session %ld)",
				 busnum, devaddr, session_id);
		} else {
			HANDLE *hd;
			char filename[LIBUSB_PATH_MAX];
			
			__win32_libusb_filename(i, filename);
			hd = CreateFile(filename, 0, 0, NULL, OPEN_EXISTING,
					FILE_FLAG_OVERLAPPED, NULL);
			if (hd == INVALID_HANDLE_VALUE)
				continue;
			usbi_dbg("allocating new device for %d/%d (session %ld)",
				busnum, devaddr, session_id);
			
			CloseHandle(hd);
			dev = usbi_alloc_device(ctx, session_id);
			if (!dev)
				return LIBUSB_ERROR_NO_MEM;
			need_unref = 1;
			r = initialize_device(dev, busnum, devaddr);
			if (r < 0)
				goto out;
			r = usbi_sanitize_device(dev);
			if (r < 0)
				goto out;
			discdevs = discovered_devs_append(*_discdevs, dev);
			if (!discdevs)
				r = LIBUSB_ERROR_NO_MEM;
			else
				*_discdevs = discdevs;
		}
	}


out:
	if (need_unref)
		libusb_unref_device(dev);
	return r;
}

static int op_get_device_list(struct libusb_context *ctx, 
			      struct discovered_devs **_discdevs)
{
	struct discovered_devs *discdevs = *_discdevs;

	/* determine whether a libusb0 device on bus */
	return enumerate_device(ctx, &discdevs, NULL);
}

static int op_get_device_descriptor(struct libusb_device *dev,
				    unsigned char *buffer,
				    int *host_endian)
{
	struct win32_device_priv *priv = __device_priv(dev);

	*host_endian = 1;
	/* return cached copy */
	memcpy(buffer, priv->dev_descriptor, LIBUSB_DT_DEVICE_SIZE);
	return 0;
}

static int op_get_active_config_descriptor(struct libusb_device *dev,
					   unsigned char *buffer,
					   size_t len, int *host_endian)
{
	struct win32_device_priv *priv = __device_priv(dev);

	if (!priv->config_descriptor)
		return LIBUSB_ERROR_NOT_FOUND; /* device is unconfigured */

	/* retrieve cached copy */
	memcpy(buffer, priv->config_descriptor, len);
	return 0;
}

static int op_get_config_descriptor(struct libusb_device *dev,
				    uint8_t config_index,
				    unsigned char *buffer, size_t len,
				    int *host_endian)
{
	struct win32_device_priv *priv = __device_priv(dev);
	HANDLE fd;
	int ret;

	fd = CreateFile(priv->filename, 0, 0, NULL, OPEN_EXISTING, 
			FILE_FLAG_OVERLAPPED, NULL);
	if (fd == INVALID_HANDLE_VALUE) {
		usbi_err(DEVICE_CTX(dev),
			 "open failed, ret=%d errno=%d", fd, errno);
		return LIBUSB_ERROR_IO;
	}
	ret = get_config_descriptor(DEVICE_CTX(dev), fd, config_index, buffer, len);
	CloseHandle(fd);
	return ret;
}

static int op_open(struct libusb_device_handle *handle)
{
	struct win32_device_handle_priv *hpriv = __device_handle_priv(handle);
	struct win32_device_priv *priv = __device_priv(handle->dev);

	hpriv->dev_handle = CreateFile(priv->filename, 0, 0, NULL,
				       OPEN_EXISTING,
				       FILE_FLAG_OVERLAPPED, NULL);
	if (hpriv->dev_handle == INVALID_HANDLE_VALUE) {
		if (errno == EACCES) {
			usbi_err(HANDLE_CTX(handle),
				 "libusb couldn't open USB device %s: "
				 "Permission denied.",
				 priv->filename);
			return LIBUSB_ERROR_ACCESS;
		} else if (errno == ENOENT) {
			return LIBUSB_ERROR_NO_DEVICE;
		} else {
			usbi_err(HANDLE_CTX(handle),
				"open failed, code %d errno %d",
				hpriv->dev_handle, errno);
			return LIBUSB_ERROR_IO;
		}
	}

	return 0;
}

static int op_close(libusb_device_handle *handle)
{
	struct win32_device_handle_priv *hpriv = __device_handle_priv(handle);

	if (hpriv->dev_handle != INVALID_HANDLE_VALUE) {
		CloseHandle(hpriv->dev_handle);
		hpriv->dev_handle = INVALID_HANDLE_VALUE;
	}
	return 0;
}

static int op_set_configuration(libusb_device_handle *handle, uint8_t config)
{
	libusb_request req;
	struct win32_device_handle_priv *hpriv = __device_handle_priv(handle);
	
	if (hpriv->dev_handle == INVALID_HANDLE_VALUE) {
		usbi_dbg("usb_set_configuration: error: device not open");
		return -EINVAL;
	}
	
	/* FIXME: the count */
	req.configuration.configuration = config;
	req.timeout = LIBUSB_DEFAULT_TIMEOUT;
	
	if (!_usb_io_sync(hpriv->dev_handle, LIBUSB_IOCTL_SET_CONFIGURATION, 
			  &req, sizeof(libusb_request), NULL, 0, NULL)) {
		usbi_dbg("usb_set_configuration: could not set config %d: "
			"win error: %s", config, usb_win_error_to_string());
		return -usb_win_error_to_errno();
	}
		
	return 0;
}

static int op_get_configuration(struct libusb_device_handle *handle,
				uint8_t *config)
{
	struct win32_device_handle_priv *hpriv = __device_handle_priv(handle);
	uint8_t active_config;
	
	if (hpriv->dev_handle == INVALID_HANDLE_VALUE) {
		usbi_dbg("usb_claim_interface: device not open");
		return -EINVAL;
	}

	active_config = get_active_config(handle->dev, hpriv->dev_handle);
	if (active_config < 0)
		*config = 0;
	else
		*config = active_config;
	return 0;
}

static int op_claim_interface(libusb_device_handle *dev, int Interface)
{
	libusb_request req;
	struct win32_device_handle_priv *hpriv = __device_handle_priv(dev);

	if (hpriv->dev_handle == INVALID_HANDLE_VALUE) {
		usbi_dbg("usb_claim_interface: device not open");
		return -EINVAL;
	}
	
	req.Interface.Interface = Interface;
	
	if (!_usb_io_sync(hpriv->dev_handle, LIBUSB_IOCTL_CLAIM_INTERFACE, 
			  &req, sizeof(libusb_request), NULL, 0, NULL)) {
		usbi_dbg("usb_claim_interface: could not claim interface %d, "
			  "win error: %s", Interface, usb_win_error_to_string());
		return -usb_win_error_to_errno();
	} else {
		return 0;
	}
}

int op_release_interface(libusb_device_handle *handle, int Interface)
{
	libusb_request req;
	struct win32_device_handle_priv *hpriv = __device_handle_priv(handle);
	
	if (hpriv->dev_handle == INVALID_HANDLE_VALUE) {
		usbi_dbg("usb_release_interface: device not open");
		return -EINVAL;
	}
	
	req.Interface.Interface = Interface;
	
	if (!_usb_io_sync(hpriv->dev_handle, LIBUSB_IOCTL_RELEASE_INTERFACE, 
			  &req, sizeof(libusb_request), NULL, 0, NULL)) {
		usbi_dbg("usb_release_interface: could not release interface %d, "
			  "win error: %s", Interface, usb_win_error_to_string());
		return -usb_win_error_to_errno();
	} else {
		return 0;
	}
}

static int op_set_interface(struct libusb_device_handle *handle,
			    int iface, int altsetting)
{
	libusb_request req;
	struct win32_device_handle_priv *hpriv = __device_handle_priv(handle);
	
	if (hpriv->dev_handle == INVALID_HANDLE_VALUE) {
		usbi_dbg("usb_release_interface: device not open");
		return -EINVAL;
	}

	req.Interface.Interface = iface;
	req.Interface.altsetting = altsetting;
	if (!_usb_io_sync(hpriv->dev_handle, LIBUSB_IOCTL_SET_INTERFACE, 
			  &req, sizeof(libusb_request), NULL, 0, NULL)) {
		usbi_dbg("usb_set_interface: could not set interface %d, "
			 "altsetting %d, win error: %s",
			 iface, altsetting, usb_win_error_to_string());
		return -usb_win_error_to_errno();
	} else {
		return 0;
	}

}

int op_clear_halt(libusb_device_handle *handle,
		  unsigned char ep)
{
	libusb_request req;
	struct win32_device_handle_priv *hpriv = __device_handle_priv(handle);
	
	if (hpriv->dev_handle == INVALID_HANDLE_VALUE) {
		usbi_dbg("usb_clear_halt: device not open");
		return -EINVAL;
	}
	
	req.endpoint.endpoint = (int)ep;
	req.timeout = LIBUSB_DEFAULT_TIMEOUT;
	
	if (!_usb_io_sync(hpriv->dev_handle, LIBUSB_IOCTL_RESET_ENDPOINT,
			  &req, sizeof(libusb_request), NULL, 0, NULL)) {
		usbi_dbg("usb_clear_halt: could not clear halt, ep 0x%02x, "
			  "win error: %s", ep, usb_win_error_to_string());
		return -usb_win_error_to_errno();
	}
	
	return 0;
}

int op_reset_device(libusb_device_handle *handle)
{
	libusb_request req;
	struct win32_device_handle_priv *hpriv = __device_handle_priv(handle);
	
	if (hpriv->dev_handle == INVALID_HANDLE_VALUE) {
		usbi_dbg("usb_reset: device not open");
		return -EINVAL;
	}
	
	req.timeout = LIBUSB_DEFAULT_TIMEOUT;
	
	if (!_usb_io_sync(hpriv->dev_handle, LIBUSB_IOCTL_RESET_DEVICE,
			  &req, sizeof(libusb_request), NULL, 0, NULL)) {
		usbi_dbg("usb_reset: could not reset device, win error: %s", 
			  usb_win_error_to_string());
		return -usb_win_error_to_errno();
	}
	
	return 0;
}

static int op_clock_gettime(int clk_id, struct timespec *tp)
{
	switch (clk_id) {
	case USBI_CLOCK_MONOTONIC:
		return clock_gettime(CLOCK_MONOTONIC, tp);
	case USBI_CLOCK_REALTIME:
		return clock_gettime(CLOCK_REALTIME, tp);
	default:
		return LIBUSB_ERROR_INVALID_PARAM;
	}
}

static void op_destroy_device(struct libusb_device *dev)
{
	struct win32_device_priv *priv = __device_priv(dev);
	if (priv->config_descriptor)
		free(priv->config_descriptor);
}

static int op_kernel_driver_active(struct libusb_device_handle *handle,
				   int Interface)
{
	return 0;
}

static int op_detach_kernel_driver(struct libusb_device_handle *handle,
				   int Interface)
{
	return 0;
}

static int op_attach_kernel_driver(struct libusb_device_handle *handle,
				   int Interface)
{
	return LIBUSB_ERROR_NOT_FOUND;
}

static int op_init(struct libusb_context *ctx)
{
	HANDLE dev;
	libusb_request req;
	int i;
	char dev_name[LIBUSB_PATH_MAX];
	int ret = LIBUSB_ERROR_OTHER;
		
	/* determine whether a libusb0 device on bus */
	for (i = 1; i < LIBUSB_MAX_DEVICES; i++) {
		/* build the Windows file name */
		__win32_libusb_filename(i, dev_name);
		
		dev = CreateFile(dev_name, 0, 0, NULL, OPEN_EXISTING, 
				 FILE_FLAG_OVERLAPPED, NULL);
		
		if (dev == INVALID_HANDLE_VALUE) {
			continue;
		}
		
		if (!_usb_io_sync(dev, LIBUSB_IOCTL_GET_VERSION,
				  &req, sizeof(libusb_request),
				  &req, sizeof(libusb_request), &ret)  ||
		    (ret < sizeof(libusb_request))) {
			usbi_dbg("usb_os_init: getting driver version failed");
			CloseHandle(dev);
			continue;
		} else  {
			_usb_version.driver.major = req.version.major;
			_usb_version.driver.minor = req.version.minor;
			_usb_version.driver.micro = req.version.micro;
			_usb_version.driver.nano = req.version.nano;

			usbi_dbg("driver version: %d.%d.%d.%d",
				 req.version.major, req.version.minor, 
				 req.version.micro, req.version.nano);
			
			/* set debug level */
			req.timeout = 0;
			req.debug.level = ctx->debug;
			
			if (!_usb_io_sync(dev, LIBUSB_IOCTL_SET_DEBUG_LEVEL, 
					  &req, sizeof(libusb_request), 
					  NULL, 0, NULL)) {
				usbi_dbg("setting debug level failed");
			}

			usbi_dbg("found usb dev=%s", dev_name);
			CloseHandle(dev);
			ret = 0;
			break;
		}
	}
	return ret;
}

static int _usb_io_sync(HANDLE dev, unsigned int code, void *out,
			int out_size, void *in, int in_size, int *ret)
{
	OVERLAPPED ol;
	DWORD _ret;
	
	memset(&ol, 0, sizeof(ol));  
	
	if (ret)
		*ret = 0;
	
	ol.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	
	if (!ol.hEvent)
		return FALSE;
	
	if (!DeviceIoControl(dev, code, out, out_size, in,
			     in_size, NULL, &ol)) {
		if (GetLastError() != ERROR_IO_PENDING) {
			CloseHandle(ol.hEvent);
			return FALSE;
		}
	}
	
	if (GetOverlappedResult(dev, &ol, &_ret, TRUE)) {
		if (ret)
			*ret = (int)_ret;
		CloseHandle(ol.hEvent);
		return TRUE;
	}
	
	CloseHandle(ol.hEvent);
	return FALSE;
}

#if 0
static int _usb_add_virtual_hub(struct usb_bus *bus)
{
	libusb_device *dev;
	
	if (!bus->root_dev) {
		if (!(dev = malloc(sizeof(*dev))))
			return FALSE;
		
		memset(dev, 0, sizeof(*dev));
		strcpy(dev->filename, "virtual-hub");
		dev->bus = bus;
		
		dev->descriptor.bLength = LIBUSB_DT_DEVICE_SIZE;
		dev->descriptor.bDescriptorType = LIBUSB_DT_DEVICE;
		dev->descriptor.bcdUSB = 0x0200;
		dev->descriptor.bDeviceClass = LIBUSB_CLASS_HUB;
		dev->descriptor.bDeviceSubClass = 0;
		dev->descriptor.bDeviceProtocol = 0;
		dev->descriptor.bMaxPacketSize0 = 64;
		dev->descriptor.idVendor = 0;
		dev->descriptor.idProduct = 0;
		dev->descriptor.bcdDevice = 0x100;
		dev->descriptor.iManufacturer = 0;
		dev->descriptor.iProduct = 0;
		dev->descriptor.iSerialNumber = 0;
		dev->descriptor.bNumConfigurations = 0;
		
		bus->root_dev = dev;
	}
	
	return TRUE;
}
#endif

static void clear_transfer_async(struct usbi_transfer *itransfer)
{
	struct libusb_transfer *transfer =
		__USBI_TRANSFER_TO_LIBUSB_TRANSFER(itransfer);
	struct win32_transfer_priv *tpriv = usbi_transfer_get_os_priv(itransfer);
	struct win32_device_handle_priv *hpriv =
		__device_handle_priv(transfer->dev_handle);
	if (tpriv->ol.hEvent) {
		usbi_remove_pollfd(ITRANSFER_CTX(itransfer), (int)tpriv->ol.hEvent);
		CloseHandle(tpriv->ol.hEvent);
		tpriv->ol.hEvent = NULL;
	}
}

static int submit_transfer_async(struct usbi_transfer *itransfer)
{
	uint8_t ep;
	struct libusb_transfer *transfer =
		__USBI_TRANSFER_TO_LIBUSB_TRANSFER(itransfer);
	struct win32_transfer_priv *tpriv = usbi_transfer_get_os_priv(itransfer);
	struct win32_device_handle_priv *hpriv =
		__device_handle_priv(transfer->dev_handle);
	int size = transfer->length-tpriv->submitted;
	
	ep = transfer->endpoint;
	
	if (hpriv->dev_handle == INVALID_HANDLE_VALUE) {
		usbi_dbg("device not open");
		return -EINVAL;
	}
	
	tpriv->requested = size > LIBUSB_MAX_READ_WRITE ? LIBUSB_MAX_READ_WRITE : size;
	tpriv->reap_action = NORMAL;

	tpriv->ol.Offset = 0;
	tpriv->ol.OffsetHigh = 0;
	tpriv->ol.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!tpriv->ol.hEvent) {
		usbi_dbg("creating event failed: win error: %s", 
			 usb_win_error_to_string());
		return -usb_win_error_to_errno();
	}
	
	ResetEvent(tpriv->ol.hEvent);
	
	if (!DeviceIoControl(hpriv->dev_handle,
			     tpriv->control_code,
			     &tpriv->req, sizeof(libusb_request),
			     transfer->buffer+tpriv->submitted,
			     tpriv->requested,
			     NULL, &tpriv->ol)) {
		if (GetLastError() != ERROR_IO_PENDING) {
			tpriv->reap_action = SUBMIT_FAILED;
			usbi_dbg("submit_transfer error: %s", usb_win_error_to_string());
			CloseHandle(tpriv->ol.hEvent);
			tpriv->ol.hEvent = NULL;
			return -usb_win_error_to_errno();
		}
	}
	return usbi_add_pollfd(HANDLE_CTX(transfer->dev_handle),
			       (int)tpriv->ol.hEvent, POLLOUT);
}

static int handle_transfer_completion(struct usbi_transfer *itransfer)
{
	struct libusb_transfer *transfer =
		__USBI_TRANSFER_TO_LIBUSB_TRANSFER(itransfer);
	struct win32_transfer_priv *tpriv = usbi_transfer_get_os_priv(itransfer);
	enum libusb_transfer_status status = LIBUSB_TRANSFER_COMPLETED;

	itransfer->transferred += tpriv->replied;

	if (tpriv->reap_action != NORMAL) {
		/* cancelled, submit_fail, or completed early */
		if (tpriv->reap_action == COMPLETED_EARLY) {
			/* FIXME we could solve this extreme corner case with a memmove
			 * or something */
			usbi_warn(ITRANSFER_CTX(itransfer), "SOME DATA LOST! "
				  "(completed early but remaining urb completed)");
		}

		if (tpriv->submitted == transfer->length) {
			usbi_dbg("CANCEL: last URB handled, reporting");
			if (tpriv->reap_action == CANCELLED) {
				clear_transfer_async(itransfer);
				usbi_handle_transfer_cancellation(itransfer);
				return 0;
			}
			if (tpriv->reap_action != COMPLETED_EARLY)
				status = LIBUSB_TRANSFER_ERROR;
			goto out;
		}
		return 0;
	}

	if (tpriv->submitted == transfer->length) {
		usbi_dbg("last URB in transfer --> complete!");
	} else {
		submit_transfer_async(itransfer);
		return 0;
	}
out:
	clear_transfer_async(itransfer);
	usbi_handle_transfer_completion(itransfer, status);
	return 0;
}

static int reap_for_handle(struct usbi_transfer *itransfer)
{
	struct libusb_transfer *transfer =
		__USBI_TRANSFER_TO_LIBUSB_TRANSFER(itransfer);
	struct win32_transfer_priv *tpriv = usbi_transfer_get_os_priv(itransfer);
	struct win32_device_handle_priv *hpriv =
		__device_handle_priv(transfer->dev_handle);
	int ret;

	if (!GetOverlappedResult(hpriv->dev_handle, &tpriv->ol, &ret, TRUE)) {
		usbi_dbg("usb_reap: reaping request failed, win error: %s", 
			 usb_win_error_to_string());
		return -usb_win_error_to_errno();
	}

	tpriv->submitted += tpriv->requested;
	tpriv->replied = ret;

	return handle_transfer_completion(itransfer);
}

static int op_handle_events(struct libusb_context *ctx,
			    struct pollfd *fds, nfds_t nfds,
			    int num_ready)
{
	int r;
	int i = 0;

	for (i = 0; i < nfds && num_ready > 0; i++) {
		struct pollfd *pollfd = &fds[i];
		struct usbi_transfer *itransfer;
		struct libusb_transfer *transfer;
		struct win32_transfer_priv *tpriv;

		if (!pollfd->revents)
			continue;

		num_ready--;
		list_for_each_entry(struct usbi_transfer,
				    itransfer, &ctx->flying_transfers, list) {
			transfer = __USBI_TRANSFER_TO_LIBUSB_TRANSFER(itransfer);
			tpriv = usbi_transfer_get_os_priv(itransfer);
			if (tpriv->ol.hEvent == (HANDLE)pollfd->fd)
				break;
		}

		if (pollfd->revents & POLLERR) {
			usbi_remove_pollfd(ITRANSFER_CTX(itransfer),
					   (int)tpriv->ol.hEvent);
			usbi_handle_disconnect(transfer->dev_handle);
			continue;
		}

		r = reap_for_handle(itransfer);
		if (r == 1 || r == LIBUSB_ERROR_NO_DEVICE)
			continue;
		else if (r < 0)
			goto out;
	}

	r = 0;
out:
	return r;
}

static int setup_bulk_async(struct usbi_transfer *itransfer,
			    DWORD control_code)
{
	uint8_t ep;
	struct libusb_transfer *transfer =
		__USBI_TRANSFER_TO_LIBUSB_TRANSFER(itransfer);
	struct win32_transfer_priv *tpriv = usbi_transfer_get_os_priv(itransfer);
	struct win32_device_handle_priv *hpriv =
		__device_handle_priv(transfer->dev_handle);
	
	ep = transfer->endpoint;

	if (((control_code == LIBUSB_IOCTL_INTERRUPT_OR_BULK_WRITE) ||
	     (control_code == LIBUSB_IOCTL_ISOCHRONOUS_WRITE)) &&
	    (ep & LIBUSB_ENDPOINT_IN)) {
		usbi_dbg("invalid endpoint 0x%02x", ep);
		return -EINVAL;
	}
	
	if (((control_code == LIBUSB_IOCTL_INTERRUPT_OR_BULK_READ) ||
	     (control_code == LIBUSB_IOCTL_ISOCHRONOUS_READ)) &&
	    !(ep & LIBUSB_ENDPOINT_IN)) {
		usbi_dbg("invalid endpoint 0x%02x", ep);
		return -EINVAL;
	}
	
	tpriv->req.endpoint.endpoint = ep;
	tpriv->req.endpoint.packet_size = transfer->length;
	tpriv->control_code = control_code;
	return 0;
}

static int setup_ctrl_async(struct usbi_transfer *itransfer,
			    int requesttype, int request,
			    int value, int index, int timeout)
{
	int read = 0;
	int code;
	struct libusb_transfer *transfer =
		__USBI_TRANSFER_TO_LIBUSB_TRANSFER(itransfer);
	struct win32_transfer_priv *tpriv = usbi_transfer_get_os_priv(itransfer);
	struct win32_device_handle_priv *hpriv =
		__device_handle_priv(transfer->dev_handle);

	if (hpriv->dev_handle == INVALID_HANDLE_VALUE) {
		usbi_dbg("device not open");
		return -EINVAL;
	}
	
	/* windows doesn't support generic control messages, so it needs to be */
	/* split up */ 
	switch (requesttype & (0x03 << 5)) {
	case LIBUSB_REQUEST_TYPE_STANDARD:      
		switch (request) {
		case LIBUSB_REQUEST_GET_STATUS: 
			tpriv->req.status.recipient = requesttype & 0x1F;
			tpriv->req.status.index = index;
			code = LIBUSB_IOCTL_GET_STATUS;
			break;
			
		case LIBUSB_REQUEST_CLEAR_FEATURE:
			tpriv->req.feature.recipient = requesttype & 0x1F;
			tpriv->req.feature.feature = value;
			tpriv->req.feature.index = index;
			code = LIBUSB_IOCTL_CLEAR_FEATURE;
			break;
			
		case LIBUSB_REQUEST_SET_FEATURE:
			tpriv->req.feature.recipient = requesttype & 0x1F;
			tpriv->req.feature.feature = value;
			tpriv->req.feature.index = index;
			code = LIBUSB_IOCTL_SET_FEATURE;
			break;
			
		case LIBUSB_REQUEST_GET_DESCRIPTOR:
			tpriv->req.descriptor.recipient = requesttype & 0x1F;
			tpriv->req.descriptor.type = (value >> 8) & 0xFF;
			tpriv->req.descriptor.index = value & 0xFF;
			tpriv->req.descriptor.language_id = index;
			code = LIBUSB_IOCTL_GET_DESCRIPTOR;
			break;
			
		case LIBUSB_REQUEST_SET_DESCRIPTOR:
			tpriv->req.descriptor.recipient = requesttype & 0x1F;
			tpriv->req.descriptor.type = (value >> 8) & 0xFF;
			tpriv->req.descriptor.index = value & 0xFF;
			tpriv->req.descriptor.language_id = index;
			code = LIBUSB_IOCTL_SET_DESCRIPTOR;
			break;
			
		case LIBUSB_REQUEST_GET_CONFIGURATION:
			code = LIBUSB_IOCTL_GET_CONFIGURATION;
			break;
			
		case LIBUSB_REQUEST_SET_CONFIGURATION:	  
			tpriv->req.configuration.configuration = value;
			code = LIBUSB_IOCTL_SET_CONFIGURATION;
			break;
			
		case LIBUSB_REQUEST_GET_INTERFACE:
			tpriv->req.Interface.Interface = index;
			code = LIBUSB_IOCTL_GET_INTERFACE;	  
			break;
			
		case LIBUSB_REQUEST_SET_INTERFACE:
			tpriv->req.Interface.Interface = index;
			tpriv->req.Interface.altsetting = value;
			code = LIBUSB_IOCTL_SET_INTERFACE;	  
			break;
			
		default:
			usbi_dbg("invalid request 0x%x", request);
			return -EINVAL;
		}
		break;
	
	case LIBUSB_REQUEST_TYPE_VENDOR:  
	case LIBUSB_REQUEST_TYPE_CLASS:
		
		tpriv->req.vendor.type = (requesttype >> 5) & 0x03;
		tpriv->req.vendor.recipient = requesttype & 0x1F;
		tpriv->req.vendor.request = request;
		tpriv->req.vendor.value = value;
		tpriv->req.vendor.index = index;
		
		if (requesttype & 0x80)
			code = LIBUSB_IOCTL_VENDOR_READ;
		else
			code = LIBUSB_IOCTL_VENDOR_WRITE;
		break;
		
	case LIBUSB_REQUEST_TYPE_RESERVED:
	default:
		usbi_dbg("invalid or unsupported request type: %x", 
			 requesttype);
		return -EINVAL;
	}

	tpriv->control_code = code;
	tpriv->req.timeout = timeout;
	tpriv->submitted = LIBUSB_CONTROL_SETUP_SIZE;
	return 0;
}

static int submit_control_transfer(struct usbi_transfer *itransfer)
{
	int ret;
	struct libusb_transfer *transfer =
		__USBI_TRANSFER_TO_LIBUSB_TRANSFER(itransfer);
	struct libusb_control_setup *ctrl = (struct libusb_control_setup *)transfer->buffer;

	ret = setup_ctrl_async(itransfer, ctrl->bmRequestType,
			       ctrl->bRequest, ctrl->wValue, ctrl->wIndex,
			       transfer->timeout);
	if (ret < 0) return ret;
	return submit_transfer_async(itransfer);
}

static int submit_bulk_transfer(struct usbi_transfer *itransfer)
{
	struct libusb_transfer *transfer =
		__USBI_TRANSFER_TO_LIBUSB_TRANSFER(itransfer);
	uint8_t ep;
	int ret;
	DWORD control_code;

	ep = transfer->endpoint;

	if (ep & LIBUSB_ENDPOINT_IN) {
		control_code = LIBUSB_IOCTL_INTERRUPT_OR_BULK_READ;
	} else {
		control_code = LIBUSB_IOCTL_INTERRUPT_OR_BULK_WRITE;
	}
	ret = setup_bulk_async(itransfer, control_code);
	if (ret < 0) return ret;
	return submit_transfer_async(itransfer);
}

static int op_submit_transfer(struct usbi_transfer *itransfer)
{
	struct libusb_transfer *transfer =
		__USBI_TRANSFER_TO_LIBUSB_TRANSFER(itransfer);

	switch (transfer->type) {
	case LIBUSB_TRANSFER_TYPE_CONTROL:
		return submit_control_transfer(itransfer);
	case LIBUSB_TRANSFER_TYPE_BULK:
	case LIBUSB_TRANSFER_TYPE_INTERRUPT:
		return submit_bulk_transfer(itransfer);
#if 0
	case LIBUSB_TRANSFER_TYPE_ISOCHRONOUS:
		return submit_iso_transfer(itransfer);
#endif
	default:
		usbi_err(TRANSFER_CTX(transfer),
			 "unknown endpoint type %d", transfer->type);
		return LIBUSB_ERROR_INVALID_PARAM;
	}
	return -1;
}

static int op_cancel_transfer(struct usbi_transfer *itransfer)
{
	struct libusb_transfer *transfer =
		__USBI_TRANSFER_TO_LIBUSB_TRANSFER(itransfer);
	struct win32_transfer_priv *tpriv = usbi_transfer_get_os_priv(itransfer);
	struct win32_device_handle_priv *hpriv =
		__device_handle_priv(transfer->dev_handle);
	int ret = 0;
	
	tpriv->reap_action = CANCELLED;
	tpriv->req.endpoint.endpoint = (int)transfer->endpoint;
	tpriv->req.timeout = LIBUSB_DEFAULT_TIMEOUT;

	ResetEvent(tpriv->ol.hEvent);
	
	if (!DeviceIoControl(hpriv->dev_handle,
			     LIBUSB_IOCTL_ABORT_ENDPOINT,
			     &tpriv->req, sizeof(libusb_request),
			     NULL,
			     0,
			     NULL, &tpriv->ol)) {
		if (GetLastError() != ERROR_IO_PENDING) {
			tpriv->reap_action = SUBMIT_FAILED;
			usbi_dbg("cancel_tranfer error: %s", usb_win_error_to_string());
		}
	}

	/* XXX: No Reap on Win32 Cancellation
	 * Reaping urb must not be called on win32, so we completes this
	 * transfer.
	 */
	usbi_handle_transfer_cancellation(itransfer);
	return 0;
}

static void op_clear_transfer_priv(struct usbi_transfer *itransfer)
{
	struct libusb_transfer *transfer =
		__USBI_TRANSFER_TO_LIBUSB_TRANSFER(itransfer);
	struct win32_transfer_priv *tpriv = usbi_transfer_get_os_priv(itransfer);

	clear_transfer_async(itransfer);
}

const struct usbi_os_backend win32_backend = {
	"WIN32 libusb",
	op_init,
	NULL,
	op_get_device_list,

	op_open,
	op_close,

	op_get_device_descriptor,
	op_get_active_config_descriptor,
	op_get_config_descriptor,
	op_get_configuration,
	op_set_configuration,
	op_claim_interface,
	op_release_interface,
	op_set_interface,

	op_clear_halt,
	op_reset_device,

	op_kernel_driver_active,
	op_detach_kernel_driver,
	op_attach_kernel_driver,

	op_destroy_device,

	op_submit_transfer,
	op_cancel_transfer,
	op_clear_transfer_priv,
	op_handle_events,
	op_clear_transfer_priv,

	op_clock_gettime,

	sizeof(struct win32_device_priv),
	sizeof(struct win32_device_handle_priv),
	sizeof(struct win32_transfer_priv),
	0,
};
