#ifndef __OS_HCD_H_INCLUDE__
#define __OS_HCD_H_INCLUDE__

#include <wdm/usb_cmn.h>
#include <wdm/wdm_hcd.h>
#include <host/usbhcd.h>

struct os_hcd_driver {
	uint8_t caps;
#define HCD_CAPS_CAN_SUSPEND	0x01	/* HCD can suspend/resume */
#define HCD_CAPS_CAN_WAKEUP	0x02	/* HCD support system wakeup */

	/* device operations */
	status_t (*init)(os_usbhc *usb);
	void (*exit)(os_usbhc *usb);
	status_t (*start)(os_usbhc *usb);
	void (*stop)(os_usbhc *usb);
	void (*suspend)(os_usbhc *usb);
	void (*resume)(os_usbhc *usb);

	/* file operations */
	status_t (*open)(struct os_hcd_file *file);
	void (*close)(struct os_hcd_file *file);
	status_t (*read)(struct os_hcd_file *file,
			 char *buf, size_t *count);
	status_t (*write)(struct os_hcd_file *file,
			  const char *buf, size_t *count);
	status_t (*ioctl)(struct os_hcd_file *file,
			  unsigned long code, void *arg, int *length);
};

struct os_hcd_port_driver {
	/* device operations */
	status_t (*init)(struct os_hcd_dev *udev);
	void (*exit)(struct os_hcd_dev *udev);
	status_t (*start)(struct os_hcd_dev *udev);
	void (*stop)(struct os_hcd_dev *udev);
	void (*suspend)(struct os_hcd_dev *udev);
	void (*resume)(struct os_hcd_dev *udev);
};

status_t os_usb_declare_controller(struct os_hcd_driver *uhcd,
				   size_t objsize);

void os_hcd_port_attached(os_device hub, int port_no);
void os_hcd_port_detached(os_device hub, int port_no);

status_t os_hcd_start_port(os_device hub, int port_no,
			   struct os_hcd_port_driver *uport,
			   size_t objsize,
			   uint16_t vid, uint16_t pid, uint16_t sn);
void os_hcd_stop_port(os_device hub, int port_no);

#endif /* __OS_HCD_H_INCLUDE__ */
