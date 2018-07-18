#include <target/gpt.h>
#include <target/irq.h>
#include <asm/timer.h>

uint64_t __systick_read(void)
{
	return read_sysreg(CNTPCT_EL0);
}

boolean __systick_poll(void)
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

	match_val_ticks = match_val * TICKS_TO_MICROSECONDS;
	write_sysreg(match_val_ticks, CNTP_CVAL_EL0);
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

void __systick_init(void)
{
	write_sysreg(SYSTICK_HW_FREQUENCY, CNTFRQ_EL0);
	write_sysreg(0, CNTVOFF_EL2);
	write_sysreg(CNTHCTL_EL1PCEN | CNTHCTL_EL1PCTEN, CNTHCTL_EL2);
	write_sysreg(CNTKCTL_EL0PCTEN, CNTKCTL_EL1);
	write_sysreg(CNTX_CTL_ENABLE, CNTP_CTL_EL0);
	write_sysreg(-1, CNTP_CVAL_EL0);
}
