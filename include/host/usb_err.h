#ifndef __USB_ERR_H_INCLUDE__
#define __USB_ERR_H_INCLUDE__

/* Connection timed out */
#define ETIMEDOUT 116	

typedef enum {
	USB_ERROR_TYPE_NONE = 0,
	USB_ERROR_TYPE_STRING,
	USB_ERROR_TYPE_ERRNO,
} usb_error_type_t;

void usb_error(char *format, ...);
void usb_message(char *format, ...);
const char *usb_win_error_to_string(void);
int usb_win_error_to_errno(void);

#endif /* __USB_ERR_H_INCLUDE__ */
