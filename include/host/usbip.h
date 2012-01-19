/*
 * Copyright (C) 2005-2007 Takahiro Hirofuchi
 */

#ifndef __USBIP_H_INCLUDE__
#define __USBIP_H_INCLUDE__

#include <host/types.h>
#include <host/errno.h>
#include <simul/usb_dev.h>

#include <pshpack4.h>

#ifdef __GNUC__
#define __attribute__(x) __attribute__(x)
#else
#define __attribute__(x)
#endif

#define USBIP_VERSION 0x000106

/* Common header for all the kinds of PDUs. */
struct op_common {
	uint16_t version;

#define OP_REQUEST	(0x80 << 8)
#define OP_REPLY	(0x00 << 8)
	uint16_t code;

	/* add more error code */
#define ST_OK	0x00
#define ST_NA	0x01
	uint32_t status; /* op_code status (for reply) */
} __attribute__((packed));

/* Dummy Code */
#define OP_UNSPEC	0x00
#define OP_REQ_UNSPEC	OP_UNSPEC
#define OP_REP_UNSPEC	OP_UNSPEC

/* Retrieve USB device information. (still not used) */
#define OP_DEVINFO	0x02
#define OP_REQ_DEVINFO	(OP_REQUEST | OP_DEVINFO)
#define OP_REP_DEVINFO	(OP_REPLY   | OP_DEVINFO)

#define USBIP_BUS_ID_SIZE 32
#define USBIP_DEV_PATH_MAX 256

struct op_devinfo_request {
	char busid[USBIP_BUS_ID_SIZE];
} __attribute__((packed));

enum usb_device_speed {
	USB_SPEED_UNKNOWN = 0,		/* enumerating */
	USB_SPEED_LOW,
	USB_SPEED_FULL,			/* usb 1.1 */
	USB_SPEED_HIGH,			/* usb 2.0 */
	USB_SPEED_VARIABLE		/* wireless (usb 2.5) */
};

struct usb_device {
	char path[USBIP_DEV_PATH_MAX];
	char busid[USBIP_BUS_ID_SIZE];
	uint32_t busnum;
	uint32_t devnum;
	uint32_t speed;

	uint16_t idVendor;
	uint16_t idProduct;
	uint16_t bcdDevice;

	uint8_t bDeviceClass;
	uint8_t bDeviceSubClass;
	uint8_t bDeviceProtocol;
	uint8_t bConfigurationValue;
	uint8_t bNumConfigurations;
	uint8_t bNumInterfaces;
} __attribute__((packed));

struct usb_interface {
	uint8_t bInterfaceClass;
	uint8_t bInterfaceSubClass;
	uint8_t bInterfaceProtocol;
	uint8_t padding;	/* alignment */
} __attribute__((packed));

struct op_devinfo_reply {
	struct usb_device udev;
	struct usb_interface uinf[];
} __attribute__((packed));

/* Import a remote USB device. */
#define OP_IMPORT	0x03
#define OP_REQ_IMPORT	(OP_REQUEST | OP_IMPORT)
#define OP_REP_IMPORT   (OP_REPLY   | OP_IMPORT)

struct op_import_request {
	char busid[USBIP_BUS_ID_SIZE];
} __attribute__((packed));

struct op_import_reply {
	struct usb_device udev;
//	struct usb_interface uinf[];
} __attribute__((packed));

/* Export a USB device to a remote host. */
#define OP_EXPORT	0x06
#define OP_REQ_EXPORT	(OP_REQUEST | OP_EXPORT)
#define OP_REP_EXPORT	(OP_REPLY   | OP_EXPORT)

struct op_export_request {
	struct usb_device udev;
} __attribute__((packed));

struct op_export_reply {
	int returncode;
} __attribute__((packed));

/* un-Export a USB device from a remote host. */
#define OP_UNEXPORT	0x07
#define OP_REQ_UNEXPORT	(OP_REQUEST | OP_UNEXPORT)
#define OP_REP_UNEXPORT	(OP_REPLY   | OP_UNEXPORT)

