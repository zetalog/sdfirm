#include <target/i3c.h>
#include <target/panic.h>
#include <target/bh.h>

#ifdef CONFIG_I3C_DEBUG
#define i3c_dbg(...)		con_dbg(__VA_ARGS__)
#else
#define i3c_dbg(...)		do { } while (0)
#endif

#ifdef SYS_REALTIME
#define i3c_poll_init()		__i3c_poll_init()
#define i3c_irq_init()		do { } while (0)
#else /* SYS_REALTIME */
#define i3c_poll_init()		do { } while (0)
#define i3c_irq_init()		__i3c_irq_init()
#endif /* SYS_REALTIME */

bh_t i3c_bh;

static void i3c_handle_bh(void)
{
	i3c_t i3c;
	__unused i3c_t si3c;

	for (i3c = 0; i3c < NR_I3C_MASTERS; i3c++) {
		si3c = i3c_master_save(i3c);
		i3c_handle_xfr();
		i3c_master_restore(si3c);
	}
}

static void i3c_bh_handler(uint8_t events)
{
	if (events == BH_POLLIRQ)
		i3c_handle_irq();
	else
		i3c_handle_bh();
}

#ifdef SYS_REALTIME
static void __i3c_poll_init(void)
{
	irq_register_poller(i3c_bh);
}
#else
static void __i3c_irq_init(void)
{
	i3c_hw_irq_init();
}
#endif

void i3c_init(void)
{
	i3c_t i3c;
	__unused i3c_t si3c;

	i3c_bh = bh_register_handler(i3c_bh_handler);
	for (i3c = 0; i3c < NR_I3C_MASTERS; i3c++) {
		si3c = i3c_master_save(i3c);
		//i3c_set_status(I3C_STATUS_IDLE);
		i3c_master_init();
		i3c_irq_init();
		i3c_transfer_reset();
		i3c_master_restore(si3c);
	}
	i3c_poll_init();
}
