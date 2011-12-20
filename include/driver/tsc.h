#ifndef __TSC_DRIVER_H_INCLUDE__
#define __TSC_DRIVER_H_INCLUDE__

#include <target/config.h>

#if TSC_MAX <= 0xFF
typedef uint8_t tsc_count_t;
#define __tsc_div(a, b)	(a/b)
#define __tsc_mul(a, b)	(a*b)
#elif TSC_MAX <= 0xFFFF
typedef uint16_t tsc_count_t;
#define __tsc_div	div16u
#define __tsc_mul	mul16u
#elif TSC_MAX <= 0xFFFFFFFFUL
typedef uint32_t tsc_count_t;
#define __tsc_div	div32u
#define __tsc_mul	mul32u
#endif

#ifdef CONFIG_ARCH_HAS_TSC
#include <asm/mach/tsc.h>
#endif

void tsc_hw_ctrl_init(void);

#endif /* __TSC_DRIVER_H_INCLUDE__ */
