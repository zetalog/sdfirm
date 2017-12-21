#ifndef __TSC_DRIVER_H_INCLUDE__
#define __TSC_DRIVER_H_INCLUDE__

#include <target/config.h>

#ifdef CONFIG_ARCH_HAS_TSC
#include <asm/mach/tsc.h>
#endif

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
#elif TSC_MAX <= 0xFFFFFFFFFFFFFFFFULL
#define __tsc_div	div64u
#define __tsc_mul	mul64u
typedef uint64_t tsc_count_t;
#endif

/* XXX:Allow ARCH Specific Definition
 * In some ARCH, a function called tsc_hw_read_counter may lead to extra
 * overheads in delay calibration.  Thus we allow such ARCH defining a
 * macro before tsc_count_t definitions to eliminate such overheads.
 */
#ifndef tsc_hw_read_counter
tsc_count_t tsc_hw_read_counter(void);
#endif
void tsc_hw_ctrl_init(void);

#endif /* __TSC_DRIVER_H_INCLUDE__ */
