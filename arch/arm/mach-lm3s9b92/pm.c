#include <target/generic.h>
#include <target/arch.h>

void pm_hw_suspend_device(uint8_t dev, uint8_t mode)
{
	if (mode > DEV_MODE_SLP)
		__raw_clearl(PM_GATE_V(dev), PM_GATE_A(DCGC0, dev));
	if (mode > DEV_MODE_ON)
		__raw_clearl(PM_GATE_V(dev), PM_GATE_A(SCGC0, dev));
	__raw_clearl(PM_GATE_V(dev), PM_GATE_A(RCGC0, dev));
}

void pm_hw_resume_device(uint8_t dev, uint8_t mode)
{
	if (mode > DEV_MODE_SLP)
		__raw_setl(PM_GATE_V(dev), PM_GATE_A(DCGC0, dev));
	if (mode > DEV_MODE_ON)
		__raw_setl(PM_GATE_V(dev), PM_GATE_A(SCGC0, dev));
	__raw_setl(PM_GATE_V(dev), PM_GATE_A(RCGC0, dev));
}

uint8_t pm_hw_device_mode(uint8_t dev)
{
	if (__raw_testl(PM_GATE_V(dev), PM_GATE_A(RCGC0, dev)))
		return DEV_MODE_ON;
	if (__raw_testl(PM_GATE_V(dev), PM_GATE_A(SCGC0, dev)))
		return DEV_MODE_SLP;
	if (__raw_testl(PM_GATE_V(dev), PM_GATE_A(DCGC0, dev)))
		return DEV_MODE_DSLP;
	return DEV_MODE_OFF;
}

void pm_hw_reset_device(uint8_t dev)
{
	__raw_setl(PM_GATE_V(dev), PM_GATE_A(SRCR0, dev));
	__raw_clearl(PM_GATE_V(dev), PM_GATE_A(SRCR0, dev));
}
