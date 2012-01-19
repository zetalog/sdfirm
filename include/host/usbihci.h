#ifndef __USBIHCI_H_INCLUDE__
#define __USBIHCI_H_INCLUDE__

#include <host/usbhcd.h>
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
#define USBIP_PORT 3240

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

// {5C9A9A36-96BC-4b2b-B6AB-27AED71EEFD0}
DEFINE_GUID(GUID_DEVINTERFACE_USB_HOST_CONTROLLER_IHCI, 
	0x5c9a9a36, 0x96bc, 0x4b2b, 0xb6, 0xab, 0x27, 0xae, 0xd7, 0x1e, 0xef, 0xd0);
// {7520DAB7-E04E-4a13-8DCB-FFB2182AE970}
DEFINE_GUID(USBIP_BUS_WMI_STD_DATA_GUID, 
	0x7520dab7, 0xe04e, 0x4a13, 0x8d, 0xcb, 0xff, 0xb2, 0x18, 0x2a, 0xe9, 0x70);
// {DEA794F4-5718-43aa-A11E-3CA1DE558145}
DEFINE_GUID(USBIP_WMI_STD_DATA_GUID, 
	0xdea794f4, 0x5718, 0x43aa, 0xa1, 0x1e, 0x3c, 0xa1, 0xde, 0x55, 0x81, 0x45);
// {57F4488F-E33A-484c-8B45-6A8EBEA365B3}
DEFINE_GUID(USBIP_NOTIFY_DEVICE_ARRIVAL_EVENT, 
	0x57f4488f, 0xe33a, 0x484c, 0x8b, 0x45, 0x6a, 0x8e, 0xbe, 0xa3, 0x65, 0xb3);

#define IHCI_SYNC_PORTS		0
#define IHCI_ATTACH_PORT	1
#define IHCI_DETACH_PORT	2

#define USB_IHCI_IOCTL_INDEX	0x0021
#define USBIHCIIOSYNC_PORTS	(USB_IHCI_IOCTL_INDEX+IHCI_SYNC_PORTS)
#define USBIHCIIOATTACH_PORT	(USB_IHCI_IOCTL_INDEX+IHCI_ATTACH_PORT)
#define USBIHCIIODETACH_PORT	(USB_IHCI_IOCTL_INDEX+IHCI_DETACH_PORT)

#define USB_DFU_IOCTL_CODE(code, dir, method)			\
	IOCTL_CODE(code, dir, method)

#define IOCTL_USB_IHCI_SYNC_PORTS				\
	USB_DFU_IOCTL_CODE(USBIHCIIOSYNC_PORTS,			\
			   IOCTL_OUTPUT, METHOD_BUFFERED)
#define IOCTL_USB_IHCI_ATTACH_PORT				\
	USB_DFU_IOCTL_CODE(USBIHCIIOATTACH_PORT,		\
			   IOCTL_INPUT, METHOD_BUFFERED)
#define IOCTL_USB_IHCI_DETACH_PORT				\
	USB_DFU_IOCTL_CODE(USBIHCIIODETACH_PORT,		\
			   IOCTL_INPUT, METHOD_BUFFERED)

#endif /* __USBIHCI_H_INCLUDE__ */
