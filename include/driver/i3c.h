#ifndef __I3C_DRIVER_H_INCLUDE__
#define __I3C_DRIVER_H_INCLUDE__

typedef uint16_t i3c_addr_t;
typedef uint16_t i3c_len_t;

typedef void (*i3c_io_cb)(i3c_len_t len);

#ifdef CONFIG_ARCH_HAS_I3C
#include <asm/mach/i3c.h>
#endif

#ifndef ARCH_HAVE_I3C
#endif

#endif /* __I3C_DRIVER_H_INCLUDE__ */
