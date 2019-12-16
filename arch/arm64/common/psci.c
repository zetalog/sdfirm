#include <target/types.h>
#include <target/barrier.h>
#include <target/smp.h>

#define PSCI_VER		((1 << 16) | 1)

#define PSCI_VERSION		0x84000000
#define PSCI_CPU_ON		0xC4000003

#define PSCI_SUCCESS		0
#define PSCI_NOT_SUPPORTED	1
#define PSCI_INVALID_PARAM	2
#define PSCI_DENIED		3

#define INVALID_ADDR		0

static uint64_t psci_cpu_on(uint64_t cpu, uint64_t ep, uint64_t context)
{
	cpu_spin_table[cpu & 0xff] = ep;
	cpu_context_table[cpu & 0xff] = context;
	__asm("dsb sy");
	__asm("sev");
	return PSCI_SUCCESS;
}

uint64_t psci_cmd(uint32_t fid, uint64_t x1, uint64_t x2,
		  uint64_t x3, uint64_t x4)
{
	uint64_t ret;

	switch (fid) {
	case PSCI_VERSION:
		ret = PSCI_VER;
		break;
	case PSCI_CPU_ON:
		ret = psci_cpu_on(x1, x2, x3);
		break;
	default:
		ret = -PSCI_NOT_SUPPORTED;
		break;
	}
	return ret;
}
