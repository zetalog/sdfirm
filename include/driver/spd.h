#ifndef __SPD_DRIVER_H_INCLUDE__
#define __SPD_DRIVER_H_INCLUDE__

#ifdef CONFIG_ARCH_HAS_DDR_SPD
#include <asm/mach/spd.h>
#endif /* CONFIG_ARCH_HAS_DDR_SPD */

#ifndef ARCH_HAVE_SPD
#define spd_hw_ctrl_init()
#endif

#endif /* __SPD_DRIVER_H_INCLUDE__ */
