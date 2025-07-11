#ifndef __MBOX_DRIVER_H_INCLUDE__
#define __MBOX_DRIVER_H_INCLUDE__

#ifdef CONFIG_ARCH_HAS_MBOX
#include <asm/mach/mbox.h>
#endif

#ifndef ARCH_HAVE_MBOX
#define mbox_hw_ctrl_init()
#define mbox_hw_select_box(bid)
#endif

#endif /* __MBOX_DRIVER_H_INCLUDE__ */

