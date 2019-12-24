#include <target/timer.h>
#include <target/jiffies.h>
#include <target/irq.h>

uint64_t __systick_read(void)
{
	return read_sysreg(CNTPCT_EL0);
}

bool __systick_poll(void)
{
	uint64_t val;

	val = read_sysreg(CNTP_CTL_EL0);
	if (val & CNTX_CTL_ISTATUS) {
		val &= ~CNTX_CTL_ISTATUS;
		write_sysreg(val, CNTP_CTL_EL0);
		return true;
	}
	return false;
}

void __systick_set_timeout(timeout_t match_val)
{
	uint64_t match_val_ticks;

	match_val_ticks = match_val * TSC_FREQ;
	write_sysreg(match_val_ticks, CNTP_TVAL_EL0);
}

void __systick_mask_irq(void)
{
	uint32_t value;

	value = read_sysreg(CNTP_CTL_EL0);
	value |= CNTX_CTL_IMASK;
	write_sysreg(value, CNTP_CTL_EL0);
}

void __systick_unmask_irq(void)
{
	uint32_t value;

	value = read_sysreg(CNTP_CTL_EL0);
	value &= ~CNTX_CTL_IMASK;
	write_sysreg(value, CNTP_CTL_EL0);
}

static bool systick_done = false;

void __systick_init(void)
{
	if (systick_done)
		return;
	systick_done = true;
	write_sysreg(SYSTICK_HW_FREQUENCY, CNTFRQ_EL0);
	write_sysreg(CNTKCTL_EL0PCTEN, CNTKCTL_EL1);
	write_sysreg(CNTX_CTL_ENABLE, CNTP_CTL_EL0);
	write_sysreg(-1, CNTP_TVAL_EL0);
}
