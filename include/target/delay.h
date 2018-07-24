#ifndef __DELAY_H_INCLUDE__
#define __DELAY_H_INCLUDE__

/* XXX: Inclusion Rule of "delay.h"
 *
 * Only drivers might use this function to ensure the correctness of the
 * hardware timing.  Codes other than the drivers should not include this
 * header file.
 */

#include <target/config.h>
#include <target/tsc.h>

#ifdef CONFIG_LPS_64BITS
typedef uint64_t lps_t;
#define __lps_div	div64u
#define __lps_mul	mul64u
#elif defined(CONFIG_LPS_32BITS)
typedef uint32_t lps_t;
#define __lps_div	div32u
#define __lps_mul	mul32u
#else
typedef uint16_t lps_t;
#define __lps_div	div16u
#define __lps_mul	mul16u
#endif

/* XXX: Function Called Per nLoops
 *
 * This function should be extremely fast.  So it leaves the ARCH port
 * rather than the compiler to determine how this function should look
 * like.
 */
#include <asm/delay.h>
#ifndef ARCH_HAVE_DELAY
#define __delay(loops)					\
	do {						\
		volatile lps_t tmp = loops;		\
		while (tmp--);				\
	} while (0)
#endif

void delay(uint8_t s);
void mdelay(uint8_t ms);
void udelay(uint8_t us);

#ifdef CONFIG_LPS_PRESET
#define loops_per_ms		((lps_t)CONFIG_LPS_PRESET_MS)
#define loops_per_us		((lps_t)(CONFIG_LPS_PRESET_MS/1000))
#else
extern lps_t loops_per_ms;
extern lps_t loops_per_us;
#endif

#endif /* __DELAY_H_INCLUDE__ */
