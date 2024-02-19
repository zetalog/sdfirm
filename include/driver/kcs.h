#ifndef __KCS_DRIVER_H_INCLUDE__
#define __KCS_DRIVER_H_INCLUDE__

#ifdef CONFIG_ARCH_HAS_KCS
#include <asm/mach/kcs.h>
#endif

#ifndef ARCH_HAS_KCS
#define kcs_hw_read_cmd()			do { } while (0)
#define kcs_hw_read_dat()			do { } while (0)
#define kcs_hw_write_dat(dat)			do { } while (0)
#define kcs_hw_write_state(state)		do { } while (0)
#define kcs_hw_write_obf(obf)			do { } while (0)
#define kcs_hw_clear_status(status)		do { } while (0)
#endif

#endif /* __KCS_DRIVER_H_INCLUDE__ */