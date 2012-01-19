#ifndef __WDM_HCD_H_INCLUDE__
#define __WDM_HCD_H_INCLUDE__

#include <usbdi.h>
#include <usbdlib.h>
#include <usbbusif.h>

#define OS_DRIVER_FDO_TYPE	FILE_DEVICE_BUS_EXTENDER
#define OS_DRIVER_FDO_CHAR	FILE_DEVICE_SECURE_OPEN
#define OS_DRIVER_BUS_TYPE	GUID_BUS_TYPE_USB
#define OS_DRIVER_PDO_CHAR	FILE_DEVICE_SECURE_OPEN

#define NR_HCD_PORTS		2
#define INVALID_HCD_PORT	NR_HCD_PORTS

struct os_hcd_urb {
	int port_no;
	uint8_t eid;		/* endpoint direction & address */
	uint8_t *buffer;
	size_t buffer_size;	/* allocated buffer size */
	size_t request_size;	/* total URB size */
	size_t urb_handled;	/* handled URB size */
	size_t transfer_size;	/* current data size in the buffer */
	/* i.e., MDL on WDM from URB */
	PMDL *mdl;
};

struct os_hcd_pipe {
	int port_no;
	uint8_t eid;
	uint8_t type;
	size_t size;
};

struct os_hcd_dev {
	os_device dev;	/* USB device PDO */
	struct os_hcd_port_driver *port_driver;

	int port_no;
	os_device hub;	/* USB hub device(os_usb_hci) */

	struct usb_device_desc dev_desc;
	uint8_t dev_addr;
	struct usb_conf_desc *conf_desc;

	/* attached device information */
	uint16_t idVendor;
	uint16_t idProduct;
	uint16_t bcdDevice;

	struct os_hcd_pipe default_pipe;
	struct os_hcd_pipe *pipes;
	int nr_pipes;

	uint8_t priv[1];
};
#define hcd_port_dev_priv(port)	((void *)((port)->priv))

struct os_hcd_port {
	os_device hub;	/* hub device */
	int port_no;
	uint8_t state;
	uint8_t resumed_state;
	struct os_work_queue urb_worker;
	os_device usb_device;
};

typedef struct os_usb_hci {
	os_device dev;
	struct os_hcd_driver *hcd_driver;

	uint8_t state;
	uint8_t resumed_state;

	/* flag indicating whether a port has its device attached */
	reflck_t inuse[NR_HCD_PORTS];
	struct os_hcd_port ports[NR_HCD_PORTS];

	uint8_t priv[1];
} os_usbhc;
#define hcd_dev_priv(hcd)	((void *)((hcd)->priv))

struct os_hcd_file {
	os_file *file;
	os_usbhc *usb;
	int port_no;
	void *priv_data;
};
#define hcd_file_priv(file)	((void *)((file)->priv_data))

DEFINE_GUID(GUID_DEVCLASS_USB_PORT, 
	    0x9d3039dd, 0xcca5, 0x4b4d,
	    0xb3, 0x3d, 0xe2, 0xdd, 0xc8, 0xa8, 0xc5, 0x2e);

NTSTATUS wdm_hcd_dispatch_ioctl(PDEVICE_OBJECT dev, PIRP irp);
NTSTATUS wdm_hcd_query_interface(PDEVICE_OBJECT dev, USHORT size,
				 USHORT version, PINTERFACE *iface);

os_device os_hcd_device(os_usbhc *hcd);
void os_hcd_init_ports(os_usbhc *hcd);
void os_hcd_exit_ports(os_usbhc *hcd);

#endif /* __WDM_HCD_H_INCLUDE__ */
