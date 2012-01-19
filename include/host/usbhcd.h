#ifndef __USBHCD_H_INCLUDE__
#define __USBHCD_H_INCLUDE__

#include <host/wdm.h>

#define HCD_IOCTL_INDEX		0x0002
#define HCDIOPORTINFO		(USBD_IOCTL_INDEX+0)

#define HCD_IOCTL_CODE(code, dir, method)			\
	IOCTL_CODE(code, dir, method)
#define IOCTL_HCD_PORTINFO					\
	HCD_IOCTL_CODE(HCDIOPORTINFO,				\
		       IOCTL_OUTPUT, METHOD_BUFFERED)

#endif /* __USBHCD_H_INCLUDE__ */
