#ifndef __PMU_RI5CY_H_INCLUDE__
#define __PMU_RI5CY_H_INCLUDE__

#include <target/arch.h>

#define CSR_PCCR(n)		(0x780 + (n))
#ifdef CONFIG_CPU_CV32E
#define CSR_PCER		0x7E0
#define CSR_PCMR		0x7E1
#else
#define CSR_PCER		0x7A0
#define CSR_PCMR		0x7A1
#endif

/* Performance Counter Mode Register (PCMR) */
#define PC_GLOBAL_ENABLE	_AC(0x00000001, UL)
#define PC_SATURATION		_AC(0x00000002, UL)

/* Performance Counter Event Register (PCER) */
#define PC_CYCLES		0
#define PC_INSTR		1
#define PC_LD_STALL		2
#define PC_JMP_STALL		3
#define PC_IMISS		4
#define PC_LD			5
#define PC_ST			6
#define PC_JUMP			7
#define PC_BRANCH		8
#define PC_BRANCH_TAKEN		9
#define PC_COMP_INSTR		10
#define PC_LD_EXT		11
#define PC_ST_EXT		12
#define PC_LD_EXT_CYC		13
#define PC_ST_EXT_CYC		14
#define PC_TCDM_CONT		15
#define PC_CSR_HAZARD		16
#define PC_FP_TYPE		17
#define PC_FP_CONT		18
#define PC_FP_DEP		19
#define PC_FP_WB		20

#define PC_ID(event)		(_AC(0x1, UL) << (event))

#ifdef CONFIG_RI5CY_ASIC
#define RI5CY_MAX_COUNTERS		1
#else
#define RI5CY_MAX_COUNTERS		21
#endif
#define ri5cy_get_counters()		RI5CY_MAX_COUNTERS

#define ri5cy_enable_event()		csr_set(CSR_PCMR, PC_GLOBAL_ENALBE)
#define ri5cy_disable_event()		csr_clear(CSR_PCMR, PC_GLOBAL_ENALBE)
#define ri5cy_config_wraparound()	csr_clear(CSR_PCMR, PC_SATURATION)
#define ri5cy_config_saturation()	csr_set(CSR_PCMR, PC_SATURATION)

#define ri5cy_get_count(event)		csr_read(CSR_PCCR(event))
#define ri5cy_enable_count(event)	csr_set(CSR_PCER, PC_ID(event))
#define ri5cy_disable_count(event)	csr_clear(CSR_PCER, PC_ID(event))

#ifndef __ASSEMBLY__
void ri5cy_set_count(uint8_t event, uint32_t count);
#endif

#endif /* __PMU_RI5CY_H_INCLUDE__ */