struct op_unexport_request {
	struct usb_device udev;
} __attribute__((packed));

struct op_unexport_reply {
	int returncode;
} __attribute__((packed));

/* Negotiate IPSec encryption key. (still not used) */
#define OP_CRYPKEY	0x04
#define OP_REQ_CRYPKEY	(OP_REQUEST | OP_CRYPKEY)
#define OP_REP_CRYPKEY	(OP_REPLY   | OP_CRYPKEY)

struct op_crypkey_request {
	/* 128bit key */
	uint32_t key[4];
} __attribute__((packed));

struct op_crypkey_reply {
	uint32_t _reserved;
} __attribute__((packed));

/* Retrieve the list of exported USB devices. */
#define OP_DEVLIST	0x05
#define OP_REQ_DEVLIST	(OP_REQUEST | OP_DEVLIST)
#define OP_REP_DEVLIST	(OP_REPLY   | OP_DEVLIST)

struct op_devlist_request {
	uint32_t _reserved;
} __attribute__((packed));

struct op_devlist_reply {
	uint32_t ndev;
	/* followed by reply_extra[] */
} __attribute__((packed));

struct op_devlist_reply_extra {
	struct usb_device    udev;
	struct usb_interface uinf[];
} __attribute__((packed));

/* A basic header followed by other additional headers */
struct usbip_header_basic {
#define USBIP_CMD_SUBMIT	0x0001
#define USBIP_CMD_UNLINK	0x0002
#define USBIP_RET_SUBMIT	0x0003
#define USBIP_RET_UNLINK	0x0004
#define USBIP_RESET_DEV		0xFFFF
	uint32_t command;

	 /* sequencial number which identifies requests.
	  * incremented per connections
	  */
	uint32_t seqnum;

	/* devid is used to specify a remote USB device uniquely instead
	 * of busnum and devnum in Linux. In the case of Linux
	 * stub_driver, this value is ((busnum << 16) | devnum)
	 */
	uint32_t devid;  

#define USBIP_DIR_OUT	0
#define USBIP_DIR_IN 	1
	uint32_t direction;
	uint32_t ep;     /* endpoint number */
} __attribute__((packed));

/* CMD_SUBMIT header */
struct usbip_header_cmd_submit {
	/* these values are basically the same as in a URB. */
	/* the same in a URB. */
	uint32_t transfer_flags;
#define USBDEVFS_URB_SHORT_NOT_OK	0x01
#define USBDEVFS_URB_ISO_ASAP		0x02
#define USBDEVFS_URB_NO_FSBR		0x20
#define USBDEVFS_URB_ZERO_PACKET	0x40
#define USBDEVFS_URB_NO_INTERRUPT	0x80

	/* set the following data size (out), or expected reading data
	 * size (in)
	 */
	uint32_t transfer_buffer_length;
	/* it is difficult for usbip to sync frames (reserved only?) */
	uint32_t start_frame;
	/* the number of iso descriptors that follows this header */
	uint32_t number_of_packets;
	/* the maximum time within which this request works in a host
	 * controller of a server side */
	uint32_t interval;
	/* set setup packet data for a CTRL request */
	uint8_t setup[8];
} __attribute__((packed));

/* RET_SUBMIT header */
struct usbip_header_ret_submit {
	uint32_t status;
	uint32_t actual_length; /* returned data length */
	uint32_t start_frame; /* ISO and INT */
	uint32_t number_of_packets;  /* ISO only */
	uint32_t error_count; /* ISO only */
} __attribute__((packed));

/* CMD_UNLINK header */
struct usbip_header_cmd_unlink {
	uint32_t seqnum; /* URB's seqnum which will be unlinked */
} __attribute__((packed));

/* RET_UNLINK header */
struct usbip_header_ret_unlink {
	uint32_t status;
} __attribute__((packed));

