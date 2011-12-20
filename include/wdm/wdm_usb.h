#ifndef __WDM_USB_H_INCLUDE__
#define __WDM_USB_H_INCLUDE__

#include <usbdi.h>
#include <usbdlib.h>

struct os_usb_urb;

typedef status_t (*os_usb_aval_cb)(os_device dev, uint8_t eid,
				   struct os_usb_urb *urb);
typedef void (*os_usb_cmpl_cb)(os_device dev, uint8_t eid,
			       struct os_usb_urb *urb);
typedef VOID (USB_IDLE_NOTIFY_CALLBACK)(PVOID context);

struct os_usb_urb {
	int action;
#define OS_USB_SUBMIT_URB		0x01
#define OS_USB_RESET_PORT		0x02
#define OS_USB_CYCLE_PORT		0x03
#define OS_USB_ABORT_PIPE		0x04

	struct os_work_item *work;
	const char *hint;
	PURB urb;
	/* sizeof (URB) */
	size_t size;
	/* urb content size */
	uint32_t length;
	/* request status */
	status_t status;

	os_waiter waiter;
	IO_STATUS_BLOCK iosb;

	uint8_t priv[1];
};
#define urb_priv(urb)		((void *)(urb)->priv)

struct os_usb_pipe {
	uint8_t eid;				/* endpoint address */
	unsigned long flags;
#define USB_PIPE_MASK		0x01		/* URB scheduling isn't called */
#define USB_PIPE_HALT		0x02		/* HALT feature is set */
	os_usb_aval_cb aval;
	os_usb_cmpl_cb cmpl;
	/* work queue of URB serialization */
	struct os_work_queue urb_worker;

	/* WDM pipe information related */
	int pipe_id;
	USBD_PIPE_INFORMATION *pipe_info;
	/* WDM URB submisson related */
	os_waiter waiter;
	IO_STATUS_BLOCK iosb;
};

struct os_usb_interface {
	os_device dev;
	struct os_usb_driver *usb_driver;

	uint8_t state;
	uint8_t resumed_state;

	BOOLEAN is_composite;
	struct usb_device_desc *usb_device_desc;
	USBD_CONFIGURATION_HANDLE config_handle;
	struct usb_conf_desc *usb_config_desc;
	struct usb_intf_desc *usb_interface_desc;
	uint16_t usb_interface_desc_length;
	PUSBD_INTERFACE_INFORMATION usb_interface;

	/* endpoints information */
	struct os_usb_pipe *usb_endpoints;
	int nr_usb_endpoints;
	struct os_usb_pipe usb_ctrl_pipe;

	/* Selective suspend feature */
	LONG idle_enabled;
	LONG idle_registered;
	PUSB_IDLE_CALLBACK_INFO idle_info;
	os_waiter idle_waiter;
	os_timer *idle_timer;
	struct os_work_queue idle_worker;
	refcnt_t ref_users;

	uint8_t priv[1];
};
typedef struct os_usb_interface os_usbif;

struct os_usb_file {
	os_file *file;
	os_usbif *usb;
	/* file are always created on pipe basis */
	struct os_usb_pipe *pipe;
	void *priv_data;
};

#define usb_dev_priv(usb)	((void *)((usb)->priv))
#define usb_file_priv(file)	((void *)((file)->priv_data))

void wdm_usb_wait_enumeration(os_usbif *usb);

void wdm_usb_start_pipe(os_device dev, int id, uint8_t eid,
			USBD_PIPE_INFORMATION *info);
void wdm_usb_stop_pipe(os_device dev, int id);

/* init/exit Non-control endpoints */
status_t wdm_usb_start_pipes(os_device dev);
void wdm_usb_stop_pipes(os_device dev);

void wdm_usb_submit_idle(os_device dev);
void wdm_usb_suspend_idle(os_device dev);
void wdm_usb_resume_idle(os_device dev);
void wdm_usb_user_idle(os_device dev);
void wdm_usb_user_unidle(os_device dev);

#endif /* __WDM_USB_H_INCLUDE__ */
