#ifndef __OS_USB_H_INCLUDE__
#define __OS_USB_H_INCLUDE__

#include <wdm/usb_cmn.h>
#include <wdm/wdm_usb.h>
#include <host/usbdev.h>

struct os_usb_driver {
	uint8_t devcls;
	uint8_t caps;
#define USB_CAPS_CAN_REMOVE	0x01	/* device can be surprisely removed */
#define USB_CAPS_CAN_SUSPEND	0x02	/* device can suspend/resume */
#define USB_CAPS_CAN_WAKEUP	0x04	/* device support system wakeup */

	/* device operations */
	status_t (*init)(os_usbif *usb);
	void (*exit)(os_usbif *usb);
	status_t (*start)(os_usbif *usb);
	void (*stop)(os_usbif *usb);
	void (*suspend)(os_usbif *usb);
	void (*resume)(os_usbif *usb);

	/* file operations */
	status_t (*open)(struct os_usb_file *file);
	void (*close)(struct os_usb_file *file);
	status_t (*read)(struct os_usb_file *file,
			 char *buf, size_t *count);
	status_t (*write)(struct os_usb_file *file,
			  const char *buf, size_t *count);
	status_t (*ioctl)(struct os_usb_file *file,
			  unsigned long code, void *arg, int *length);

	/* usb asynchronous pipe callback */
	status_t (*ctrl_aval)(os_usbif *usb, struct os_usb_urb *urb);
	void (*ctrl_cmpl)(os_usbif *usb, struct os_usb_urb *urb);
};

/* XXX: Free URB on Completion
 *
 * If cmpl is set, purb must be set, and os_usb_free_request should be
 * called in the cmpl to free the URB.
 */
status_t os_usb_submit_default_sync(os_device dev,
				    uint8_t request_type,
				    uint8_t request,
				    uint16_t value,
				    uint16_t index,
				    uint16_t length,
				    uint8_t *additional_info);
status_t os_usb_submit_default_aysnc(os_device dev,
				     struct os_usb_urb *urb,
				     uint8_t request_type,
				     uint8_t request,
				     uint16_t value,
				     uint16_t index,
				     uint16_t length,
				     uint8_t *additional_info);
status_t os_usb_configure_device(os_device dev);
void os_usb_deconfigure_device(os_device dev);
struct os_usb_urb *os_usb_alloc_sync(int action);
status_t os_usb_init_urb(struct os_usb_urb *urb, uint32_t size,
			 uint32_t length, const char *what);
void os_usb_free_sync(struct os_usb_urb *urb);
struct os_usb_urb *os_usb_alloc_async(os_device dev, uint8_t eid,
				      int action, int extrasize);
status_t os_usb_submit_async(os_device dev, uint8_t eid,
			     struct os_usb_urb *urb);
status_t os_usb_declare_interface(struct os_usb_driver *intf,
				  size_t objsize);
status_t os_usb_claim_endpoint(os_device dev, uint8_t eid,
			       os_usb_aval_cb aval, os_usb_cmpl_cb cmpl);

void os_usb_suspend_async(os_device dev, uint8_t eid);
void os_usb_resume_async(os_device dev, uint8_t eid);
status_t os_usb_abort_pipe(os_device dev, uint8_t eid);
status_t os_usb_abort_pipes(os_device dev);
struct os_usb_pipe *os_usb_pipe_by_eid(os_device dev, uint8_t eid);

status_t os_usb_submit_urb_sync(os_device dev, uint8_t eid,
				struct os_usb_urb *urb);
void os_usb_submit_urb_async(os_device dev, uint8_t eid,
			     struct os_usb_urb *urb);

status_t os_usb_reset_device(os_device dev);
status_t os_usb_cycle_device(os_device dev);
void os_usb_reset_device_async(os_device dev);
void os_usb_cycle_device_async(os_device dev);
int os_usb_is_composite(os_device dev);
uint8_t os_usb_interface_num(os_device dev);

status_t usb_get_device_desc(os_device dev,
			     struct usb_device_desc **pdesc);
status_t usb_get_config_desc(os_device dev,
			     struct usb_conf_desc **pdesc,
			     uint16_t index);
status_t __usb_get_config_desc(os_device dev,
			       struct usb_conf_desc **pdesc,
			       uint16_t index,
			       uint16_t length);
status_t usb_get_string_desc(os_device dev,
			     struct usb_string_desc **pdesc,
			     uint8_t index);
status_t usb_get_configuration(os_device dev, uint8_t *conf);
status_t usb_set_configuration(os_device dev, uint8_t conf);
status_t usb_set_interface(os_device dev, uint8_t intf, uint16_t alter);
status_t usb_halt_endpoint(os_device dev, uint8_t eid);
status_t usb_unhalt_endpoint(os_device dev, uint8_t eid);

struct usb_intf_desc *usb_match_interface_cls(struct usb_conf_desc *conf_desc,
					      uint8_t devcls, uint8_t subcls, uint8_t proto,
					      uint16_t *length);
struct usb_intf_desc *usb_match_interface_num(struct usb_conf_desc *conf_desc,
					      uint8_t intf_num,
					      uint16_t alt_num,
					      uint16_t *length);
os_device os_usb_device(os_usbif *usb);

#endif /* __OS_USB_H_INCLUDE__ */