/* the same as usb_iso_packet_descriptor but packed for pdu */
struct usbip_iso_packet_descriptor {
	uint32_t offset;
	uint32_t length;            /* expected length */
	uint32_t actual_length;
	uint32_t status;
} __attribute__((packed));

struct usbip_header {
	struct usbip_header_basic base;
	union {
		struct usbip_header_cmd_submit	cmd_submit;
		struct usbip_header_ret_submit	ret_submit;
		struct usbip_header_cmd_unlink	cmd_unlink;
		struct usbip_header_ret_unlink	ret_unlink;
	} u;
} __attribute__((packed));

struct usb_ctrl_setup {
    uint8_t bRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} __attribute__((packed));

#include <poppack.h>

#define USBIP_PORT 3240

#ifdef WIN32

DEFINE_GUID(GUID_DEVINTERFACE_USB_HOST_CONTROLLER_IHCI,
        0xD35F7840, 0x6A0C, 0x11d2, 0xB8, 0x41, 0x00, 0xC0, 0x4F, 0xAD, 0x51, 0x71);
DEFINE_GUID(USBIP_BUS_WMI_STD_DATA_GUID, 
        0x0006A660, 0x8F12, 0x11d2, 0xB8, 0x54, 0x00, 0xC0, 0x4F, 0xAD, 0x51, 0x71);
DEFINE_GUID (USBIP_WMI_STD_DATA_GUID, 
        0xBBA21300L, 0x6DD3, 0x11d2, 0xB8, 0x44, 0x00, 0xC0, 0x4F, 0xAD, 0x51, 0x71);
DEFINE_GUID (USBIP_NOTIFY_DEVICE_ARRIVAL_EVENT, 
        0x1cdaff1, 0xc901, 0x45b4, 0xb3, 0x59, 0xb5, 0x54, 0x27, 0x25, 0xe2, 0x9c);

#define USBVBUS_IOCTL(_index_) \
    CTL_CODE (FILE_DEVICE_BUS_EXTENDER, _index_, METHOD_BUFFERED, FILE_READ_DATA)

#define IOCTL_USBVBUS_PLUGIN_HARDWARE               USBVBUS_IOCTL (0x0)
#define IOCTL_USBVBUS_UNPLUG_HARDWARE               USBVBUS_IOCTL (0x1)
#define IOCTL_USBVBUS_EJECT_HARDWARE                USBVBUS_IOCTL (0x2)
#define IOCTL_USBVBUS_GET_PORTS_STATUS              USBVBUS_IOCTL (0x3)

typedef struct _ioctl_usbvbus_plugin {
	unsigned int devid;
	/* 4 bytes */
	unsigned short vendor;
	unsigned short product;
	/* 8 bytes */
	unsigned short version;
	unsigned char speed;
	unsigned char inum;
	/* 12 bytes */
	unsigned char int0_class;
	unsigned char int0_subclass;
	unsigned char int0_protocol;
	signed char addr;  /* then it can not be bigger then 127 */
	/* 16 bytes */
} ioctl_usbvbus_plugin;

typedef struct _ioctl_usbvbus_get_ports_status {
	union {
		signed char max_used_port; /* then it can not be bigger than 127 */
		unsigned char port_status[128];
		/* 128 bytes */
	};
} ioctl_usbvbus_get_ports_status;

typedef struct _ioctl_usbvbus_unplug {
	signed char addr;
	char unused[3];

} ioctl_usbvbus_unplug;

typedef struct _BUSENUM_EJECT_HARDWARE {
	/* sizeof (struct _EJECT_HARDWARE) */
	ULONG Size;                                    
	/* serial number of the device to be ejected */
	ULONG   SerialNo;
	ULONG Reserved[2];    
} BUSENUM_EJECT_HARDWARE, *PBUSENUM_EJECT_HARDWARE;
#endif

#endif /* __USBIP_H_INCLUDE__ */
