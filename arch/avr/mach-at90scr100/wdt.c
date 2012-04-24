#include <target/wdt.h>
#include <target/irq.h>

#define __wdt_hw_ctrl_enable(tout)					\
	do {								\
		wdr();							\
		WDTCSR = (uint8_t)(_BV(WDCE) | _BV(WDE));		\
		WDTCSR = (uint8_t)((tout & 0x08 ? _BV(WDP3) : 0x00) |	\
				   _BV(WDE) | (tout & 0x07));		\
	} while (0)
#define __wdt_hw_ctrl_disable()						\
	do {								\
		WDTCSR = (uint8_t)(_BV(WDCE) | _BV(WDE));		\
		WDTCSR = 0;						\
	} while (0)
#define __wdt_hw_ack_reset()		(MCUSR = ~(_BV(WDRF)))

void wdt_hw_ctrl_start(uint8_t tout)
{
	irq_flags_t flags;

	__wdt_hw_ack_reset();
	irq_local_save(flags);
	__wdt_hw_ctrl_enable(tout);
	irq_local_restore(flags);
}

void wdt_hw_ctrl_stop(void)
{
	irq_flags_t flags;

	__wdt_hw_ack_reset();
	irq_local_save(flags);
	__wdt_hw_ctrl_disable();
	irq_local_restore(flags);
}
