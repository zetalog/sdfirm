#ifndef __SIM_USB_DEV_H_INCLUDE__
#define __SIM_USB_DEV_H_INCLUDE__

#define USBIP_FIFO_STATE_EMPTY		0
#define USBIP_FIFO_STATE_FULL		1
#define USBIP_FIFO_STATE_COUNT		2

#define USBIP_FIFO_EVENT_TXPOLL		0
#define USBIP_FIFO_EVENT_TXAVAL		1
#define USBIP_FIFO_EVENT_TXCMPL		2
#define USBIP_FIFO_EVENT_RXPOLL		3
#define USBIP_FIFO_EVENT_RXAVAL		4
#define USBIP_FIFO_EVENT_RXCMPL		5
#define USBIP_FIFO_EVENT_COUNT		6

void usbip_dev_init(void);
void usbip_dev_exit(void);
void usbip_dev_raise(uint8_t dir, uint8_t eid, int event);

#endif /* __SIM_USB_DEV_H_INCLUDE__ */
