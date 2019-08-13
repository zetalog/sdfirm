#ifndef __TSTMR_VEGA_H_INCLUDE__
#define __TSTMR_VEGA_H_INCLUDE__

#include <target/types.h>
#include <asm/io.h>

#define TSTMRA_BASE		UL(0x40034000)
#define TSTMRB_BASE		UL(0x4102C000)
#define TSTMR_REG(id, offset)	((caddr_t)(TSTMR##id##_BASE+(offset)))

#define TSTMR_FREQ		1000000

/* ----------------------------------------------------------------------------
   Time Stamp Timer (TSTMR)
   ---------------------------------------------------------------------------- */

/* L - Time Stamp Timer Register Low */
#define TSTMR_L(id)		TSTMR_REG(id, 0x00)
/* H - Time Stamp Timer Register High */
#define TSTMR_H(id)		TSTMR_REG(id, 0x04)

#define TSTMR_HIGH_OFFSET	0
#define TSTMR_HIGH_MASK		REG_24BIT_MASK
#define TSTMR_HIGH(value)	_GET_FV(TSTMR_HIGH, value)

static inline uint32_t tstmr_read_counter_low(void)
{
	return __raw_readl(TSTMR_L(A));
}
static inline uint32_t tstmr_read_counter_high(void)
{
	uint32_t hi = __raw_readl(TSTMR_H(A));
	return TSTMR_HIGH(hi);
}

/* The TSTMR LOW read should occur first, followed by the TSTMR HIGH read */
static inline uint64_t tstmr_read_counter(void)
{
	uint32_t hi, lo, tmp;

	do {
		hi = tstmr_read_counter_high();
		lo = tstmr_read_counter_low();
		tmp = tstmr_read_counter_high();
	} while (hi != tmp);
	return (uint64_t)hi << 32 | lo;
}

#endif /* __TSTMR_VEGA_H_INCLUDE__ */
