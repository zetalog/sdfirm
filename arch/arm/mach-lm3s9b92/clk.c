#include <target/generic.h>
#include <target/arch.h>
#include <target/delay.h>

#define __clk_hw_is_oscdis(cfg, IM)	(cfg & _BV(IM##OSCDIS))
#define __clk_hw_is_usercc2(cfg)	(cfg & _BV(USERCC2))	

void clk_hw_set_config(uint32_t cfg)
{
	uint32_t loop, rcc, rcc2;
	
	/* no writes to RCC2 is allowed for a Sandstorm-class device */
	if ((cpu_hw_device_class() == DEVICE_CLASS_SANDSTORM) &&
	    (cfg & _BV(USERCC2)))
		return;

	/* backup the current value of the RCC and RCC2 */
	rcc = __raw_readl(RCC);
	rcc2 = __raw_readl(RCC2);

	/* bypass the PLL and system clock dividers for now */
	rcc |= _BV(BYPASS);
	rcc &= ~(_BV(USESYSDIV));
	rcc2 |= _BV(BYPASS2);
	__raw_writel(rcc, RCC);
	__raw_writel(rcc2, RCC2);
	
	if ((__clk_hw_is_oscdis(rcc, I) && !__clk_hw_is_oscdis(cfg, I)) ||
	    (__clk_hw_is_oscdis(rcc, M) && !__clk_hw_is_oscdis(cfg, M))) {
		/* the previously enabled oscillators must be enabled
		 * along with the newly requested oscillators
		 */
		rcc &= (~(_BV(IOSCDIS) | _BV(MOSCDIS)) |
		        (cfg & (_BV(IOSCDIS) | _BV(MOSCDIS))));
		__raw_writel(rcc, RCC);
		
		/* wait for a bit until the oscillator time to stabilize
		 * iterations is adjusted based on the current clock source
		 * this is required for slow clock sources
		 */
		if ((__clk_hw_is_usercc2(rcc2) &&
		     (((rcc2 & OSCSRC2_MASK) == OSCSRC_IOSC_30K) ||
		      ((rcc2 & OSCSRC2_MASK) == OSCSRC_EOSC_32K))) ||
		    (!__clk_hw_is_usercc2(rcc2) &&
		     ((rcc & OSCSRC_MASK) == OSCSRC_IOSC_30K))) {
			/* reasonable 4096 iterations */
			__delay(4096);
		} else {
			/* reasonable 16*32768 iterations */
			for (loop = 0; loop < 16; loop++)
				__delay(32768);
		}
	}
	
	/* update the new XTAL and OSC configuration */
	rcc &= ~(XTAL_MASK | OSCSRC_MASK | _BV(PWRDN) | _BV(OEN));
	rcc |= cfg & (XTAL_MASK | OSCSRC_MASK | _BV(PWRDN) | _BV(OEN));

	/* update the RCC2 OSC configuration */
	rcc2 &= ~(_BV(USERCC2) | OSCSRC2_MASK | _BV(PWRDN2));
	rcc2 |= cfg & (_BV(USERCC2) | OSCSRC2_MASK | _BV(PWRDN2));
	/* since the OSCSRC2 field in RCC2 overlaps the XTAL field in RCC, the
	 * OSCSRC field has a special encoding within cfg to avoid the overlap
	 */
	rcc2 |= (cfg & 0x00000008) << 3;
	
	/* clear the PLL lock interrupt */
	__raw_writel(_BV(PLLL), MISC);

	if (rcc2 & _BV(USERCC2)) {
		__raw_writel(rcc2, RCC2);
		__raw_writel(rcc, RCC);
	} else {
		__raw_writel(rcc, RCC);
		__raw_writel(rcc2, RCC2);
	}
	
	/* wait for a bit so that new crystal value and oscillator source
	 * can take effect
	 */
	__delay(16);
	
	/* update system divider and new OSC configuration */
	rcc &= ~(SYSDIV_MASK | _BV(USESYSDIV) | _BV(IOSCDIS) | _BV(MOSCDIS));
	rcc |= cfg & (SYSDIV_MASK | _BV(USESYSDIV) | _BV(IOSCDIS) | _BV(MOSCDIS));

	rcc2 &= ~(SYSDIV2_MASK);
	rcc2 |= cfg & SYSDIV2_MASK;
	if (cfg & _BV(DIV400)) {
		rcc |= _BV(USESYSDIV);
		rcc2 &= ~(_BV(USESYSDIV));
		rcc2 |= cfg & (_BV(DIV400) | _BV(SYSDIV2LSB));
	} else {
		rcc2 &= ~(_BV(DIV400));
	}
	
	if (!(cfg & _BV(BYPASS))) {
		/* wait until the PLL has locked */
		for (loop = 32768; loop > 0; loop--) {
			if (__raw_readl(RIS) & _BV(PLLL))
				break;
		}
		/* enable use of the PLL */
		rcc &= ~(_BV(BYPASS));
		rcc2 &= ~(_BV(BYPASS2));
	}
	
	/* write the final RCC value */
	__raw_writel(rcc, RCC);
	__raw_writel(rcc2, RCC2);
	
	/* wait for a bit so that the system divider can take effect */
	__delay(16);
}
