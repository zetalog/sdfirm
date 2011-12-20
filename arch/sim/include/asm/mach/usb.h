#ifndef __USB_MACH_H_INCLUDE__
#define __USB_MACH_H_INCLUDE__

#ifndef ARCH_HAVE_USB
#define ARCH_HAVE_USB		1
#else
#error "Multiple USB controller defined"
#endif

#include <target/config.h>

typedef uint8_t utb_size_t;
#define utb_text_size_t text_byte_t

#define NR_USBD_HW_ENDPS	16
#define USBD_HW_CTRL_SIZE	64

void usb_hw_ctrl_init(void);

void usbd_hw_ctrl_init(void);
void usbd_hw_ctrl_start(void);
void usbd_hw_ctrl_stop(void);

void usbd_hw_bus_attach(void);
void usbd_hw_bus_detach(void);

boolean usbd_hw_endp_caps(uint8_t addr, uint8_t type);
utb_size_t usbd_hw_endp_size(uint8_t addr);

void usbd_hw_endp_enable(void);
void usbd_hw_endp_unhalt(void);
void usbd_hw_endp_halt(void);
void usbd_hw_endp_select(uint8_t addr);

uint8_t usbd_hw_read_byte(void);
void usbd_hw_write_byte(uint8_t byte);

void usbd_hw_transfer_open(void);
void usbd_hw_transfer_close(void);

void usbd_hw_set_address(void);
void usbd_hw_set_config(void);

void usbd_hw_request_open(void);
void usbd_hw_request_close(void);
void usbd_hw_request_reset(void);

#endif /* __USB_MACH_H_INCLUDE__ */
