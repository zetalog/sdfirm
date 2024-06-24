#ifndef __DRIVER_SUPERIO_H_INCLUDE__
#define __DRIVER_SUPERIO_H_INCLUDE__

#ifdef CONFIG_ARCH_HAS_SIO
#include <asm/mach/sio.h>
#endif

#ifndef ARCH_HAVE_SIO
#define SIO_HW_ENTER		0x87
#define SIO_HW_EXIT		0xAA
#endif

#endif /* __DRIVER_SUPERIO_H_INCLUDE__ */
