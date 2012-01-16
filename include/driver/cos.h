#ifndef __COS_DRIVER_H_INCLUDE__
#define __COS_DRIVER_H_INCLUDE__

#include <target/config.h>

#ifdef CONFIG_COS_SCOS
#include <driver/scos.h>
#endif

#ifndef ARCH_HAVE_ICC
#define cos_vs_activate()			SCS_ERR_SUCCESS
#define cos_vs_deactivate()
#define cos_vs_init()
#endif

#endif /* __COS_DRIVER_H_INCLUDE__ */
